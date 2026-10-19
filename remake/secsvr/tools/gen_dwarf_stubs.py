#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
gen_dwarf_stubs.py -- 从 DWARF 调试信息生成 C++ 桩源码。

用途：对带完整 DWARF 的服务端二进制（secsvr 三件套等）还原全部编译单元与头文件，
恢复类型信息（namespace/class/struct/union/enum/typedef/成员变量/方法签名）与
参数/局部变量名字；函数体保持为空（"具体的内容暂时不要动"）。

用法：
    python3 gen_dwarf_stubs.py <原始 ELF> <输出根目录>

输出布局（镜像 DWARF 中的原始路径，去掉 CI workspace 前缀）：
    <out>/src/...        项目源码（.cpp 桩 + .h 桩）
    <out>/include/...    加密库头（4.1.2 CU 相对路径）
    <out>/output/...     构建产物 include（zenlib/framework/protocol 安装头）
    <out>/import/...     第三方/内部 SDK 头（多为空桩）

依赖：pyelftools。
"""

from __future__ import print_function

import argparse
import os
import re
import sys
from collections import OrderedDict, defaultdict

from elftools.elf.elffile import ELFFile


# ---------------------------------------------------------------------------
# 基础工具
# ---------------------------------------------------------------------------

def _b(s):
    return s.decode("utf-8", "replace") if isinstance(s, bytes) else str(s)


def attr_str(die, name):
    a = die.attributes.get(name)
    if a is None:
        return None
    v = a.value
    return _b(v)


def attr_int(die, name):
    a = die.attributes.get(name)
    if a is None:
        return None
    v = a.value
    if isinstance(v, bytes):
        v = int.from_bytes(v, "little", signed=False)
    try:
        return int(v)
    except Exception:
        return None


SYSTEM_PREFIXES = ("/usr/", "/usr/lib", "/usr/src", "/usr/local", "/lib/", "/lib64")


def is_system_path(p):
    return p is None or p.startswith(SYSTEM_PREFIXES)


def strip_workspace(path):
    """CI 绝对路径 -> 镜像相对路径（去掉 /workspace/<tag>/ 或 /data/ 前缀）。"""
    m = re.search(r"/workspace/[^/]+/", path)
    if m:
        return path[m.end():]
    if path.startswith("/data/"):
        return path[len("/data/"):]
    return path.lstrip("/")


def system_include(path):
    """系统头路径 -> #include 形式。"""
    if not path:
        return None
    if path.startswith("/usr/lib/gcc"):
        return "<%s>" % os.path.basename(path)
    if path.startswith("/usr/include/c++/"):
        rest = path[len("/usr/include/c++/"):]
        parts = rest.split("/", 1)
        return "<%s>" % (parts[1] if len(parts) > 1 else parts[0])
    if path.startswith("/usr/include/"):
        return "<%s>" % path[len("/usr/include/"):]
    if path.startswith("/usr/"):
        return "<%s>" % os.path.basename(path)
    return None


HEADER_EXTS = (".h", ".hpp", ".hh", ".inl", ".tcc", ".hxx")
CPP_EXTS = (".cpp", ".cc", ".cxx", ".c", ".C")


# ---------------------------------------------------------------------------
# 文件路径解析（DWARF line program file table）
# ---------------------------------------------------------------------------

class FileResolver(object):
    def __init__(self, cu):
        self.cu = cu
        self.lp = None
        self.files = []
        self.dirs = []
        self.base = None
        top = cu.get_top_DIE()
        self.cu_name = attr_str(top, "DW_AT_name") or ""
        self.comp_dir = attr_str(top, "DW_AT_comp_dir")
        try:
            self.lp = cu.dwarfinfo.line_program_for_CU(cu)
        except Exception:
            self.lp = None
        if self.lp is not None:
            h = self.lp.header
            self.files = h["file_entry"]
            self.dirs = [_b(d) for d in h["include_directory"]]
        self._compute_base()

    def _compute_base(self):
        if not self.dirs:
            self.base = os.path.dirname(self.cu_name) if self.cu_name else "."
            return
        d0 = self.dirs[0]
        if os.path.isabs(d0):
            self.base = self.comp_dir or d0
            return
        # 相对目录：用 CU 名反推 workspace 根
        if self.cu_name and os.path.isabs(self.cu_name):
            for fe in self.files:
                nm = _b(fe.name)
                if fe.dir_index <= len(self.dirs):
                    dd = self.dirs[fe.dir_index - 1] if fe.dir_index > 0 else d0
                    cand = os.path.normpath(os.path.join(dd, nm))
                    if self.cu_name.endswith(cand) and len(cand) < len(self.cu_name):
                        self.base = self.cu_name[:-len(cand)]
                        return
            self.base = os.path.dirname(self.cu_name)
        else:
            self.base = ""

    def resolve(self, idx):
        if self.lp is None or idx is None or idx <= 0 or idx > len(self.files):
            return None
        fe = self.files[idx - 1]
        name = _b(fe.name)
        di = fe.dir_index
        if di == 0:
            d = self.comp_dir or (self.dirs[0] if self.dirs else ".")
        elif di <= len(self.dirs):
            d = self.dirs[di - 1]
        else:
            return None
        if os.path.isabs(name):
            return os.path.normpath(name)
        if os.path.isabs(d):
            return os.path.normpath(os.path.join(d, name))
        if self.base:
            return os.path.normpath(os.path.join(self.base, d, name))
        return os.path.normpath(os.path.join(d, name))

    def mirror_path(self, idx):
        p = self.resolve(idx)
        if p is None or is_system_path(p):
            return None
        return strip_workspace(p)


# ---------------------------------------------------------------------------
# DIE 记录与登记
# ---------------------------------------------------------------------------

CLASS_KINDS = {
    "DW_TAG_structure_type": "struct",
    "DW_TAG_class_type": "class",
    "DW_TAG_union_type": "union",
}

ACCESS = {1: "public", 2: "protected", 3: "private"}


