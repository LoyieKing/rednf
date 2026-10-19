#!/usr/bin/env python3
"""从 gunnersvr 的 DWARF 提取 protocol/common 消息类布局与函数签名。

输出每个类的：成员（名/类型/偏移/行）、方法（名/助记名/参数类型/const）、
以及成员类型解析（递归展开 typedef/struct）。
用法: dwarf_proto_extract.py <binary> <src_basename>...
"""
import sys
from elftools.elf.elffile import ELFFile
from elftools.dwarf.descriptions import describe_form_class


TYPE_NAMES = {}


def _attrs(die):
    return {a.name: a for a in die.attributes.values()}


def _str(die, name):
    a = die.attributes.get(name)
    if a is None:
        return None
    try:
        return a.value.decode("utf-8", "replace")
    except Exception:
        return None


def _int(die, name):
    a = die.attributes.get(name)
    if a is None:
        return None
    try:
        return a.value
    except Exception:
        return None


def _ref(die, name):
    a = die.attributes.get(name)
    if a is None:
        return None
    return a.value


def resolve_ref(die, name, dwarf):
    """解析 DW_AT_* 引用到 DIE（处理 ref4 的 CU 相对偏移）。"""
    a = die.attributes.get(name)
    if a is None:
        return None
    form = a.form
    try:
        if form == "DW_FORM_ref_addr":
            return dwarf.get_DIE_from_refaddr(a.value)
        # ref1/ref2/ref4/ref8/ref_udata: CU 相对
        cu_base = die.cu.cu_offset
        return dwarf.get_DIE_from_refaddr(cu_base + a.value)
    except Exception:
        return None


def die_name(die, dwarf):
    """递归解析类型 DIE 到可读名称。"""
    if die is None:
        return "?"
    tag = die.tag
    try:
        if tag in ("DW_TAG_structure_type", "DW_TAG_class_type", "DW_TAG_union_type"):
            n = _str(die, "DW_AT_name")
            if n:
                return n
            ref = resolve_ref(die, "DW_AT_specification", dwarf)
            if ref is not None:
                n = _str(ref, "DW_AT_name")
                if n:
                    return n
            off = die.offset
            return f"<anon@{off:x}>"
        if tag == "DW_TAG_typedef":
            n = _str(die, "DW_AT_name")
            t = resolve_ref(die, "DW_AT_type", dwarf)
            inner = ""
            if t is not None:
                try:
                    inner = die_name(t, dwarf)
                except Exception:
                    inner = "?"
            return f"{n}" if n else inner
        if tag == "DW_TAG_pointer_type":
            t = resolve_ref(die, "DW_AT_type", dwarf)
            inner = "void"
            if t is not None:
                try:
                    inner = die_name(t, dwarf)
                except Exception:
                    inner = "?"
            return f"{inner}*"
        if tag == "DW_TAG_reference_type":
            t = resolve_ref(die, "DW_AT_type", dwarf)
            inner = "?"
            if t is not None:
                try:
                    inner = die_name(t, dwarf)
                except Exception:
                    inner = "?"
            return f"{inner}&"
        if tag == "DW_TAG_const_type":
            t = resolve_ref(die, "DW_AT_type", dwarf)
            inner = "?"
            if t is not None:
                try:
                    inner = die_name(t, dwarf)
                except Exception:
                    inner = "?"
            return f"const {inner}"
        if tag == "DW_TAG_volatile_type":
            t = resolve_ref(die, "DW_AT_type", dwarf)
            inner = "?"
            if t is not None:
                try:
                    inner = die_name(t, dwarf)
                except Exception:
                    inner = "?"
            return f"volatile {inner}"
        if tag == "DW_TAG_base_type":
            n = _str(die, "DW_AT_name")
            sz = _int(die, "DW_AT_byte_size")
            return f"{n}" + (f"[{sz}]" if n is None else "")
        if tag == "DW_TAG_array_type":
            t = resolve_ref(die, "DW_AT_type", dwarf)
            inner = "?"
            if t is not None:
                try:
                    inner = die_name(t, dwarf)
                except Exception:
                    inner = "?"
            return f"{inner}[]"
        if tag == "DW_TAG_subroutine_type":
            return "fn"
        if tag == "DW_TAG_enumeration_type":
            n = _str(die, "DW_AT_name")
            return n or "<enum>"
        if tag == "DW_TAG_namespace":
            return _str(die, "DW_AT_name") or "<ns>"
        if tag == "DW_TAG_imported_declaration":
            return "imported"
        if tag == "DW_TAG_subprogram":
            return _str(die, "DW_AT_name") or "sub"
        if tag == "DW_TAG_member":
            return _str(die, "DW_AT_name") or "member"
        return tag.replace("DW_TAG_", "")
    except Exception as e:
        return f"<err:{e}>"


