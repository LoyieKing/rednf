#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""List all DWARF CU names in an ELF, optionally filtered by substring.

Usage: dwarf_cus.py <elf> [substring]
"""
from __future__ import print_function

import sys

from elftools.elf.elffile import ELFFile


def main():
    path = sys.argv[1]
    needle = sys.argv[2] if len(sys.argv) > 2 else ""
    with open(path, "rb") as f:
        dwarf = ELFFile(f).get_dwarf_info()
        for cu in dwarf.iter_CUs():
            a = cu.get_top_DIE().attributes.get("DW_AT_name")
            name = a.value.decode("utf-8", "replace") if a else ""
            if needle in name:
                print(name)


if __name__ == "__main__":
    main()
