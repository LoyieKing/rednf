#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Find which DWARF CU (source file + line) covers a given code address.

Usage: dwarf_addr_cu.py <elf> <hex-addr>
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


def main():
    elf_path = sys.argv[1]
    addr = int(sys.argv[2], 16)
    with open(elf_path, "rb") as f:
        dwarf = ELFFile(f).get_dwarf_info()
        for cu in dwarf.iter_CUs():
            top = cu.get_top_DIE()
            cu_name = s(top, "DW_AT_name") or ""
            lp = dwarf.line_program_for_CU(cu)
            if lp is None:
                continue
            for entry in lp.get_entries():
                if entry.state is None:
                    continue
                if entry.state.address == addr:
                    fname = ""
                    try:
                        fname = lp["file_entry"][
                            entry.state.file - 1].name.decode("utf-8", "replace")
                    except Exception:
                        pass
                    print("CU  %s\nhit %s:%s" % (cu_name, fname,
                                                 entry.state.line))
                    return
    print("no CU line entry for 0x%x" % addr)


if __name__ == "__main__":
    main()
