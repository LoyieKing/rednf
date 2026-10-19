#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Enumerate the concrete (address-bearing) DW_TAG_subprogram DIEs of one CU.

Usage: cu_syms.py <elf> <cu-name-substring>

For each DW_TAG_subprogram in the matching CU that carries DW_AT_low_pc, print
  <hex-low_pc> <hex-size> <ret-type> <mangled-name>
preceded by a header line giving the CU name and DWARF version.  The size comes
from DW_AT_high_pc (DWARF2-3: end address).  Names are resolved through
DW_AT_specification / DW_AT_abstract_origin, the return type through
DW_AT_type of the declaration DIE.  Output is sorted by low_pc so it can be
joined against `nm -DC -S` output.
"""
from __future__ import print_function

import sys

from elftools.elf.elffile import ELFFile


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


def walk(die):
    for c in die.iter_children():
        yield c
        for gc in walk(c):
            yield gc


def ref(die, attr, ctx):
    """Resolve DW_AT_<attr> (ref4 relative to the CU, or ref_addr)."""
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


def decl_of(die, ctx):
    """The DIE carrying DW_AT_type / the declaration name."""
    if die.attributes.get("DW_AT_type") is not None:
        return die
    for attr in ("DW_AT_specification", "DW_AT_abstract_origin"):
        r = ref(die, attr, ctx)
        if r is not None and r.attributes.get("DW_AT_type") is not None:
            return r
    return None


def tname(die, ctx, depth=0):
    """Render a type DIE (return types / parameter types)."""
    if die is None or depth > 6:
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
    if tag == "DW_TAG_volatile_type":
        return "volatile " + tname(ref(die, "DW_AT_type", ctx), ctx, depth + 1)
    if tag == "DW_TAG_reference_type":
        return tname(ref(die, "DW_AT_type", ctx), ctx, depth + 1) + "&"
    if tag in ("DW_TAG_structure_type", "DW_TAG_class_type",
               "DW_TAG_union_type", "DW_TAG_enumeration_type"):
        nm = s(die, "DW_AT_name")
        if nm:
            return nm
        sp = ref(die, "DW_AT_specification", ctx)
        return (s(sp, "DW_AT_name") if sp is not None else None) or "<anon>"
    return tag


def nm_of(die, ctx):
    for d in (die, decl_of(die, ctx)):
        if d is None:
            continue
        nm = s(d, "DW_AT_MIPS_linkage_name") or s(d, "DW_AT_linkage_name") \
            or s(d, "DW_AT_name")
        if nm:
            return nm
    for attr in ("DW_AT_specification", "DW_AT_abstract_origin"):
        r = ref(die, attr, ctx)
        if r is None:
            continue
        nm = s(r, "DW_AT_MIPS_linkage_name") or s(r, "DW_AT_linkage_name") \
            or s(r, "DW_AT_name")
        if nm:
            return nm
    return "?"


def main():
    elf_path, target = sys.argv[1], sys.argv[2]
    with open(elf_path, "rb") as f:
        dwarf = ELFFile(f).get_dwarf_info()
        for cu in dwarf.iter_CUs():
            top = cu.get_top_DIE()
            name = s(top, "DW_AT_name") or ""
            if target not in name:
                continue
            ctx = (dwarf, cu.cu_offset)
            print("# CU %s (dwarf v%d)" % (name, cu["version"]))
            rows = []
            for die in walk(top):
                if die.tag != "DW_TAG_subprogram":
                    continue
                low = i(die, "DW_AT_low_pc")
                if low is None:
                    continue
                hdie = die.attributes.get("DW_AT_high_pc")
                size = 0
                if hdie is not None:
                    if cu["version"] >= 4 and hdie.form != "DW_FORM_addr":
                        size = hdie.value
                    else:
                        size = hdie.value - low
                ret = tname(ref(decl_of(die, ctx) or die, "DW_AT_type", ctx), ctx)
                rows.append((low, size, ret, nm_of(die, ctx)))
            rows.sort()
            for low, size, ret, nm in rows:
                print("%08x %08x %-24s %s" % (low, size, ret, nm))
            print("# total %d" % len(rows))
            return
    print("no CU matching %r" % target)


if __name__ == "__main__":
    main()
