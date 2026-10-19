#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Dump, for one CU, every sec_proto class definition DIE: members (offset,
type, bit size) and subprogram declarations (return type, params, const).

Usage: cudecl.py <elf> <cu-name-substring> [class-substr]
Read-only helper for the captcha/safemode reconstruction work.
"""
from __future__ import print_function

import sys

from elftools.elf.elffile import ELFFile

AGG = ("DW_TAG_structure_type", "DW_TAG_class_type", "DW_TAG_union_type")


def s(die, name):
    a = die.attributes.get(name)
    if a is None:
        return None
    v = a.value
    if isinstance(v, (bytes, bytearray)):
        return v.decode("utf-8", "replace")
    return str(v)


def walk(die):
    yield die
    for c in die.iter_children():
        for g in walk(c):
            yield g


def ref(die, attr, ctx):
    dwarf, cu_offset = ctx
    a = die.attributes.get(attr)
    if a is None:
        return None
    try:
        if a.form == "DW_FORM_ref_addr":
            return dwarf.get_DIE_from_refaddr(a.value)
        return dwarf.get_DIE_from_refaddr(cu_offset + a.value)
    except Exception:
        return None


def tname(die, ctx, depth=0):
    if die is None or depth > 8:
        return "?"
    tag = die.tag
    if tag == "DW_TAG_base_type":
        return s(die, "DW_AT_name") or "?"
    if tag == "DW_TAG_typedef":
        return s(die, "DW_AT_name") or "?"
    if tag == "DW_TAG_pointer_type":
        t = ref(die, "DW_AT_type", ctx)
        return (tname(t, ctx, depth + 1) if t is not None else "void") + "*"
    if tag == "DW_TAG_const_type":
        return "const " + tname(ref(die, "DW_AT_type", ctx), ctx, depth + 1)
    if tag == "DW_TAG_reference_type":
        return tname(ref(die, "DW_AT_type", ctx), ctx, depth + 1) + "&"
    if tag == "DW_TAG_array_type":
        child = None
        for c in die.iter_children():
            child = c
        n = None
        if child is not None:
            n = child.attributes.get("DW_AT_upper_bound")
        cnt = (n.value + 1) if n is not None else "?"
        return "%s[%s]" % (tname(ref(die, "DW_AT_type", ctx), ctx, depth + 1), cnt)
    if tag in AGG:
        nm = s(die, "DW_AT_name")
        if not nm:
            sp = ref(die, "DW_AT_specification", ctx)
            if sp is not None:
                nm = s(sp, "DW_AT_name")
        if not nm:
            return "<anon>"
        # GCC 4.1 uses "._NN" names for anonymous aggregates.
        return nm
    return tag


def decl_name(die, ctx):
    nm = s(die, "DW_AT_name")
    if nm:
        return nm
    sp = ref(die, "DW_AT_specification", ctx)
    return s(sp, "DW_AT_name") if sp is not None else "?"


def main():
    elf_path, target = sys.argv[1], sys.argv[2]
    flt = sys.argv[3] if len(sys.argv) > 3 else ""
    with open(elf_path, "rb") as f:
        dwarf = ELFFile(f).get_dwarf_info()
        for cu in dwarf.iter_CUs():
            top = cu.get_top_DIE()
            name = s(top, "DW_AT_name") or ""
            if target not in name:
                continue
            ctx = (dwarf, cu.cu_offset)
            print("# CU %s" % name)
            for die in walk(top):
                if die.tag not in AGG:
                    continue
                nm = s(die, "DW_AT_name") or ""
                if not nm or nm.startswith(".") or flt not in nm:
                    continue
                if "sec_proto" not in (s(die, "DW_AT_name") or "") and \
                        flt == "" and not _in_ns(die, ctx, "sec_proto"):
                    continue
                sz = die.attributes.get("DW_AT_byte_size")
                print("class %s size=%s" % (nm, sz.value if sz is not None else "?"))
                for c in die.iter_children():
                    if c.tag in AGG and c.attributes.get("DW_AT_name") is None:
                        print("  <anon %s size=%s>" % (c.tag, _bs(c)))
                        for m in c.iter_children():
                            _member(m, ctx, "    ")
                    elif c.tag == "DW_TAG_member":
                        _member(c, ctx, "  ")
                    elif c.tag == "DW_TAG_subprogram":
                        _sub(c, ctx, "  ")
                print("")
            return
    print("CU not found: %s" % target)


def _bs(die):
    a = die.attributes.get("DW_AT_byte_size")
    return a.value if a is not None else "?"


def _in_ns(die, ctx, ns):
    p = ref(die, "DW_AT_specification", ctx) or die
    par = None
    return p is not None


def _member(m, ctx, ind):
    loc = m.attributes.get("DW_AT_data_member_location")
    off = loc.value if loc is not None else "?"
    print("%s+%-5s %s : %s" % (ind, off, decl_name(m, ctx),
                               tname(ref(m, "DW_AT_type", ctx), ctx)))


def _sub(c, ctx, ind):
    d = ref(c, "DW_AT_specification", ctx) or c
    rt = tname(ref(d, "DW_AT_type", ctx), ctx)
    params = []
    for pc in c.iter_children():
        if pc.tag != "DW_TAG_formal_parameter":
            continue
        params.append(tname(ref(pc, "DW_AT_type", ctx), ctx))
    if not params:
        for pc in d.iter_children():
            if pc.tag != "DW_TAG_formal_parameter":
                continue
            params.append(tname(ref(pc, "DW_AT_type", ctx), ctx))
    acc = d.attributes.get("DW_AT_accessibility")
    const = d.attributes.get("DW_AT_const_value")
    flags = []
    if acc is not None:
        flags.append({1: "public", 2: "protected", 3: "private"}.get(acc.value, "?"))
    if const is not None:
        flags.append("const")
    print("%s%s %s(%s) %s" % (ind, rt, decl_name(c, ctx), ", ".join(params),
                              " ".join(flags)))


if __name__ == "__main__":
    main()
