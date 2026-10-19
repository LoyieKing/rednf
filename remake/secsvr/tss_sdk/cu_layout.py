#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Dump the nested DWARF type layout (structs/unions/classes/enums/typedefs)
of one CU as an indented tree, with member offsets and byte sizes.

Usage: cu_layout.py <elf> <cu-name-substring> [name-filter-substring]

Anonymous aggregates (GCC's "._NN" names) are printed as-is so the caller can
see the real nesting; the auto-generated headers flatten these away.
Aggregate DIEs whose DW_AT_name lives on the declaration DIE are resolved
through DW_AT_specification.
"""
from __future__ import print_function

import sys

from elftools.elf.elffile import ELFFile

AGG = ("DW_TAG_structure_type", "DW_TAG_class_type", "DW_TAG_union_type")
TAGSHORT = {
    "DW_TAG_structure_type": "struct",
    "DW_TAG_class_type": "class",
    "DW_TAG_union_type": "union",
}


def s(die, name):
    a = die.attributes.get(name)
    if a is None:
        return None
    v = a.value
    if isinstance(v, (bytes, bytearray)):
        return v.decode("utf-8", "replace")
    return str(v)


def i(die, name):
    a = die.attributes.get(name)
    return None if a is None else a.value


def resolve(die, attr, dwarf):
    a = die.attributes.get(attr)
    if a is None:
        return None
    try:
        if a.form == "DW_FORM_ref_addr":
            return dwarf.get_DIE_from_refaddr(a.value)
        return dwarf.get_DIE_from_refaddr(die.cu.cu_offset + a.value)
    except Exception:
        return None


def die_name(die, dwarf):
    """DW_AT_name, falling back to the DW_AT_specification declaration."""
    nm = s(die, "DW_AT_name")
    if nm:
        return nm
    ref = resolve(die, "DW_AT_specification", dwarf)
    if ref is not None:
        return s(ref, "DW_AT_name")
    return None


def off_of(die):
    a = die.attributes.get("DW_AT_data_member_location")
    if a is None:
        return None
    v = a.value
    if isinstance(v, (bytes, bytearray, list)):
        if len(v) >= 2 and v[0] == 35:
            off = 0
            shift = 0
            for b in v[1:]:
                off |= (b & 0x7f) << shift
                if not (b & 0x80):
                    return off
                shift += 7
        return None
    return v


def walk(die):
    for c in die.iter_children():
        yield c
        for gc in walk(c):
            yield gc


def dump_type(die, dwarf, indent, depth=0):
    if die is None or depth > 6:
        return
    tag = die.tag
    if tag == "DW_TAG_typedef":
        print("%s%s = typedef" % (indent, s(die, "DW_AT_name")))
        dump_type(resolve(die, "DW_AT_type", dwarf), dwarf, indent + "  ",
                  depth + 1)
        return
    if tag == "DW_TAG_pointer_type":
        t = resolve(die, "DW_AT_type", dwarf)
        print("%spointer -> %s" % (indent, die_name(t, dwarf) if t else "void"))
        return
    if tag == "DW_TAG_const_type":
        dump_type(resolve(die, "DW_AT_type", dwarf), dwarf, indent + "const ",
                  depth + 1)
        return
    if tag == "DW_TAG_array_type":
        dims = []
        for c in die.iter_children():
            if c.tag == "DW_TAG_subrange_type":
                cnt = i(c, "DW_AT_count")
                up = i(c, "DW_AT_upper_bound")
                dims.append(cnt if cnt is not None else
                            (up + 1 if up is not None else None))
        print("%sarray[%s] of" % (indent, ",".join(str(d) for d in dims)))
        dump_type(resolve(die, "DW_AT_type", dwarf), dwarf, indent + "  ",
                  depth + 1)
        return
    if tag == "DW_TAG_enumeration_type":
        print("%senum %s size=%s" % (indent, die_name(die, dwarf) or "?",
                                     i(die, "DW_AT_byte_size")))
        for c in die.iter_children():
            if c.tag == "DW_TAG_enumerator":
                print("%s  %s = %s" % (indent, s(c, "DW_AT_name"),
                                       i(c, "DW_AT_const_value")))
        return
    if tag in AGG:
        sz = i(die, "DW_AT_byte_size")
        print("%s%s %s size=%s%s" % (indent, TAGSHORT[tag],
                                     die_name(die, dwarf) or "<anon>", sz,
                                     "" if sz else "   (decl)"))
        if not sz:
            return
        for c in die.iter_children():
            if c.tag != "DW_TAG_member":
                continue
            if c.attributes.get("DW_AT_external"):
                continue
            t = resolve(c, "DW_AT_type", dwarf)
            print("%s  +%-6s %s :" % (indent, off_of(c),
                                      s(c, "DW_AT_name") or "<anon member>"))
            dump_type(t, dwarf, indent + "        ", depth + 1)
        return
    print("%s%s" % (indent, s(die, "DW_AT_name") or (tag + "@%x" % die.offset)))


def main():
    elf_path, target = sys.argv[1], sys.argv[2]
    filt = sys.argv[3] if len(sys.argv) > 3 else ""
    with open(elf_path, "rb") as f:
        dwarf = ELFFile(f).get_dwarf_info()
        for cu in dwarf.iter_CUs():
            top = cu.get_top_DIE()
            name = s(top, "DW_AT_name") or ""
            if target not in name:
                continue
            print("# CU %s (dwarf v%d)" % (name, cu["version"]))
            for die in walk(top):
                if die.tag not in AGG or not i(die, "DW_AT_byte_size"):
                    continue
                nm = die_name(die, dwarf) or ""
                if filt and filt not in nm:
                    continue
                print("\n## %s" % (nm or "<anon@%x>" % die.offset))
                dump_type(die, dwarf, "  ")
            return
    print("no CU matching %r" % target)


if __name__ == "__main__":
    main()