def cu_id(cu):
    """cu 可以是 CU 对象或 CU 记录 dict。"""
    return id(cu["cu"]) if isinstance(cu, dict) else id(cu)


class Registry(object):
    def __init__(self):
        self.named_types = OrderedDict()   # (kind, qname) -> entry
        self.free_funcs = OrderedDict()    # qname -> [records]
        self.defns = OrderedDict()         # qname -> [records with low_pc]
        self.globals = OrderedDict()       # qname -> [records]
        self.spec_to_defn = {}             # (cu_obj, spec_ref) -> defn record
        self.cu_dies = {}                  # id(cu_obj) -> {abs_off: rec}
        self.type_file = {}                # qname -> mirror path
        self.typedef_cache = {}

    # -- 类型查询 ---------------------------------------------------------

    def _find_named(self, kind, name):
        for key, rec in self.named_types.items():
            if rec["kind"] == kind and rec["name"] == name:
                return rec
        return None

    def type_qname(self, die, cu):
        nm = attr_str(die, "DW_AT_name")
        if not nm:
            return None
        if die.tag in CLASS_KINDS:
            kind = "class"
        elif die.tag == "DW_TAG_enumeration_type":
            kind = "enum"
        else:
            kind = die.tag
        rec = self._find_named(kind, nm)
        if rec:
            return rec["qname"]
        return nm

    def typedef_qname(self, die, cu):
        key = (cu_id(cu), id(die))
        if key in self.typedef_cache:
            return self.typedef_cache[key]
        nm = attr_str(die, "DW_AT_name")
        rec = self._find_named("typedef", nm) if nm else None
        qn = rec["qname"] if rec else (nm or "?")
        self.typedef_cache[key] = qn
        return qn


class TypeRenderer(object):
    def __init__(self, reg):
        self.reg = reg

    def die_at(self, cu, ref):
        if ref is None:
            return None
        rec = self.reg.cu_dies.get(cu_id(cu), {})
        base = cu.cu_offset if not isinstance(cu, dict) else cu["cu"].cu_offset
        r = rec.get(base + ref)
        return r["die"] if r else None

    def rec_at(self, cu, ref):
        if ref is None:
            return None
        rec = self.reg.cu_dies.get(cu_id(cu), {})
        base = cu.cu_offset if not isinstance(cu, dict) else cu["cu"].cu_offset
        return rec.get(base + ref)

    # -- 类型渲染 ---------------------------------------------------------

    def render(self, die, cu, name=""):
        """把类型 DIE 渲染成 C++ 声明片段。name 为被声明对象名。"""
        if die is None:
            return ("void " + name).strip() if name else "void"
        tag = die.tag
        if tag == "DW_TAG_base_type":
            return ((attr_str(die, "DW_AT_name") or "int") + (" " + name if name else "")).strip()
        if tag in ("DW_TAG_const_type", "DW_TAG_volatile_type"):
            inner = self.die_at(cu, attr_int(die, "DW_AT_type"))
            cv = "const" if tag == "DW_TAG_const_type" else "volatile"
            if inner is not None and inner.tag in (
                    "DW_TAG_pointer_type", "DW_TAG_reference_type", "DW_TAG_ptr_to_member_type"):
                if name:
                    return self.render(inner, cu, "%s %s" % (cv, name))
                return ("const " + self.render(inner, cu)).strip()
            if name:
                return ("%s %s" % (cv, self.render(inner, cu, name))).strip()
            return ("%s %s" % (cv, self.render(inner, cu))).strip()
        if tag == "DW_TAG_pointer_type":
            inner = self.die_at(cu, attr_int(die, "DW_AT_type"))
            return self.render(inner, cu, "*" + name if name else "*")
        if tag == "DW_TAG_reference_type":
            inner = self.die_at(cu, attr_int(die, "DW_AT_type"))
            return self.render(inner, cu, "&" + name if name else "&")
        if tag == "DW_TAG_array_type":
            inner = self.die_at(cu, attr_int(die, "DW_AT_type"))
            dims = []
            for ch in die.iter_children():
                if ch.tag == "DW_TAG_subrange_type":
                    cnt = attr_int(ch, "DW_AT_count")
                    if cnt is None:
                        ub = attr_int(ch, "DW_AT_upper_bound")
                        lb = attr_int(ch, "DW_AT_lower_bound")
                        cnt = (ub - lb + 1) if (ub is not None and lb is not None) else None
                    dims.append("[%d]" % cnt if cnt is not None else "[]")
            if not dims:
                dims = ["[]"]
            return self.render(inner, cu, name + "".join(dims))
        if tag == "DW_TAG_subroutine_type":
            ret = self.return_type(die, cu)
            args = self.param_list(die, cu)
            return ("%s (*%s)(%s)" % (ret, name, args)) if name else ("%s (*)(%s)" % (ret, args))
        if tag == "DW_TAG_ptr_to_member_type":
            cont = self.die_at(cu, attr_int(die, "DW_AT_containing_type"))
            inner = self.die_at(cu, attr_int(die, "DW_AT_type"))
            cls = self.render_name(cont, cu) if cont else "?"
            if inner is not None and inner.tag == "DW_TAG_subroutine_type":
                ret = self.return_type(inner, cu)
                args = self.param_list(inner, cu)
                return ("%s (%s::*%s)(%s)" % (ret, cls, name, args)) if name else \
                    ("%s (%s::*)(%s)" % (ret, cls, args))
            return ("int %s::*%s" % (cls, name)).strip() if name else ("int %s::*" % cls)
        if tag == "DW_TAG_typedef":
            qn = self.reg.typedef_qname(die, cu)
            return (qn + (" " + name if name else "")).strip()
        if tag in CLASS_KINDS:
            qn = self.reg.type_qname(die, cu)
            if qn:
                return (qn + (" " + name if name else "")).strip()
            return ("/*anon struct*/ int %s" % name).strip() if name else "/*anon struct*/ int"
        if tag == "DW_TAG_enumeration_type":
            qn = self.reg.type_qname(die, cu)
            if qn:
                return (qn + (" " + name if name else "")).strip()
            return ("/*anon enum*/ int %s" % name).strip() if name else "/*anon enum*/ int"
        return ("/*%s*/ int %s" % (tag.replace("DW_TAG_", ""), name)).strip()

    def render_name(self, die, cu):
        if die is None:
            return "?"
        if die.tag == "DW_TAG_typedef":
            return self.reg.typedef_qname(die, cu)
        if die.tag in CLASS_KINDS or die.tag == "DW_TAG_enumeration_type":
            return self.reg.type_qname(die, cu) or "?"
        if die.tag == "DW_TAG_base_type":
            return attr_str(die, "DW_AT_name") or "?"
        return self.render(die, cu).strip()

    def return_type(self, fdie, cu):
        d = self.die_at(cu, attr_int(fdie, "DW_AT_type"))
        return self.render(d, cu).strip() or "void"

    def param_list(self, fdie, cu):
        """仅用 DIE 自带名字渲染形参列表（声明通常无名字 -> argN）。"""
        parts = []
        for ch in fdie.iter_children():
            if ch.tag == "DW_TAG_formal_parameter":
                if ch.attributes.get("DW_AT_artificial"):
                    continue
                d = self.die_at(cu, attr_int(ch, "DW_AT_type"))
                pname = attr_str(ch, "DW_AT_name") or "arg%d" % len(parts)
                parts.append(self.render(d, cu, pname).strip() if d else pname)
            elif ch.tag == "DW_TAG_unspecified_parameters":
                parts.append("...")
        return ", ".join(parts)

    def param_names(self, fdie, cu):
        """返回非 artificial 形参名字列表（用于定义 DIE -> 声明 DIE 名字对齐）。"""
        out = []
        for ch in fdie.iter_children():
            if ch.tag == "DW_TAG_formal_parameter" and not ch.attributes.get("DW_AT_artificial"):
                out.append(attr_str(ch, "DW_AT_name") or "")
        return out