def type_qname(die, dwarf):
    """带限定名的类型名（用于 class 定义）。"""
    return die_name(die, dwarf)


def cu_file_map(dwarf):
    """{cu_offset: {file_idx: path}}"""
    out = {}
    for cu in dwarf.iter_CUs():
        top = cu.get_top_DIE()
        lf = top.attributes.get("DW_AT_stmt_list")
        if lf is None:
            out[cu.cu_offset] = {}
            continue
        try:
            lp = dwarf.line_program_for_CU(cu)
            fe = lp.header.get("file_entry") or []
            out[cu.cu_offset] = {i: e.get("name", b"") for i, e in enumerate(fe)}
        except Exception:
            out[cu.cu_offset] = {}
    return out


def walk_scope(die, ns_stack, dwarf, target, seen_classes):
    """递归遍历，记录 namespace 栈；输出 sec_proto 类。"""
    tag = die.tag
    if tag == "DW_TAG_namespace":
        n = _str(die, "DW_AT_name") or ""
        ns_stack = ns_stack + [n]
    elif tag in ("DW_TAG_structure_type", "DW_TAG_class_type"):
        name = _str(die, "DW_AT_name")
        if name and ns_stack == ["sec_proto"]:
            byte_size = _int(die, "DW_AT_byte_size")
            decl = _int(die, "DW_AT_decl_line")
            has_members = any(c.tag == "DW_TAG_member" for c in die.iter_children())
            if has_members or byte_size is not None:
                if name in seen_classes:
                    return
                seen_classes.add(name)
                print(f"\nclass {name} size={byte_size} decl={decl}")
                for c in die.iter_children():
                    if c.tag == "DW_TAG_member":
                        mname = _str(c, "DW_AT_name")
                        off = _int(c, "DW_AT_data_member_location")
                        mline = _int(c, "DW_AT_decl_line")
                        tdie = resolve_ref(c, "DW_AT_type", dwarf)
                        tname = "?"
                        if tdie is not None:
                            try:
                                tname = die_name(tdie, dwarf)
                            except Exception:
                                tname = "?"
                        print(f"  member {mname} @{off} line{mline} : {tname}")
                    elif c.tag == "DW_TAG_subprogram":
                        fname = _str(c, "DW_AT_name")
                        mg = _str(c, "DW_AT_MIPS_linkage_name") or _str(c, "DW_AT_linkage_name")
                        fline = _int(c, "DW_AT_decl_line")
                        params = []
                        for p in c.iter_children():
                            if p.tag == "DW_TAG_formal_parameter":
                                tdie = resolve_ref(p, "DW_AT_type", dwarf)
                                pn = "?"
                                if tdie is not None:
                                    try:
                                        pn = die_name(tdie, dwarf)
                                    except Exception:
                                        pn = "?"
                                params.append(pn)
                        rdie = resolve_ref(c, "DW_AT_type", dwarf)
                        rn = "?"
                        if rdie is not None:
                            try:
                                rn = die_name(rdie, dwarf)
                            except Exception:
                                rn = "?"
                        print(f"  fn {fname} line{fline} ret={rn} mg={mg} params={params}")
    for c in die.iter_children():
        walk_scope(c, ns_stack, dwarf, target, seen_classes)


