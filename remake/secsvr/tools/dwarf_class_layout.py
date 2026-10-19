#!/usr/bin/env python3
"""从任意二进制的 DWARF 提取类/结构布局（成员名/偏移/类型/继承/方法），
供 commlib/framework 头文件重建时核对偏移与匿名联合。

用法: dwarf_class_layout.py <binary> <类名子串>...
"""
import sys
from elftools.elf.elffile import ELFFile


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
    return None if a is None else a.value


def resolve_ref(die, name, dwarf):
    a = die.attributes.get(name)
    if a is None:
        return None
    try:
        if a.form == "DW_FORM_ref_addr":
            return dwarf.get_DIE_from_refaddr(a.value)
        return dwarf.get_DIE_from_refaddr(die.cu.cu_offset + a.value)
    except Exception:
        return None


def type_name(die, dwarf, depth=0):
    if die is None or depth > 8:
        return "?"
    tag = die.tag
    if tag in ("DW_TAG_structure_type", "DW_TAG_class_type", "DW_TAG_union_type"):
        n = _str(die, "DW_AT_name")
        if n:
            return n
        ref = resolve_ref(die, "DW_AT_specification", dwarf)
        if ref is not None:
            n = _str(ref, "DW_AT_name")
            if n:
                return n
        return f"<anon@{die.offset:x}>"
    if tag == "DW_TAG_typedef":
        n = _str(die, "DW_AT_name")
        t = resolve_ref(die, "DW_AT_type", dwarf)
        inner = type_name(t, dwarf, depth + 1) if t is not None else "?"
        return n if n else inner
    if tag == "DW_TAG_pointer_type":
        t = resolve_ref(die, "DW_AT_type", dwarf)
        inner = "void" if t is None else type_name(t, dwarf, depth + 1)
        return f"{inner}*"
    if tag == "DW_TAG_reference_type":
        t = resolve_ref(die, "DW_AT_type", dwarf)
        inner = "?" if t is None else type_name(t, dwarf, depth + 1)
        return f"{inner}&"
    if tag == "DW_TAG_const_type":
        t = resolve_ref(die, "DW_AT_type", dwarf)
        inner = "?" if t is None else type_name(t, dwarf, depth + 1)
        return f"const {inner}"
    if tag == "DW_TAG_volatile_type":
        t = resolve_ref(die, "DW_AT_type", dwarf)
        inner = "?" if t is None else type_name(t, dwarf, depth + 1)
        return f"volatile {inner}"
    if tag == "DW_TAG_base_type":
        n = _str(die, "DW_AT_name")
        sz = _int(die, "DW_AT_byte_size")
        return n or f"base[{sz}]"
    if tag == "DW_TAG_array_type":
        t = resolve_ref(die, "DW_AT_type", dwarf)
        inner = "?" if t is None else type_name(t, dwarf, depth + 1)
        return f"{inner}[]"
    if tag == "DW_TAG_enumeration_type":
        return _str(die, "DW_AT_name") or "<enum>"
    if tag == "DW_TAG_subroutine_type":
        return "fn"
    return tag.replace("DW_TAG_", "")


def member_offset(die):
    a = die.attributes.get("DW_AT_data_member_location")
    if a is None:
        return None
    v = a.value
    if isinstance(v, int):
        return v
    # DWARF2/3 可能用位置表达式（DW_OP_plus_uconst <const>）
    if isinstance(v, (bytes, list)) and len(v) >= 2 and v[0] == 0x23:
        return int.from_bytes(bytes(v[1:]), "little")
    return None


def dump_class(die, dwarf, indent=0, seen=None):
    if seen is None:
        seen = set()
    pad = "  " * indent
    tag = die.tag
    if tag in ("DW_TAG_structure_type", "DW_TAG_class_type", "DW_TAG_union_type"):
        name = _str(die, "DW_AT_name") or f"<anon@{die.offset:x}>"
        size = _int(die, "DW_AT_byte_size")
        decl = _int(die, "DW_AT_decl_line")
        print(f"{pad}{tag.split('_')[-1]} {name} size={size} decl={decl}")
        if name in seen:
            return
        seen.add(name)
        for c in die.iter_children():
            if c.tag == "DW_TAG_inheritance":
                off = member_offset(c)
                t = resolve_ref(c, "DW_AT_type", dwarf)
                print(f"{pad}  inherit off={off} -> {type_name(t, dwarf)}")
            elif c.tag == "DW_TAG_member":
                mname = _str(c, "DW_AT_name")
                off = member_offset(c)
                t = resolve_ref(c, "DW_AT_type", dwarf)
                print(f"{pad}  {mname} @{off} : {type_name(t, dwarf)}")
                if t is not None and t.tag in ("DW_TAG_structure_type", "DW_TAG_class_type",
                                               "DW_TAG_union_type"):
                    inner_name = _str(t, "DW_AT_name")
                    if not inner_name:
                        dump_class(t, dwarf, indent + 2, seen)
            elif c.tag in ("DW_TAG_subprogram", "DW_TAG_variable"):
                mname = _str(c, "DW_AT_name")
                lnk = _int(c, "DW_AT_external")
                virt = _int(c, "DW_AT_virtuality")
                print(f"{pad}  [{'V' if virt else ' '}] {mname} ext={lnk} decl={_int(c, 'DW_AT_decl_line')}")
        print()
    elif tag == "DW_TAG_namespace":
        print(f"{pad}namespace {_str(die, 'DW_AT_name') or ''}")
        for c in die.iter_children():
            dump_class(c, dwarf, indent + 1, seen)


def main():
    binary = sys.argv[1]
    filters = sys.argv[2:]
    with open(binary, "rb") as f:
        elf = ELFFile(f)
        dwarf = elf.get_dwarf_info()
        for cu in dwarf.iter_CUs():
            top = cu.get_top_DIE()
            for die in top.iter_children():
                if die.tag in ("DW_TAG_structure_type", "DW_TAG_class_type", "DW_TAG_union_type",
                               "DW_TAG_namespace"):
                    name = _str(die, "DW_AT_name") or ""
                    if any(flt in name for flt in filters):
                        dump_class(die, dwarf)


if __name__ == "__main__":
    main()