# ---------------------------------------------------------------------------
# CU 分析
# ---------------------------------------------------------------------------

def iter_dies(die):
    yield die
    for ch in die.iter_children():
        for d in iter_dies(ch):
            yield d


def _qname(scope, name):
    parts = [p for p in scope if p and p != "::"]
    return "::".join(parts + [name]) if parts else name


def _register_named(reg, kind, r, nm):
    key = (kind, _qname(r["scope"], nm))
    entry = reg.named_types.get(key)
    if entry is None:
        entry = {"kind": kind, "name": nm, "qname": key[1], "file": r["decl_file"],
                 "die": r["die"], "cu": r["cu"], "scope": r["scope"],
                 "declaration": "DW_AT_declaration" in r["die"].attributes}
        reg.named_types[key] = entry
        if r["decl_file"]:
            reg.type_file[key[1]] = r["decl_file"]
    elif entry["declaration"] and "DW_AT_declaration" not in r["die"].attributes:
        entry["declaration"] = False
        entry["die"] = r["die"]
        entry["cu"] = r["cu"]
        entry["first_scope"] = r["scope"]


def _register_class(reg, r, nm):
    key = ("class", _qname(r["scope"], nm))
    entry = reg.named_types.get(key)
    if entry is None:
        entry = {"kind": "class", "name": nm, "qname": key[1], "file": r["decl_file"],
                 "die": r["die"], "cu": r["cu"], "scope": r["scope"],
                 "declaration": "DW_AT_declaration" in r["die"].attributes}
        reg.named_types[key] = entry
        if r["decl_file"]:
            reg.type_file[key[1]] = r["decl_file"]
    if "DW_AT_declaration" in r["die"].attributes:
        return
    if entry["declaration"]:
        entry["declaration"] = False
        entry["die"] = r["die"]
        entry["cu"] = r["cu"]
        return
    cur_n = sum(1 for _ in entry["die"].iter_children())
    new_n = sum(1 for _ in r["die"].iter_children())
    if new_n > cur_n:
        entry["die"] = r["die"]
        entry["cu"] = r["cu"]


def _register_function(reg, r):
    d = r["die"]
    spec = attr_int(d, "DW_AT_specification")
    if spec is not None:
        if "DW_AT_low_pc" in d.attributes:
            reg.spec_to_defn[(r["cu"], spec)] = r
        return
    nm = attr_str(d, "DW_AT_name")
    if not nm:
        return
    if r["parent"] is not None and r["parent"]["die"].tag in CLASS_KINDS:
        return  # 类成员方法：由类定义统一输出
    if nm.startswith("__tcf_") or nm.startswith("_GLOBAL__I_") or \
            nm == "__static_initialization_and_destruction_0":
        return
    qn = _qname(r["scope"], nm)
    reg.free_funcs.setdefault(qn, []).append(r)
    if "DW_AT_low_pc" in d.attributes and not is_system_path(r["decl_file"]):
        reg.defns.setdefault(qn, []).append(r)


def _register_variable(reg, r):
    d = r["die"]
    nm = attr_str(d, "DW_AT_name")
    if not nm:
        return
    if r["parent"] is not None and r["parent"]["die"].tag in CLASS_KINDS:
        return  # 静态数据成员由类输出
    qn = _qname(r["scope"], nm)
    reg.globals.setdefault(qn, []).append(r)


def analyze(elf_path):
    reg = Registry()
    cus = []
    with open(elf_path, "rb") as f:
        elf = ELFFile(f)
        if not elf.has_dwarf_info():
            print("no DWARF in %s" % elf_path, file=sys.stderr)
            sys.exit(2)
        dwarf = elf.get_dwarf_info()
        for cu in dwarf.iter_CUs():
            top = cu.get_top_DIE()
            prod = attr_str(top, "DW_AT_producer") or ""
            if "GNU C++" not in prod:
                continue
            res = FileResolver(cu)
            rec = {"cu": cu, "top": top, "resolver": res,
                   "name": attr_str(top, "DW_AT_name") or "",
                   "producer": prod, "dies": [], "by_off": {}}
            cus.append(rec)
            reg.cu_dies[id(cu)] = rec["by_off"]
            _walk_cu(reg, rec)
    return cus, reg


