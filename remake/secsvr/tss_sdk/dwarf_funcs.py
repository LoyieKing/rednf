#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""List DWARF subprograms (incl. inlined callees) of a CU from an ELF binary.

Usage: dwarf_funcs.py <elf> <cu-name-substring> [--inlined]
Default lists namespace/CU-scope DW_TAG_subprogram DIEs with name, decl file
and line, declared-ness and address range.  --inlined additionally lists every
DW_TAG_inlined_subroutine callee name seen inside them (the compiler-inlined
static helpers of the original source).
"""
from __future__ import print_function

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


def i(die, name):
    a = die.attributes.get(name)
    return None if a is None else a.value


def walk(die, depth=0):
    for c in die.iter_children():
        yield c, depth
        for gc, gd in walk(c, depth + 1):
            yield gc, gd


def main():
    elf_path, target = sys.argv[1], sys.argv[2]
    want_inlined = "--inlined" in sys.argv
    with open(elf_path, "rb") as f:
        dwarf = ELFFile(f).get_dwarf_info()
        for cu in dwarf.iter_CUs():
            top = cu.get_top_DIE()
            name = s(top, "DW_AT_name") or ""
            if target not in name:
                continue
            print("== CU %s ==" % name)
            for die, depth in walk(top):
                if die.tag == "DW_TAG_subprogram":
                    nm = s(die, "DW_AT_name")
                    if not nm or s(die, "DW_AT_declaration"):
                        continue
                    low = i(die, "DW_AT_low_pc")
                    high = i(die, "DW_AT_high_pc")
                    line = i(die, "DW_AT_decl_line")
                    fil = i(die, "DW_AT_decl_file")
                    spec = s(die, "DW_AT_specification")
                    extra = ""
                    if spec:
                        extra = " spec:%s" % spec
                    if low:
                        extra += " addr:0x%x-0x%x" % (low, low + (high or 0))
                    print("%s%s  line:%s file:%s depth:%d" % (
                        "  " * depth, nm, line, fil, depth))
                    if not want_inlined:
                        continue
                    seen = set()
                    for d2, _ in walk(die):
                        if d2.tag == "DW_TAG_inlined_subroutine":
                            callee = s(d2, "DW_AT_name")
                            if callee and callee not in seen:
                                seen.add(callee)
                                cl = i(d2, "DW_AT_call_line")
                                print("%s    inlined: %s (call_line %s)" % (
                                    "  " * depth, callee, cl))


if __name__ == "__main__":
    main()
