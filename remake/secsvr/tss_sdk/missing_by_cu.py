#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Attribute ORIG dynamic symbols to DWARF CUs and report missing ones per CU.

Usage: missing_by_cu.py <elf> <missing-demangled-names-file>
Outputs: "count cu" lines sorted descending, plus the missing names per top CU
with --detail <cu-substring>.
"""
from __future__ import print_function

import subprocess
import sys

from elftools.elf.elffile import ELFFile


def deresolve(v):
    return v.decode("utf-8", "replace") if isinstance(v, bytes) else v


def attr_name(die, key):
    a = die.attributes.get(key)
    return None if a is None else deresolve(a.value)


def follow(cu, die, ref):
    a = die.attributes.get(ref)
    if a is None:
        return None
    for cand in (cu.cu_offset + a.value, a.value):
        try:
            return cu.get_DIE_from_refaddr(cand)
        except Exception:
            pass
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
    path, missing_file = sys.argv[1], sys.argv[2]
    detail = sys.argv[4] if len(sys.argv) > 4 and sys.argv[3] == "--detail" else None
    missing = set(x.strip() for x in open(missing_file) if x.strip())

    # addr -> mangled symbol from dynsym
    out = subprocess.check_output(["nm", "-D", "--defined-only", path]).decode()
    by_addr = {}
    for line in out.splitlines():
        parts = line.split()
        if len(parts) != 3:
            continue
        addr, typ, name = parts
        if typ.lower() not in ("t", "w"):
            continue
        by_addr[int(addr, 16)] = (name, typ)

    rows = []
    with open(path, "rb") as f:
        dwarf = ELFFile(f).get_dwarf_info()
        for cu in dwarf.iter_CUs():
            top = cu.get_top_DIE()
            cu_name = attr_name(top, "DW_AT_name") or ""
            for die in walk(top):
                if die.tag != "DW_TAG_subprogram":
                    continue
                low = die.attributes.get("DW_AT_low_pc")
                high = die.attributes.get("DW_AT_high_pc")
                if low is None or high is None:
                    continue
                mangled = resolve(cu, die, "DW_AT_MIPS_linkage_name") or \
                    resolve(cu, die, "DW_AT_linkage_name")
                if not mangled:
                    continue
                rows.append((low.value, high.value, cu_name, mangled))

    cu_counts = {}
    cu_examples = {}
    proc = subprocess.Popen(["c++filt"], stdin=subprocess.PIPE,
                            stdout=subprocess.PIPE)
    payload = b"\n".join(r[3].encode() for r in rows)
    dem, _ = proc.communicate(payload)
    dems = dem.decode("utf-8", "replace").splitlines()
    for (low, high, cu_name, mangled), d in zip(rows, dems):
        if d in missing:
            cu_counts[cu_name] = cu_counts.get(cu_name, 0) + 1
            cu_examples.setdefault(cu_name, []).append(d)
    for cu_name in sorted(cu_counts, key=lambda k: -cu_counts[k]):
        print("%5d %s" % (cu_counts[cu_name], cu_name))
    if detail:
        print("\n== examples for %s ==" % detail)
        for cu_name, ex in cu_examples.items():
            if detail in cu_name:
                for e in ex[:80]:
                    print(e)
    print("\nattributed=%d" % sum(cu_counts.values()))


if __name__ == "__main__":
    main()
