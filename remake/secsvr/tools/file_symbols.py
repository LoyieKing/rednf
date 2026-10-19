#!/usr/bin/env python3
"""列出二进制中某个源文件对应的全部符号（地址 + 行号 + demangled 名）。

用法: python3 file_symbols.py <binary> <src-路径片段>
例:   python3 file_symbols.py <gunnersvr> TdrParse.cpp
      python3 file_symbols.py <gunnersvr> comm_proto_public_cfgsvr.cpp
"""
import subprocess
import sys
import re
from collections import defaultdict


def main():
    binary = sys.argv[1]
    frag = sys.argv[2]
    out = subprocess.run(["nm", "-C", binary], capture_output=True, text=True).stdout
    addrs = []
    mp = {}
    for line in out.splitlines():
        p = line.split(None, 2)
        if len(p) < 3:
            continue
        addr, kind, name = p
        if kind not in ("T", "t", "W", "w") or not re.match(r"^[0-9a-f]+$", addr):
            continue
        if re.match(r"^(global constructors|global destructors|__static_initialization|_GLOBAL__|__tcf_)", name):
            continue
        if name.startswith("__x86.get_pc_thunk") or re.match(r"^\.L[0-9]+$", name):
            continue
        addrs.append(int(addr, 16))
        mp[int(addr, 16)] = (name, kind)
    r = subprocess.run(
        ["addr2line", "-e", binary] + [f"0x{a:x}" for a in addrs],
        capture_output=True, text=True).stdout
    rows = []
    for a, loc in zip(addrs, r.splitlines()):
        fname, line = loc.split(":")
        if frag in fname:
            rows.append((int(line) if line.strip() else 0, a, mp[a][0], mp[a][1], fname))
    rows.sort()
    for line, a, name, kind, fname in rows:
        print(f"{a:08x} {kind} line={line:<6} {name}  [{fname}]")


if __name__ == "__main__":
    main()