def main():
    binary = sys.argv[1]
    targets = sys.argv[2:]
    # 第一遍：收集 sec_proto 命名空间里的类声明名
    sec_proto_names = set()
    # 第二遍数据：{name: (cu_file, die)}
    definitions = {}
    with open(binary, "rb") as f:
        elf = ELFFile(f)
        dwarf = elf.get_dwarf_info()
        cfmap = cu_file_map(dwarf)
        for cu in dwarf.iter_CUs():
            top = cu.get_top_DIE()
            fmap = cfmap.get(cu.cu_offset, {})
            cu_file = top.attributes.get("DW_AT_name")
            cu_file = cu_file.value.decode("utf-8", "replace") if cu_file else ""
            if not cu_file:
                continue
            if not any(t in cu_file for t in targets):
                continue
            if not cu_file.endswith(".cpp"):
                continue
            # 收集 sec_proto 命名空间内的类声明
            stack = [(top, [])]
            while stack:
                die, ns = stack.pop()
                if die.tag == "DW_TAG_namespace":
                    nm = _str(die, "DW_AT_name") or ""
                    ns = ns + [nm]
                elif die.tag in ("DW_TAG_structure_type", "DW_TAG_class_type"):
                    if ns == ["sec_proto"]:
                        nm = _str(die, "DW_AT_name")
                        if nm:
                            sec_proto_names.add(nm)
                for c in die.iter_children():
                    stack.append((c, ns))
        # 第二遍：找定义（带 spec 或带成员）
        for cu in dwarf.iter_CUs():
            top = cu.get_top_DIE()
            cu_file = top.attributes.get("DW_AT_name")
            cu_file = cu_file.value.decode("utf-8", "replace") if cu_file else ""
            if not cu_file.endswith(".cpp") or not any(t in cu_file for t in targets):
                continue
            stack = [top]
            while stack:
                die = stack.pop()
                if die.tag in ("DW_TAG_structure_type", "DW_TAG_class_type"):
                    bs = _int(die, "DW_AT_byte_size")
                    has_members = any(c.tag == "DW_TAG_member" for c in die.iter_children())
                    name = None
                    spec = die.attributes.get("DW_AT_specification")
                    if spec is not None:
                        try:
                            sd = dwarf.get_DIE_from_refaddr(die.cu.cu_offset + spec.value)
                            name = _str(sd, "DW_AT_name")
                        except Exception:
                            name = None
                    else:
                        name = _str(die, "DW_AT_name")
                    if name in sec_proto_names and (has_members or bs is not None):
                        if name not in definitions:
                            definitions[name] = (cu_file, die)
                for c in die.iter_children():
                    stack.append(c)
        # 输出
        for name in sorted(definitions):
            cu_file, die = definitions[name]
            bs = _int(die, "DW_AT_byte_size")
            print(f"\n==== {name} size={bs} defined_in={cu_file} ====")
            for c in die.iter_children():
                if c.tag == "DW_TAG_member":
                    mname = _str(c, "DW_AT_name")
                    off = _int(c, "DW_AT_data_member_location")
                    mline = _int(c, "DW_AT_decl_line")
                    tdie = resolve_ref(c, "DW_AT_type", dwarf)
                    tname = "?"
                    if tdie is not None:
                        try:
                            tname = die_name(tdie, dwarf)
                        except Exception:
                            tname = "?"
                    print(f"  member {mname} @{off} line{mline} : {tname}")
                elif c.tag == "DW_TAG_subprogram":
                    fname = _str(c, "DW_AT_name")
                    mg = _str(c, "DW_AT_MIPS_linkage_name") or _str(c, "DW_AT_linkage_name")
                    fline = _int(c, "DW_AT_decl_line")
                    params = []
                    for p in c.iter_children():
                        if p.tag == "DW_TAG_formal_parameter":
                            tdie = resolve_ref(p, "DW_AT_type", dwarf)
                            pn = "?"
                            if tdie is not None:
                                try:
                                    pn = die_name(tdie, dwarf)
                                except Exception:
                                    pn = "?"
                            params.append(pn)
                    rdie = resolve_ref(c, "DW_AT_type", dwarf)
                    rn = "?"
                    if rdie is not None:
                        try:
                            rn = die_name(rdie, dwarf)
                        except Exception:
                            rn = "?"
                    print(f"  fn {fname} line{fline} ret={rn} mg={mg} params={params}")


if __name__ == "__main__":
    main()
