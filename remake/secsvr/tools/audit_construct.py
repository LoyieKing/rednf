#!/usr/bin/env python3
"""Extract construct() field writes (offset -> immediate) from a 32-bit ELF binary.

Usage: audit_construct.py <binary> <symbol-substring>
Prints for each construct function the offsets and immediates written relative to `this`,
plus memset call sizes when the size is an immediate.
"""
import re
import subprocess
import sys


def disasm(binary, start, end):
    out = subprocess.run(
        ["objdump", "-d", "--start-address=0x%x" % start, "--stop-address=0x%x" % end, binary],
        capture_output=True, text=True).stdout
    return out


def main():
    binary = sys.argv[1]
    pattern = sys.argv[2]
    nm = subprocess.run(["nm", "-S", binary], capture_output=True, text=True).stdout
    funcs = []
    for line in nm.splitlines():
        parts = line.split()
        if len(parts) == 3:
            addr, typ, name = int(parts[0], 16), parts[1], parts[2]
            size = None
        elif len(parts) == 4:
            addr, size, typ, name = int(parts[0], 16), int(parts[1], 16), parts[2], parts[3]
        else:
            continue
        if typ not in ("T", "t"):
            continue
        if pattern in name and "construct" in name:
            funcs.append((addr, size, name))
    funcs.sort()
    for i, (addr, size, name) in enumerate(funcs):
        end = addr + (size if size else 0x200)
        asm = disasm(binary, addr, end)
        print("== %s @ 0x%x" % (name, addr))
        # find the register that holds `this` (loaded from 0x8(%ebp) in prologue)
        this_regs = set(re.findall(r"mov\s+0x8\(%ebp\),%(e[a-z]{2})\b", asm))
        if not this_regs:
            this_regs = set(re.findall(r"mov\s+0x8\(%ebp\),%(e[a-z]{2})", asm))
        # find writes to this-relative offsets with immediate
        for m in re.finditer(r"movl\s+\$([-0-9a-fx]+),((?:0x[0-9a-f]+)?)\(%(\w+)\)", asm):
            imm, off, reg = m.groups()
            if reg in this_regs and reg != "esp":
                print("    +%s = %s" % (off or "0x0", imm))
        for m in re.finditer(r"memset@plt", asm):
            # look at preceding lines for size/ptr setup
            pre = asm[max(0, m.start() - 300):m.start()]
            sizes = re.findall(r"movl\s+\$0x[0-9a-f]+,0x8\(%esp\)", pre)
            if sizes:
                print("    memset size=%s" % sizes[-1])


if __name__ == "__main__":
    main()
