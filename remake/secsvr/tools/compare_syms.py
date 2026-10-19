#!/usr/bin/env python3
"""符号级对照：给定库（.a/.o/可执行）vs 给定二进制。

用法：python3 compare_syms.py [--bin <二进制>] [--lib <库或.o>] [--filter 正则]
输出：二进制有而我们缺的符号（按类分组），以及统计。
"""
import re
import subprocess
import sys
import os

BIN = "/mnt/d/Docs/my_sources/dnf_workspace/dnf_installer/build/dnf_data/home/template/neople/secsvr/zergsvr/secagent"
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
LIB = os.path.join(ROOT, "tencrypt_new", "libtencrypt_new.a")


def nm_symbols(path, tfilter=True):
    out = subprocess.run(["nm", "-C", path], capture_output=True, text=True).stdout
    syms = set()
    for line in out.splitlines():
        parts = line.split(None, 2)
        if len(parts) < 3:
            continue
        addr, kind, name = parts
        if tfilter and kind not in ("T", "t", "W", "w", "V", "v"):
            continue
        # 去掉 GCC 辅助符号
        if name.startswith("__x86.get_pc_thunk") or name.startswith("DW.ref."):
            continue
        syms.add(name)
    return syms


def main():
    args = sys.argv[1:]
    bin_path = BIN
    lib_path = LIB
    filt = None
    while args:
        a = args.pop(0)
        if a == "--bin":
            bin_path = args.pop(0)
        elif a == "--lib":
            lib_path = args.pop(0)
        elif a == "--filter":
            filt = re.compile(args.pop(0))
        else:
            print(f"未知参数: {a}")
            sys.exit(1)
    bin_syms = nm_symbols(bin_path)
    our_syms = nm_symbols(lib_path)
    missing = sorted(bin_syms - our_syms)
    extra = sorted(our_syms - bin_syms)
    if filt:
        missing = [s for s in missing if filt.search(s)]
        extra = [s for s in extra if filt.search(s)]
    print(f"二进制符号: {len(bin_syms)}  库符号: {len(our_syms)}")
    print(f"缺（二进制有、库没有）: {len(missing)}")
    for s in missing:
        print("  MISSING", s)
    print(f"多（库有、二进制没有）: {len(extra)}")
    for s in extra:
        print("  EXTRA", s)


if __name__ == "__main__":
    main()
