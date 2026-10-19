#!/usr/bin/env python3
"""按源文件核对还原覆盖：二进制每个符号经 DWARF 映射回源文件，与 .o 符号集对照。

用法：
  python3 check_file_coverage.py <binary> <obj 目录> <源文件相对名>...
例：
  python3 check_file_coverage.py <gunnersvr> /tmp/zen_audit zen_os_adapt_file.cpp
输出每个文件的缺符号（二进制有、.o 没有）与多余符号。
"""
import subprocess
import sys
import os
import re
from collections import defaultdict


def nm_syms(path, include_local=True):
    """返回 {demangled_name} 集合（含 T/t/W）。"""
    out = subprocess.run(["nm", "-C", path], capture_output=True, text=True).stdout
    syms = set()
    for line in out.splitlines():
        parts = line.split(None, 2)
        if len(parts) < 3:
            continue
        addr, kind, name = parts
        if kind not in ("T", "t", "W", "w"):
            continue
        if name.startswith("__x86.get_pc_thunk") or name.startswith("DW.ref."):
            continue
        # 汇编器/旧编译器保留的本地基本块标签（如 SUSE GCC 4.1.0 的跳转表
        # .L51/.L84 等）不是函数，无法也无需由源码还原复现。
        if re.match(r"^\.L[0-9]+$", name):
            continue
        # 去掉 GCC 辅助符号
        if re.match(r"^(global constructors|global destructors|__static_initialization|_GLOBAL__|__tcf_)", name):
            continue
        syms.add(name)
    return syms


def file_to_symbols(binary, syms):
    """用 addr2line 把每个符号地址映射到源文件。返回 {basename: {符号名}}。"""
    out = defaultdict(set)
    # 一次性拿 nm -C 的 (addr -> demangled)
    addr_to_sym = {}
    dem_lines = subprocess.run(["nm", "-C", binary], capture_output=True, text=True).stdout
    for line in dem_lines.splitlines():
        parts = line.split(None, 2)
        if len(parts) < 3:
            continue
        addr, kind, name = parts
        if kind not in ("T", "t", "W", "w") or not re.match(r"^[0-9a-f]+$", addr):
            continue
        if re.match(r"^(global constructors|global destructors|__static_initialization|_GLOBAL__|__tcf_)", name):
            continue
        if name.startswith("__x86.get_pc_thunk") or name.startswith("DW.ref."):
            continue
        addr_to_sym[int(addr, 16)] = name

    addrs = sorted(addr_to_sym)
    r = subprocess.run(
        ["addr2line", "-e", binary] + [f"0x{a:x}" for a in addrs],
        capture_output=True, text=True)
    lines = r.stdout.splitlines()
    for a, loc in zip(addrs, lines):
        fname = loc.split(":")[0]
        base = os.path.basename(fname)
        if not base.endswith((".cpp", ".c", ".cc", ".inl", ".tcc")):
            continue
        if addr_to_sym[a] in syms:
            out[base].add(addr_to_sym[a])
    return out


def main():
    binary = sys.argv[1]
    objdir = sys.argv[2]
    targets = sys.argv[3:]
    bin_syms = nm_syms(binary)
    fmap = file_to_symbols(binary, bin_syms)
    ok = True
    for t in targets:
        obj = os.path.join(objdir, t.replace(".cpp", ".o"))
        if not os.path.exists(obj):
            print(f"{t}: 缺 .o（未编译）")
            ok = False
            continue
        have = nm_syms(obj)
        expect = fmap.get(t, set())
        missing = sorted(expect - have)
        extra = sorted(have - bin_syms)
        status = "OK" if not missing else "MISSING"
        if missing:
            ok = False
        print(f"{t}: 预期 {len(expect)} 符号, 已实现 {len(expect & have)}")
        for s in missing:
            print(f"  MISSING {s}")
        for s in extra[:10]:
            print(f"  EXTRA(库有二进制无) {s}")
    sys.exit(0 if ok else 1)


if __name__ == "__main__":
    main()
