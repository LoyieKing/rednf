#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""List defined symbols attributed to a CU by DWARF (low_pc + mangled name).

Usage: cu_symbols.py <elf> <cu-name-substring> [--sizes]
GCC 4.1 emits DW_AT_specification as a CU-relative reference; names live on the
referenced declaration DIE, so both forms are resolved here.
"""
from __future__ import print_function

import sys

from elftools.elf.elffile import ELFFile


def deresolve(value):
    if isinstance(value, bytes):
        return value.decode("utf-8", "replace")
    return value


def attr_name(die, key):
    a = die.attributes.get(key)
    return None if a is None else deresolve(a.value)


def follow(cu, die, ref_name):
    a = die.attributes.get(ref_name)
    if a is None:
        return None
    off = a.value
    for candidate in (cu.cu_offset + off, off):
        try:
            return cu.get_DIE_from_refaddr(candidate)
        except Exception:
            continue
    return None


def resolve(cu, die, key):
    cur = die
    for _ in range(8):
        v = attr_name(cur, key)
        if v:
            return v
        nxt = follow(cu, cur, "DW_AT_specification") or \
            follow(cu, cur, "DW_AT_abstract_origin")
        if nxt is None:
            return None
        cur = nxt
    return None


def walk(die):
    yield die
    for c in die.iter_children():
        for g in walk(c):
            yield g


def main():
    path, needle = sys.argv[1], sys.argv[2]
    want_sizes = "--sizes" in sys.argv
    with open(path, "rb") as f:
        dwarf = ELFFile(f).get_dwarf_info()
        for cu in dwarf.iter_CUs():
            top = cu.get_top_DIE()
            nm = attr_name(top, "DW_AT_name") or ""
            if needle not in nm:
                continue
            print("== CU %s" % nm)
            for die in walk(top):
                if die.tag != "DW_TAG_subprogram":
                    continue
                low = die.attributes.get("DW_AT_low_pc")
                if low is None:
                    continue
                name = resolve(cu, die, "DW_AT_name")
                mangled = resolve(cu, die, "DW_AT_MIPS_linkage_name") or \
                    resolve(cu, die, "DW_AT_linkage_name") or name
                high = die.attributes.get("DW_AT_high_pc")
                size = ""
                if want_sizes and high is not None:
                    size = " size=0x%x" % (high.value - low.value)
                print("0x%08x %s%s" % (low.value, mangled, size))
            return
    print("CU not found: %s" % needle)


if __name__ == "__main__":
    main()