def _walk_cu(reg, rec):
    cu = rec["cu"]
    by_off = rec["by_off"]

    def build(die, parent, scope):
        r = {"die": die, "cu": cu, "parent": parent, "scope": tuple(scope)}
        df = attr_int(die, "DW_AT_decl_file")
        r["decl_file"] = rec["resolver"].mirror_path(df) if df else None
        r["abs_file"] = rec["resolver"].resolve(df) if df else None
        r["children"] = []
        if parent is not None:
            parent["children"].append(r)
        by_off[die.offset] = r
        rec["dies"].append(r)
        new_scope = list(scope)
        if die.tag == "DW_TAG_namespace" and attr_str(die, "DW_AT_name"):
            new_scope.append(attr_str(die, "DW_AT_name"))
        elif die.tag in CLASS_KINDS and attr_str(die, "DW_AT_name"):
            new_scope.append(attr_str(die, "DW_AT_name"))
        for ch in die.iter_children():
            build(ch, r, new_scope)

    build(rec["top"], None, [])

    for r in rec["dies"]:
        d = r["die"]
        if d.tag in CLASS_KINDS:
            nm = attr_str(d, "DW_AT_name")
            if nm and not is_system_path(r["decl_file"]):
                _register_class(reg, r, nm)
        elif d.tag == "DW_TAG_enumeration_type":
            nm = attr_str(d, "DW_AT_name")
            if nm and not is_system_path(r["decl_file"]):
                _register_named(reg, "enum", r, nm)
        elif d.tag == "DW_TAG_typedef":
            nm = attr_str(d, "DW_AT_name")
            if nm and not is_system_path(r["decl_file"]):
                _register_named(reg, "typedef", r, nm)
    for r in rec["dies"]:
        d = r["die"]
        if d.tag == "DW_TAG_subprogram":
            _register_function(reg, r)
        elif d.tag == "DW_TAG_variable":
            _register_variable(reg, r)


# ---------------------------------------------------------------------------
# 输出
# ---------------------------------------------------------------------------

def header_guard(path):
    s = re.sub(r"[^A-Za-z0-9]+", "_", path).strip("_").upper()
    return "SECSVR_%s_H_" % s


def clean_scope(scope):
    return tuple(s for s in scope if s and s != "::")


