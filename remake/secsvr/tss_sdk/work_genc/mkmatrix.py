#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Emit a <addr_orig> <size_orig> <size_recon> <ratio> <demangled name> TSV for
one CU, using cu_symbols.py as the authoritative ORIG symbol set.

Usage: mkmatrix.py <orig.so> <recon.o> <cu-file-substring>
The join is on the mangled name (cu_symbols.py prints DWARF-attributed mangled
names; nm -S prints the same spelling), so unrelated TUs that merely mention a
class in a template argument can never leak in.
"""
from __future__ import print_function

import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
TOOLS = os.path.dirname(HERE)


def obj_sizes(path):
    out = {}
    for line in subprocess.check_output(
            ["nm", "-S", "--defined-only", path]).decode("utf-8", "replace").splitlines():
        f = line.split(None, 3)
        if len(f) != 4:
            continue
        addr, size, typ, name = f
        if typ.lower() == "u":
            continue
        out[name] = int(size, 16)
    return out


def demangle(names):
    p = subprocess.Popen(["c++filt"], stdin=subprocess.PIPE,
                         stdout=subprocess.PIPE)
    data = ("\n".join(names) + "\n").encode("utf-8")
    out, _ = p.communicate(data)
    return out.decode("utf-8", "replace").splitlines()


def main():
    orig_elf, recon_obj, cu = sys.argv[1], sys.argv[2], sys.argv[3]
    raw = subprocess.check_output(
        ["python3", os.path.join(TOOLS, "cu_symbols.py"), orig_elf, cu, "--sizes"]
    ).decode("utf-8", "replace")
    rows = []
    for line in raw.splitlines():
        m = re.match(r"0x([0-9a-f]+) (\S+) size=0x([0-9a-f]+)", line)
        if not m:
            continue
        rows.append((int(m.group(1), 16), m.group(2), int(m.group(3), 16)))
    sizes = obj_sizes(recon_obj)
    names = demangle([r[1] for r in rows])
    print("addr_orig\tsize_orig\tsize_recon\tratio\tname")
    for (addr, mangled, osz), name in sorted(zip(rows, names)):
        rsz = sizes.get(mangled, -1)
        ratio = (float(rsz) / osz) if (rsz >= 0 and osz) else 0.0
        suffix = "" if rsz >= 0 else "  [MISSING IN RECON]"
        print("%08x\t%d\t%d\t%.3f\t%s%s" % (addr, osz, rsz, ratio, name, suffix))


if __name__ == "__main__":
    main()
