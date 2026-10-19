#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
gen_headers.py -- 从 DWARF 重建 secsvr 三件套的全部头文件（类型信息 + 声明）。

背景：gen_dwarf_stubs.py 生成的头桩不完整：
  1) 部分 class/enum 定义 DIE 没有 DW_AT_decl_file，导致无法归属到头文件；
  2) tencrypt_new 独立工程路径（/data/yunfeiyang/.../tencrypt_new/src/../inc/x.h）
     未归一化到桩镜像的 inc/、include/、src/ 布局；
  3) 同一头文件同时以 src/... 与 output/.../release/include/... 两个路径被引用
     （zenlib/protocol 用 src 头，framework/服务用安装头），需要双路径输出。

本工具对每个二进制单独重建头文件（保留各自的镜像树），并做跨二进制合并：
同一 qname 的类型取成员最多的定义。

用法：
    python3 gen_headers.py <二进制> <输出根> [--only src output import inc include]

只重写有类型/声明内容的头文件；空桩保留不动。
"""

from __future__ import print_function

import argparse
import os
import re
import sys
from collections import OrderedDict, defaultdict

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import gen_dwarf_stubs as gds


# ---------------------------------------------------------------------------
# 路径归一化
# ---------------------------------------------------------------------------

def _norm_segments(path):
    """把 a/../b 归一化成 b（不解析符号链接）。"""
    parts = path.split("/")
    out = []
    for p in parts:
        if p == ".":
            continue
        if p == ".." and out and out[-1] not in ("..", ""):
            out.pop()
        else:
            out.append(p)
    return "/".join(out)


def normalize_path(path):
    """把 DWARF 原始路径归一到桩镜像布局。"""
    p = (path or "").replace("\\", "/")
    p = gds.strip_workspace(p)          # 去掉 /workspace/<tag>/ 或 /data/ 前缀
    p = p.replace("/./", "/")
    # tencrypt_new 独立工程：yunfeiyang/dev/branch/src/common/tencrypt_new/<...>
    m = re.search(r"(?:^|/)tencrypt_new/src/\.\./inc/(.+)$", p)
    if m:
        return "inc/" + m.group(1)
    m = re.search(r"(?:^|/)tencrypt_new/include/(.+)$", p)
    if m:
        return "include/" + m.group(1)
    m = re.search(r"(?:^|/)tencrypt_new/src/(.+)$", p)
    if m:
        return "src/" + m.group(1)
    return _norm_segments(p)


def twin_path2(path):
    m = re.match(r"^src/(.+?)/([^/]+)$", path)
    if m:
        return "output/%s/release/include/%s" % (m.group(1), m.group(2))
    m = re.match(r"^output/(.+?)/release/include/([^/]+)$", path)
    if m:
        return "src/%s/%s" % (m.group(1), m.group(2))
    return None


def is_system(p):
    return p is None or p.startswith(("/usr/", "/lib", "/lib64", "/usr/local", "/usr/src"))


# ---------------------------------------------------------------------------
# 带 decl_file 修正的注册
# ---------------------------------------------------------------------------

def best_decl_file(r):
    """DIE 自带 decl_file 优先；否则递归取第一个带 decl_file 的子 DIE。"""
    if r["decl_file"]:
        return r["decl_file"]

    def rec(node, depth=0):
        if depth > 24:
            return None
        for ch in node["children"]:
            if ch["decl_file"]:
                return ch["decl_file"]
            v = rec(ch, depth + 1)
            if v:
                return v
        return None

    return rec(r)


def register_with_fix(reg, cu_rec):
    """在已构建的 CU record 树上重新注册，decl_file 缺失时用子 DIE 回填。"""
    for r in cu_rec["dies"]:
        d = r["die"]
        if d.tag not in gds.CLASS_KINDS and d.tag not in (
                "DW_TAG_enumeration_type", "DW_TAG_typedef"):
            continue
        nm = gds.attr_str(d, "DW_AT_name")
        if not nm:
            continue
        df = best_decl_file(r)
        if not df or is_system(df):
            continue
        r2 = dict(r)
        r2["decl_file"] = normalize_path(df)
        if not r2["decl_file"].endswith(gds.HEADER_EXTS):
            continue
        if d.tag in gds.CLASS_KINDS:
            gds._register_class(reg, r2, nm)
        elif d.tag == "DW_TAG_enumeration_type":
            gds._register_named(reg, "enum", r2, nm)
        else:
            gds._register_named(reg, "typedef", r2, nm)

    for r in cu_rec["dies"]:
        d = r["die"]
        if d.tag == "DW_TAG_subprogram":
            gds._register_function(reg, r)
        elif d.tag == "DW_TAG_variable":
            gds._register_variable(reg, r)


def analyze_headers(elf_path):
    """返回 (cus, reg)。reg 使用修正后的路径注册。"""
    cus, base_reg = gds.analyze(elf_path)
    reg = gds.Registry()
    # 继承 DIE 引用解析表（TypeRenderer.die_at/rec_at 依赖 cu_dies）
    reg.cu_dies = base_reg.cu_dies
    for cu in cus:
        register_with_fix(reg, cu)
    return cus, reg


# ---------------------------------------------------------------------------
# 头文件输出（复用 Emitter 渲染，修正 header_items 的路径）
# ---------------------------------------------------------------------------

class HeaderEmitter(gds.Emitter):
    """只输出头文件；.cpp 桩不重写。"""

    def __init__(self, reg, cus, out_root):
        gds.Emitter.__init__(self, reg, cus, out_root)

    def emit_all_headers(self, only_prefixes=None):
        header_files = set()
        for e in self.reg.named_types.values():
            if e["file"] and e["file"].endswith(gds.HEADER_EXTS):
                header_files.add(e["file"])
        for lst in self.reg.free_funcs.values():
            for r in lst:
                if r["decl_file"] and r["decl_file"].endswith(gds.HEADER_EXTS):
                    header_files.add(r["decl_file"])
        for lst in self.reg.globals.values():
            for r in lst:
                if r["decl_file"] and r["decl_file"].endswith(gds.HEADER_EXTS):
                    header_files.add(r["decl_file"])
        inferred = self.infer_same_stem_decls()
        written = set()
        counts = {"headers": 0, "types": 0, "funcs": 0, "vars": 0}
        # 阶段 1：注册内容头
        for fpath in sorted(header_files):
            if only_prefixes and not fpath.startswith(tuple(only_prefixes)):
                continue
            its = self.header_items(fpath)
            if not its:
                continue
            self.write_header(fpath)
            counts["headers"] += 1
            counts["types"] += sum(1 for i in its if i[0] == "type")
            counts["funcs"] += sum(1 for i in its if i[0] == "func")
            counts["vars"] += sum(1 for i in its if i[0] == "var")
            written.add(fpath)
        # 阶段 2：仅有推断声明的头（既有空桩上补内容）
        for h in sorted(inferred):
            if h in written:
                continue
            if only_prefixes and not h.startswith(tuple(only_prefixes)):
                continue
            if not os.path.exists(self.out_path(h)):
                continue
            self.write_empty_header(h)
            written.add(h)
            counts["headers"] += 1
        # 阶段 3：追加推断声明
        for h in sorted(inferred):
            if h not in written:
                continue
            decls = sorted(set(inferred[h]))
            if decls:
                self.append_inferred(h, decls)
                counts["funcs"] += len(decls)
        # 阶段 4：双路径镜像（含推断内容）
        for fpath in sorted(written):
            # 双路径镜像：src ↔ output
            tw = twin_path2(fpath)
            if tw:
                # 镜像内容与主文件一致（仅路径注释/宏守卫不同）
                content = open(self.out_path(fpath), "r", encoding="utf-8").read()
                content = content.replace(fpath, tw)
                content = content.replace(gds.header_guard(fpath), gds.header_guard(tw))
                self.ensure_dir(tw)
                with open(self.out_path(tw), "w", encoding="utf-8", newline="\n") as fo:
                    fo.write(content)
                counts["headers"] += 1
        return counts, written

    def infer_same_stem_decls(self):
        """把 .cpp 里的自由函数定义推断成同名头文件的声明（GCC 4.1 DWARF
        通常不为头文件中的自由函数单独发声明 DIE）。返回 {header: [lines]}。"""
        out = defaultdict(list)
        for cu in self.cus:
            cpp = cu["name"] or ""
            cpp = gds.strip_workspace(cpp)
            if not cpp.endswith(gds.CPP_EXTS):
                continue
            stem = os.path.splitext(os.path.basename(cpp))[0]
            # 该 CU include 闭包里的同名头
            headers = set()
            res = cu["resolver"]
            if res.lp is not None:
                for i in range(1, len(res.files) + 1):
                    p = res.mirror_path(i)
                    if p and p.endswith(gds.HEADER_EXTS) and \
                            os.path.splitext(os.path.basename(p))[0] == stem:
                        headers.add(normalize_path(p))
            if not headers:
                continue
            for r in cu["dies"]:
                d = r["die"]
                if d.tag != "DW_TAG_subprogram" or "DW_AT_low_pc" not in d.attributes:
                    continue
                if r["decl_file"] and not r["decl_file"].endswith(gds.CPP_EXTS):
                    continue
                nm = gds.attr_str(d, "DW_AT_name")
                if not nm or nm.startswith(("__tcf_", "_GLOBAL__I_", "__static_initialization")):
                    continue
                ret = self.renderer.return_type(d, cu)
                names = self.renderer.param_names(d, cu)
                parts = []
                for i, p in enumerate([p for p in d.iter_children()
                                       if p.tag == "DW_TAG_formal_parameter" and
                                       not p.attributes.get("DW_AT_artificial")]):
                    td = self.renderer.die_at(cu, gds.attr_int(p, "DW_AT_type"))
                    pname = names[i] if i < len(names) and names[i] else "arg%d" % i
                    parts.append(self.renderer.render(td, cu, pname).strip() if td else pname)
                dl = gds.attr_int(d, "DW_AT_decl_line")
                cmt = " // line %d (inferred)" % dl if dl else " // inferred"
                decl = "extern %s %s(%s);%s" % (ret, nm, ", ".join(parts), cmt)
                for h in headers:
                    out[h].append(decl)
        return out

    def append_inferred(self, fpath, decl_lines):
        guard = gds.header_guard(fpath)
        p = self.out_path(fpath)
        lines = open(p, "r", encoding="utf-8").read().rstrip("\n").split("\n")
        # 插到 #endif 之前
        out = []
        for ln in lines:
            if ln.strip() == "#endif // %s" % guard:
                out.append("")
                out.extend(decl_lines)
                out.append("")
                out.append(ln)
            else:
                out.append(ln)
        with open(p, "w", encoding="utf-8", newline="\n") as fo:
            fo.write("\n".join(out) + "\n")


def merge_registries(regs):
    """合并多个 Registry（跨二进制），同类 qname 取成员最多的定义。"""
    merged = gds.Registry()
    for reg in regs:
        for key, entry in reg.named_types.items():
            m = merged.named_types.get(key)
            if m is None:
                merged.named_types[key] = dict(entry)
                continue
            if entry["declaration"] and not m["declaration"]:
                continue
            if m["declaration"] and not entry["declaration"]:
                merged.named_types[key] = dict(entry)
                continue
            cn = sum(1 for _ in m["die"].iter_children())
            nn = sum(1 for _ in entry["die"].iter_children())
            if nn > cn:
                merged.named_types[key] = dict(entry)
        for qn, lst in reg.free_funcs.items():
            merged.free_funcs.setdefault(qn, [])
            have = {id(r["die"]) for r in merged.free_funcs[qn]}
            for r in lst:
                if id(r["die"]) not in have:
                    merged.free_funcs[qn].append(r)
        for qn, lst in reg.globals.items():
            merged.globals.setdefault(qn, [])
            have = {id(r["die"]) for r in merged.globals[qn]}
            for r in lst:
                if id(r["die"]) not in have:
                    merged.globals[qn].append(r)
        for qn, lst in reg.defns.items():
            merged.defns.setdefault(qn, [])
            for r in lst:
                merged.defns[qn].append(r)
        merged.spec_to_defn.update(reg.spec_to_defn)
    return merged


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("elfs", nargs="+")
    ap.add_argument("--out", default=None)
    ap.add_argument("--only", nargs="*",
                    help="只输出这些前缀下的头（如 src output import inc include）")
    args = ap.parse_args()

    regs = []
    cus_all = []
    for elf in args.elfs:
        cus, reg = analyze_headers(elf)
        cus_all.extend(cus)
        regs.append(reg)
        print("[%s] C++ CU: %d, named types: %d, free funcs: %d" % (
            os.path.basename(elf), len(cus), len(reg.named_types), len(reg.free_funcs)))

    if len(regs) > 1:
        merged = merge_registries(regs)
        print("merged named types:", len(merged.named_types),
              "free funcs:", len(merged.free_funcs))
        regs = [merged]

    for reg in regs:
        out = args.out
        em = HeaderEmitter(reg, cus_all, out)
        counts, written = em.emit_all_headers(args.only)
        print("headers written:", counts)


if __name__ == "__main__":
    main()
