#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Find every DWARF declaration of a named type across all CUs of an ELF.

Usage: dwarf_type_decl.py <elf> <type-name>

Prints, for each matching DW_TAG_*type DIE, the CU name, the decl_file /
decl_line (from the CU's line table) and the children (enumerators / members),
so a header-owned declaration can be attributed to its source file.
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


def main():
    elf_path, target = sys.argv[1], sys.argv[2]
    with open(elf_path, "rb") as f:
        dwarf = ELFFile(f).get_dwarf_info()
        for cu in dwarf.iter_CUs():
            top = cu.get_top_DIE()
            cu_name = s(top, "DW_AT_name") or ""
            lp = dwarf.line_program_for_CU(cu)
            files = [fe.name.decode("utf-8", "replace")
                     for fe in lp["file_entry"]]
            for die in walk(top):
                if s(die, "DW_AT_name") != target:
                    continue
                df = i(die, "DW_AT_decl_file")
                dl = i(die, "DW_AT_decl_line")
                fn = files[df - 1] if df and df <= len(files) else "?"
                kids = [s(c, "DW_AT_name") for c in die.iter_children()
                        if c.tag in ("DW_TAG_enumerator", "DW_TAG_member")]
                print("%-40s %s:%s  children=%s" % (die.tag, fn, dl, kids))
    return


if __name__ == "__main__":
    main()