class Emitter(object):
    def __init__(self, reg, cus, out_root):
        self.reg = reg
        self.cus = cus
        self.out_root = out_root
        self.renderer = TypeRenderer(reg)
        self.cu_recs = {id(c["cu"]): c for c in cus}
        # 声明 DIE 绝对偏移 -> 定义 DIE record（用于取参数名）
        self.decl_to_defn = {}
        for cu in cus:
            for r in cu["dies"]:
                d = r["die"]
                spec = attr_int(d, "DW_AT_specification")
                if spec is not None and "DW_AT_low_pc" in d.attributes:
                    self.decl_to_defn[r["cu"].cu_offset + spec] = r

    # -- 文件内容收集 -----------------------------------------------------

    def header_items(self, fpath):
        items = []
        for key, entry in self.reg.named_types.items():
            if entry["file"] == fpath and not entry["declaration"]:
                items.append(("type", entry))
        for qn, lst in self.reg.free_funcs.items():
            for r in lst:
                if r["decl_file"] == fpath and (
                        "DW_AT_declaration" in r["die"].attributes or
                        "DW_AT_low_pc" not in r["die"].attributes):
                    items.append(("func", r, qn))
                    break
        for qn, lst in self.reg.globals.items():
            for r in lst:
                if r["decl_file"] == fpath and "DW_AT_location" not in r["die"].attributes:
                    items.append(("var", r, qn))
                    break
        return items

    def write_all(self):
        written = set()
        counts = {"headers": 0, "cpps": 0, "types": 0, "funcs": 0, "vars": 0}
        header_files = set()
        for e in self.reg.named_types.values():
            if e["file"] and e["file"].endswith(HEADER_EXTS):
                header_files.add(e["file"])
        for lst in self.reg.free_funcs.values():
            for r in lst:
                if r["decl_file"] and r["decl_file"].endswith(HEADER_EXTS):
                    header_files.add(r["decl_file"])
        for lst in self.reg.globals.values():
            for r in lst:
                if r["decl_file"] and r["decl_file"].endswith(HEADER_EXTS):
                    header_files.add(r["decl_file"])
        for fpath in sorted(header_files):
            self.write_header(fpath)
            its = self.header_items(fpath)
            counts["headers"] += 1
            counts["types"] += sum(1 for i in its if i[0] == "type")
            counts["funcs"] += sum(1 for i in its if i[0] == "func")
            counts["vars"] += sum(1 for i in its if i[0] == "var")
            written.add(fpath)

        for cu in self.cus:
            fpath = strip_workspace(cu["name"]) if cu["name"] else None
            if not fpath or not fpath.endswith(CPP_EXTS):
                continue
            self.write_cpp(cu, fpath)
            counts["cpps"] += 1
            written.add(fpath)

        for cu in self.cus:
            res = cu["resolver"]
            if res.lp is None:
                continue
            for i in range(1, len(res.files) + 1):
                p = res.mirror_path(i)
                if p and p not in written and p.endswith(HEADER_EXTS):
                    self.write_empty_header(p)
                    written.add(p)
                    counts["headers"] += 1
        return counts

    # -- 路径工具 ---------------------------------------------------------

    def out_path(self, fpath):
        return os.path.join(self.out_root, fpath)

    def ensure_dir(self, fpath):
        p = self.out_path(fpath)
        os.makedirs(os.path.dirname(p) or ".", exist_ok=True)

    # -- 头文件 -----------------------------------------------------------

    def write_header(self, fpath):
        items = self.header_items(fpath)
        guard = header_guard(fpath)
        lines = [
            "// Auto-generated header stub from DWARF info",
            "// Original path: %s" % fpath,
            "// 内容为类型信息与声明（函数体暂未还原）。",
            "#ifndef %s" % guard,
            "#define %s" % guard,
            "",
        ]
        incs = self.header_includes(fpath, items)
        for inc in sorted(incs):
            lines.append(inc)
        if incs:
            lines.append("")
        tree = defaultdict(list)
        for it in items:
            scope = it[1]["scope"] if it[0] == "type" else it[1]["scope"]
            tree[clean_scope(scope)].append(it)
        self.emit_scope_tree(tree, lines, 0)
        lines.append("")
        lines.append("#endif // %s" % guard)
        self.ensure_dir(fpath)
        with open(self.out_path(fpath), "w", encoding="utf-8", newline="\n") as fo:
            fo.write("\n".join(lines) + "\n")

    def header_includes(self, fpath, items):
        """推断头文件需要的 include：引用类型的 decl_file + 系统头。"""
        incs = set()

        def collect(die, cu):
            for d in iter_dies(die):
                off = attr_int(d, "DW_AT_type")
                if off is not None:
                    rr = self.renderer.rec_at(cu, off)
                    if rr is not None:
                        if rr["abs_file"] and is_system_path(rr["abs_file"]):
                            si = system_include(rr["abs_file"])
                            if si:
                                incs.add("#include %s" % si)
                        elif rr["decl_file"] and rr["decl_file"] != fpath:
                            if rr["decl_file"].endswith(HEADER_EXTS):
                                incs.add('#include "%s"' % rr["decl_file"])
                            else:
                                si = system_include(rr["abs_file"]) if rr["abs_file"] else None
                                if si:
                                    incs.add("#include %s" % si)

        for it in items:
            if it[0] == "type":
                entry = it[1]
                collect(entry["die"], entry["cu"])
                # 基类
                for ch in entry["die"].iter_children():
                    if ch.tag == "DW_TAG_inheritance":
                        rr = self.renderer.rec_at(entry["cu"], attr_int(ch, "DW_AT_type"))
                        if rr:
                            if rr["abs_file"] and is_system_path(rr["abs_file"]):
                                si = system_include(rr["abs_file"])
                                if si:
                                    incs.add("#include %s" % si)
                            elif rr["decl_file"] and rr["decl_file"] != fpath:
                                incs.add('#include "%s"' % rr["decl_file"])
            else:
                r = it[1]
                collect(r["die"], r["cu"])
        return incs

    def write_empty_header(self, fpath):
        guard = header_guard(fpath)
        lines = [
            "// Auto-generated empty header stub from DWARF line info",
            "// Original path: %s" % fpath,
            "#ifndef %s" % guard,
            "#define %s" % guard,
            "",
            "#endif // %s" % guard,
            "",
        ]
        self.ensure_dir(fpath)
        with open(self.out_path(fpath), "w", encoding="utf-8", newline="\n") as fo:
            fo.write("\n".join(lines))

    # -- scope 树 ---------------------------------------------------------

    def emit_scope_tree(self, tree, lines, indent):
        children = defaultdict(dict)
        here = []
        for scope, its in tree.items():
            if len(scope) == 0:
                here.extend(its)
            else:
                children[scope[0]][scope[1:]] = its
        pad = "    " * indent
        if here:
            self.emit_header_items(here, lines, indent)
        for ns in sorted(children):
            lines.append("%snamespace %s {" % (pad, ns))
            self.emit_scope_tree(children[ns], lines, indent + 1)
            lines.append("%s} // namespace %s" % (pad, ns))

    def emit_header_items(self, items, lines, indent):
        for it in items:
            if it[0] == "type":
                self.emit_type(it[1], lines, indent)
            elif it[0] == "func":
                self.emit_free_func_decl(it[1], it[2], lines, indent)
            elif it[0] == "var":
                self.emit_global_decl(it[1], it[2], lines, indent)

    # -- 类型输出 ---------------------------------------------------------

    def emit_type(self, entry, lines, indent):
        d = entry["die"]
        cu = entry["cu"]
        pad = "    " * indent
        kind = entry["kind"]
        if kind == "typedef":
            self.emit_typedef(d, cu, entry, lines, indent)
            return
        if kind == "enum":
            self.emit_enum(d, cu, entry, lines, indent)
            return
        kw = CLASS_KINDS.get(d.tag, "struct")
        size = attr_int(d, "DW_AT_byte_size")
        if size is not None:
            lines.append("%s// sizeof = %d" % (pad, size))
        dl = attr_int(d, "DW_AT_decl_line")
        cmt = " // line %d" % dl if dl is not None else ""
        bases = []
        for ch in d.iter_children():
            if ch.tag == "DW_TAG_inheritance":
                base = self.renderer.die_at(cu, attr_int(ch, "DW_AT_type"))
                base_name = self.renderer.render_name(base, cu) if base else "?"
                virt = "virtual " if attr_int(ch, "DW_AT_virtuality") else ""
                acc = ACCESS.get(attr_int(ch, "DW_AT_accessibility"), "public")
                bases.append("%s%s %s" % (virt, acc, base_name))
        head = "%s%s %s" % (pad, kw, entry["name"])
        if bases:
            head += " : " + ", ".join(bases)
        lines.append("%s {%s" % (head, cmt))
        children = list(d.iter_children())
        # 嵌套类型
        for ch in children:
            if ch.tag == "DW_TAG_typedef":
                nm = attr_str(ch, "DW_AT_name")
                if nm:
                    self.emit_typedef(ch, cu, None, lines, indent + 1, nested_name=nm)
            elif ch.tag == "DW_TAG_enumeration_type":
                self.emit_enum(ch, cu, None, lines, indent + 1, nested=True)
            elif ch.tag in CLASS_KINDS and attr_str(ch, "DW_AT_name") and \
                    "DW_AT_declaration" not in ch.attributes:
                nm = attr_str(ch, "DW_AT_name")
                sz = attr_int(ch, "DW_AT_byte_size")
                if sz is not None:
                    lines.append("    " * (indent + 1) + "// sizeof = %d" % sz)
                lines.append("    " * (indent + 1) + "%s %s {" % (CLASS_KINDS[ch.tag], nm))
                self.emit_class_body(ch, cu, lines, indent + 1)
                lines.append("    " * (indent + 1) + "};")
        self.emit_class_body(d, cu, lines, indent)
        lines.append("%s};" % pad)

    def emit_class_body(self, d, cu, lines, indent):
        pad = "    " * indent
        cur_access = None
        for ch in d.iter_children():
            if ch.tag == "DW_TAG_member":
                acc = ACCESS.get(attr_int(ch, "DW_AT_accessibility"), "public")
                if acc != cur_access:
                    lines.append("%s%s:" % (pad, acc))
                    cur_access = acc
                self.emit_member(ch, cu, lines, indent)
            elif ch.tag == "DW_TAG_subprogram":
                acc = ACCESS.get(attr_int(ch, "DW_AT_accessibility"), "public")
                if acc != cur_access:
                    lines.append("%s%s:" % (pad, acc))
                    cur_access = acc
                self.emit_method_decl(ch, cu, lines, indent)
            elif ch.tag == "DW_TAG_variable":
                acc = ACCESS.get(attr_int(ch, "DW_AT_accessibility"), "public")
                if acc != cur_access:
                    lines.append("%s%s:" % (pad, acc))
                    cur_access = acc
                self.emit_static_member(ch, cu, lines, indent)

    def emit_member(self, ch, cu, lines, indent):
        pad = "    " * indent
        nm = attr_str(ch, "DW_AT_name")
        td = self.renderer.die_at(cu, attr_int(ch, "DW_AT_type"))
        bits = attr_int(ch, "DW_AT_bit_size")
        loc = attr_int(ch, "DW_AT_data_member_location")
        cmt = []
        if loc is not None:
            cmt.append("offset %d" % loc)
        if bits is not None:
            cmt.append("bitfield %d" % bits)
        if cmt:
            lines.append("%s// %s" % (pad, ", ".join(cmt)))
        if td is not None and td.tag in CLASS_KINDS and not attr_str(td, "DW_AT_name"):
            # 匿名 struct/union 成员：内联定义
            kw = CLASS_KINDS[td.tag]
            sz = attr_int(td, "DW_AT_byte_size")
            if sz is not None:
                lines.append("%s// anonymous %s, sizeof = %d" % (pad, kw, sz))
            if nm:
                lines.append("%s%s {" % (pad, kw))
                self.emit_class_body(td, cu, lines, indent + 1)
                lines.append("%s} %s;" % (pad, nm))
            else:
                # 未命名 union/struct 成员（GCC 匿名扩展）
                lines.append("%s%s {" % (pad, kw))
                self.emit_class_body(td, cu, lines, indent + 1)
                lines.append("%s};" % pad)
            return
        decl = self.renderer.render(td, cu, nm or "_unnamed_")
        tail = " : %d;" % bits if bits is not None else ";"
        lines.append("%s%s%s" % (pad, decl, tail))

    def emit_static_member(self, ch, cu, lines, indent):
        pad = "    " * indent
        nm = attr_str(ch, "DW_AT_name")
        td = self.renderer.die_at(cu, attr_int(ch, "DW_AT_type"))
        decl = self.renderer.render(td, cu, nm)
        lines.append("%sstatic %s;" % (pad, decl))

    def emit_method_decl(self, ch, cu, lines, indent):
        pad = "    " * indent
        nm = attr_str(ch, "DW_AT_name")
        if not nm:
            return
        cls_name = None
        # 找所在类名（通过声明 DIE 的父链）
        prec = self.cu_recs.get(cu_id(cu))
        if prec is not None:
            r0 = prec["by_off"].get(ch.offset)
            p = r0["parent"] if r0 else None
            while p is not None:
                if p["die"].tag in CLASS_KINDS:
                    cls_name = attr_str(p["die"], "DW_AT_name")
                    break
                p = p["parent"]
        is_ctor = cls_name is not None and nm == cls_name
        is_dtor = cls_name is not None and nm == "~" + cls_name
        ret = self.renderer.return_type(ch, cu) if not (is_ctor or is_dtor) else ""
        defn = self.decl_to_defn.get(ch.offset)
        cp = [p for p in ch.iter_children()
              if p.tag == "DW_TAG_formal_parameter" and not p.attributes.get("DW_AT_artificial")]
        params = self.renderer.param_list(ch, cu)
        if defn is not None:
            names = self.renderer.param_names(defn["die"], defn["cu"])
            parts = []
            for i, p in enumerate(cp):
                td = self.renderer.die_at(cu, attr_int(p, "DW_AT_type"))
                pname = names[i] if i < len(names) and names[i] else "arg%d" % i
                parts.append(self.renderer.render(td, cu, pname).strip() if td else pname)
            params = ", ".join(parts)
        virt = ""
        pure = ""
        v = attr_int(ch, "DW_AT_virtuality")
        if v == 1:
            virt = "virtual "
        elif v == 2:
            virt = "virtual "
            pure = " = 0"
        const = ""
        for p in ch.iter_children():
            if p.tag == "DW_TAG_formal_parameter" and p.attributes.get("DW_AT_artificial"):
                td = self.renderer.die_at(cu, attr_int(p, "DW_AT_type"))
                if td is not None and td.tag == "DW_TAG_pointer_type":
                    pt = self.renderer.die_at(cu, attr_int(td, "DW_AT_type"))
                    if pt is not None and pt.tag == "DW_TAG_const_type":
                        const = " const"
                break
        has_this = any(p.attributes.get("DW_AT_artificial") for p in ch.iter_children()
                       if p.tag == "DW_TAG_formal_parameter")
        static = "static " if (not has_this and ch.attributes.get("DW_AT_external")) else ""
        dl = attr_int(ch, "DW_AT_decl_line")
        cmt = " // line %d" % dl if dl is not None else ""
        ret_part = ret + " " if ret else ""
        lines.append("%s%s%s%s%s(%s)%s%s;%s" % (pad, static, virt, ret_part, nm, params, const, pure, cmt))

    def emit_typedef(self, d, cu, entry, lines, indent, nested_name=None):
        pad = "    " * indent
        nm = nested_name or (entry["name"] if entry else attr_str(d, "DW_AT_name"))
        if not nm:
            return
        td = self.renderer.die_at(cu, attr_int(d, "DW_AT_type"))
        if td is not None and td.tag in CLASS_KINDS and not attr_str(td, "DW_AT_name"):
            kw = CLASS_KINDS[td.tag]
            lines.append("%stypedef %s {" % (pad, kw))
            self.emit_class_body(td, cu, lines, indent + 1)
            lines.append("%s} %s;" % (pad, nm))
            return
        decl = self.renderer.render(td, cu, nm).strip()
        lines.append("%stypedef %s;" % (pad, decl))

    def emit_enum(self, d, cu, entry, lines, indent, nested=False):
        pad = "    " * indent
        nm = entry["name"] if entry else attr_str(d, "DW_AT_name")
        lines.append("%senum %s {" % (pad, nm) if nm else "%senum {" % pad)
        for ch in d.iter_children():
            if ch.tag == "DW_TAG_enumerator":
                en = attr_str(ch, "DW_AT_name")
                val = attr_int(ch, "DW_AT_const_value")
                if val is None:
                    cv = ch.attributes.get("DW_AT_const_value")
                    val = cv.value if cv is not None else None
                if val is not None:
                    lines.append("%s    %s = %d," % (pad, en, val))
                else:
                    lines.append("%s    %s," % (pad, en))
        lines.append("%s};" % pad)

    # -- 自由函数 / 全局变量声明 -------------------------------------------

    def emit_free_func_decl(self, r, qn, lines, indent):
        pad = "    " * indent
        d = r["die"]
        cu = r["cu"]
        nm = attr_str(d, "DW_AT_name")
        if not nm:
            return
        ret = self.renderer.return_type(d, cu)
        defn = None
        for rr in self.reg.defns.get(qn, []):
            defn = rr
            break
        cp = [p for p in d.iter_children()
              if p.tag == "DW_TAG_formal_parameter" and not p.attributes.get("DW_AT_artificial")]
        params = self.renderer.param_list(d, cu)
        if defn is not None:
            names = self.renderer.param_names(defn["die"], defn["cu"])
            parts = []
            for i, p in enumerate(cp):
                td = self.renderer.die_at(cu, attr_int(p, "DW_AT_type"))
                pname = names[i] if i < len(names) and names[i] else "arg%d" % i
                parts.append(self.renderer.render(td, cu, pname).strip() if td else pname)
            params = ", ".join(parts)
        extern = "extern " if d.attributes.get("DW_AT_external") else ""
        dl = attr_int(d, "DW_AT_decl_line")
        cmt = " // line %d" % dl if dl is not None else ""
        lines.append("%s%s%s %s(%s);%s" % (pad, extern, ret, nm, params, cmt))

    def emit_global_decl(self, r, qn, lines, indent):
        pad = "    " * indent
        d = r["die"]
        cu = r["cu"]
        nm = attr_str(d, "DW_AT_name")
        td = self.renderer.die_at(cu, attr_int(d, "DW_AT_type"))
        decl = self.renderer.render(td, cu, nm).strip()
        dl = attr_int(d, "DW_AT_decl_line")
        cmt = " // line %d" % dl if dl is not None else ""
        lines.append("%sextern %s;%s" % (pad, decl, cmt))

    # -- .cpp 输出 ---------------------------------------------------------

    def write_cpp(self, cu, fpath):
        lines = [
            "// Auto-generated stub from DWARF info",
            "// Original source: %s" % (cu["name"] or fpath),
            "// Compiler: %s" % cu["producer"],
            "// 函数体暂为空；仅保留签名、参数名与局部变量名。",
            "",
        ]
        inc_lines = []
        seen = set()
        res = cu["resolver"]
        if res.lp is not None:
            for i in range(1, len(res.files) + 1):
                p = res.mirror_path(i)
                if p and p != fpath and p not in seen:
                    seen.add(p)
                    inc_lines.append('#include "%s"' % p)
        for si in sorted(self.cu_system_includes(cu)):
            if si not in inc_lines:
                inc_lines.append("#include %s" % si)
        lines.extend(inc_lines)
        if inc_lines:
            lines.append("")
        scope_items = defaultdict(list)
        for r in cu["dies"]:
            d = r["die"]
            if d.tag == "DW_TAG_subprogram" and "DW_AT_low_pc" in d.attributes:
                if r["decl_file"] and r["decl_file"].endswith(HEADER_EXTS):
                    continue  # 头文件内联函数：头文件已保留声明
                if attr_int(d, "DW_AT_specification") is None and attr_str(d, "DW_AT_name") is None:
                    continue
                nm0 = attr_str(d, "DW_AT_name") or ""
                if nm0.startswith("__tcf_") or nm0.startswith("_GLOBAL__I_") or \
                        nm0 == "__static_initialization_and_destruction_0":
                    continue
                scope_items[clean_scope(r["scope"])].append(("func", r))
            elif d.tag == "DW_TAG_variable" and "DW_AT_location" in d.attributes:
                if r["parent"] is not None and r["parent"]["die"].tag not in (
                        "DW_TAG_compile_unit", "DW_TAG_namespace"):
                    continue
                scope_items[clean_scope(r["scope"])].append(("var", r))
        self.emit_cpp_scope(scope_items, lines, 0)
        self.ensure_dir(fpath)
        with open(self.out_path(fpath), "w", encoding="utf-8", newline="\n") as fo:
            fo.write("\n".join(lines) + "\n")

    def cu_system_includes(self, cu):
        out = set()
        res = cu["resolver"]
        if res.lp is None:
            return out
        for i in range(1, len(res.files) + 1):
            p = res.resolve(i)
            if p and is_system_path(p):
                si = system_include(p)
                if si:
                    out.add(si)
        return out

    def emit_cpp_scope(self, scope_items, lines, indent):
        children = defaultdict(dict)
        here = []
        for scope, its in scope_items.items():
            if len(scope) == 0:
                here.extend(its)
            else:
                children[scope[0]][scope[1:]] = its
        pad = "    " * indent
        if here:
            self.emit_cpp_items(here, lines, indent)
        for ns in sorted(children):
            lines.append("%snamespace %s {" % (pad, ns))
            self.emit_cpp_scope(children[ns], lines, indent + 1)
            lines.append("%s} // namespace %s" % (pad, ns))

    def emit_cpp_items(self, items, lines, indent):
        for it in items:
            if it[0] == "func":
                self.emit_cpp_func(it[1], lines, indent)
            elif it[0] == "var":
                self.emit_cpp_var(it[1], lines, indent)

    def emit_cpp_func(self, r, lines, indent):
        d = r["die"]
        cu = r["cu"]
        pad = "    " * indent
        spec = attr_int(d, "DW_AT_specification")
        decl_rec = None
        if spec is not None:
            rec = self.cu_recs.get(id(r["cu"]))
            if rec is not None:
                decl_rec = rec["by_off"].get(rec["cu"].cu_offset + spec)
        if decl_rec is not None and decl_rec["die"].tag == "DW_TAG_subprogram":
            decl_die = decl_rec["die"]
            decl_cu = r["cu"]
            nm = attr_str(decl_die, "DW_AT_name")
            ret = self.renderer.return_type(decl_die, decl_cu)
            cp = [p for p in decl_die.iter_children()
                  if p.tag == "DW_TAG_formal_parameter" and not p.attributes.get("DW_AT_artificial")]
            names = self.renderer.param_names(d, cu)
            parts = []
            for i, p in enumerate(cp):
                td = self.renderer.die_at(decl_cu, attr_int(p, "DW_AT_type"))
                pname = names[i] if i < len(names) and names[i] else "arg%d" % i
                parts.append(self.renderer.render(td, decl_cu, pname).strip() if td else pname)
            params = ", ".join(parts)
            # 限定名：声明所在类/命名空间
            scope = decl_rec["scope"]
            scope2 = [s for s in scope if s and s != "::"]
            fqn = ("::".join(scope2) + "::" + nm) if scope2 else nm
        else:
            nm = attr_str(d, "DW_AT_name")
            if not nm:
                return
            ret = self.renderer.return_type(d, cu)
            names = self.renderer.param_names(d, cu)
            parts = []
            for i, p in enumerate([p for p in d.iter_children()
                                   if p.tag == "DW_TAG_formal_parameter" and
                                   not p.attributes.get("DW_AT_artificial")]):
                td = self.renderer.die_at(cu, attr_int(p, "DW_AT_type"))
                pname = names[i] if i < len(names) and names[i] else "arg%d" % i
                parts.append(self.renderer.render(td, cu, pname).strip() if td else pname)
            params = ", ".join(parts)
            scope = r["scope"]
            scope2 = [s for s in scope if s and s != "::"]
            fqn = ("::".join(scope2) + "::" + nm) if scope2 else nm
        cmts = []
        mn = attr_str(d, "DW_AT_MIPS_linkage_name")
        if mn:
            cmts.append("mangled: %s" % mn)
        dl = attr_int(d, "DW_AT_decl_line")
        if dl:
            cmts.append("line %d" % dl)
        for c in cmts:
            lines.append("%s// %s" % (pad, c))
        lines.append("%s%s %s(%s) {" % (pad, ret, fqn, params))
        for lv in self.collect_locals(d, cu):
            lines.append("%s    // local: %s;" % (pad, lv))
        lines.append("%s}" % pad)
        lines.append("")

    def collect_locals(self, d, cu):
        out = []

        def rec(die):
            for ch in die.iter_children():
                if ch.tag == "DW_TAG_variable":
                    vn = attr_str(ch, "DW_AT_name")
                    if vn and not ch.attributes.get("DW_AT_artificial"):
                        td = self.renderer.die_at(cu, attr_int(ch, "DW_AT_type"))
                        out.append(self.renderer.render(td, cu, vn).strip() if td else vn)
                elif ch.tag in ("DW_TAG_lexical_block", "DW_TAG_inlined_subroutine"):
                    rec(ch)
                elif ch.tag == "DW_TAG_subprogram":
                    continue

        rec(d)
        return out

    def emit_cpp_var(self, r, lines, indent):
        d = r["die"]
        cu = r["cu"]
        pad = "    " * indent
        nm = attr_str(d, "DW_AT_name")
        if not nm:
            return
        td = self.renderer.die_at(cu, attr_int(d, "DW_AT_type"))
        decl = self.renderer.render(td, cu, nm).strip()
        lines.append("%s%s;" % (pad, decl))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("elf")
    ap.add_argument("out")
    args = ap.parse_args()
    cus, reg = analyze(args.elf)
    em = Emitter(reg, cus, args.out)
    counts = em.write_all()
    print("C++ CU:", len(cus))
    print("counts:", counts)
    print("named types:", len(reg.named_types))
    print("free funcs:", len(reg.free_funcs))
    print("definitions:", sum(len(v) for v in reg.defns.values()))
    print("spec-linked defns:", len(reg.spec_to_defn))
    print("globals:", len(reg.globals))


if __name__ == "__main__":
    main()
