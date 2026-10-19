#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Debug helper: dump DW_TAG_inlined_subroutine DIEs of one CU with the raw
DW_AT_abstract_origin form/value, the resolved callee DIE and its attributes.

Usage: dwarf_inl_dbg.py <elf> <cu-name-substring> [max]
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


def walk(die):
    for c in die.iter_children():
        yield c
        for gc in walk(c):
            yield gc


def attrs_of(die):
    out = []
    for k, v in die.attributes.items():
        val = v.value.hex() if isinstance(v.value, (bytes, bytearray)) \
            else v.value
        out.append("%s=%s(%s)" % (k, val, v.form))
    return " ".join(out)


def main():
    elf_path, target = sys.argv[1], sys.argv[2]
    limit = int(sys.argv[3]) if len(sys.argv) > 3 else 3
    with open(elf_path, "rb") as f:
        dwarf = ELFFile(f).get_dwarf_info()
        for cu in dwarf.iter_CUs():
            top = cu.get_top_DIE()
            if target not in (s(top, "DW_AT_name") or ""):
                continue
            n = 0
            for die in walk(top):
                if die.tag != "DW_TAG_inlined_subroutine":
                    continue
                print("off=0x%x cu=0x%x %s" % (die.offset, cu.cu_offset,
                                               attrs_of(die)))
                ao = die.attributes.get("DW_AT_abstract_origin")
                if ao is not None:
                    for base in (cu.cu_offset, 0):
                        try:
                            d2 = dwarf.get_DIE_from_refaddr(ao.value + base)
                            print("    base=0x%x -> %s | %s" % (
                                base, d2.tag, attrs_of(d2)))
                        except Exception as exc:
                            print("    base=0x%x failed %s" % (base, exc))
                n += 1
                if n >= limit:
                    break
            break
    return


if __name__ == "__main__":
    main()
