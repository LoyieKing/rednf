#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Print DW_TAG_inlined_subroutine callee names inside selected DWARF subprograms.

Usage: dwarf_inline.py <elf> <cu-name-substring> <callee-name-regex>
Lists, for every DW_TAG_subprogram whose name matches the regex, the distinct
inlined callee names (the compiler-inlined static/inline functions of the
original source) with their call lines.
"""
from __future__ import print_function

import re
import sys

from elftools.elf.elffile import ELFFile


def s(die, name):
    a = die.attributes.get(name)
    if a is None:
        return None
    try:
        return a.value.decode("utf-8", "replace")
    except Exception:
        return str(a.value)


def walk(die):
    for c in die.iter_children():
        yield c
        for gc in walk(c):
            yield gc


def main():
    elf_path, target, regex = sys.argv[1], sys.argv[2], sys.argv[3]
    rx = re.compile(regex)
    with open(elf_path, "rb") as f:
        dwarf = ELFFile(f).get_dwarf_info()
        for cu in dwarf.iter_CUs():
            top = cu.get_top_DIE()
            name = s(top, "DW_AT_name") or ""
            if target not in name:
                continue
            print("== CU %s ==" % name)
            for die in walk(top):
                if die.tag != "DW_TAG_subprogram":
                    continue
                nm = s(die, "DW_AT_name")
                if not nm or not rx.search(nm):
                    continue
                low = die.attributes.get("DW_AT_low_pc")
                print("-- %s @ %s" % (nm, low.value if low else None))
                seen = set()
                for d2 in walk(die):
                    if d2.tag != "DW_TAG_inlined_subroutine":
                        continue
                    callee = s(d2, "DW_AT_name")
                    line = d2.attributes.get("DW_AT_call_line")
                    key = (callee, line.value if line else None)
                    if callee and key not in seen:
                        seen.add(key)
                        print("     inlined %s (call_line %s)" % key)


if __name__ == "__main__":
    main()
