#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Annotate a PIC disassembly listing with the target of lea/mov <disp>(%ebx).

GCC 4.1 i386 PIC code keeps a GOT base in EBX, set by
`call __i686.get_pc_thunk.bx` / `add $imm,%ebx`; for the shared object here the
base is always 0x294534, so `<disp>(%ebx)` resolves to a .rodata address.
Read-only references are replaced by `<disp>(%ebx)=0xADDR "literal"`, which
makes the printVariable format/name arguments and TDR error strings visible.

Usage: dis_annotate.py <elf> <objdump-listing.txt>
"""
from __future__ import print_function

import re
import sys

from elftools.elf.elffile import ELFFile

LEA = re.compile(r"^\s*([0-9a-f]+):\s+(?:[0-9a-f]{2} )+\s*\t?(.*)$")
DISP = re.compile(r"(-?0x[0-9a-f]+)\(%ebx\)")
ADDEB = re.compile(r"add\s+\$(-?0x[0-9a-f]+),%ebx")


class Image(object):
    def __init__(self, path):
        self.f = open(path, "rb")
        self.elf = ELFFile(self.f)
        self.secs = []
        for sec in self.elf.iter_sections():
            self.secs.append((sec["sh_addr"], sec["sh_addr"] + sec["sh_size"],
                              sec["sh_offset"], sec.name))

    def read(self, addr, n=256):
        for lo, hi, off, _name in self.secs:
            if lo <= addr < hi:
                self.f.seek(off + (addr - lo))
                return self.f.read(min(n, hi - addr))
        return None

    def cstr(self, addr):
        b = self.read(addr)
        if b is None:
            return None
        z = b.find(b"\0")
        if z < 0:
            return None
        try:
            return b[:z].decode("utf-8")
        except UnicodeDecodeError:
            return b[:z].decode("latin-1")


def main():
    img = Image(sys.argv[1])
    lines = open(sys.argv[2], "r", errors="replace").read().splitlines()
    base = None
    for line in lines:
        m = LEA.match(line)
        if not m:
            print(line)
            continue
        cur = int(m.group(1), 16)
        asm = m.group(2)
        nm = ADDEB.match(asm)
        if nm:
            # the `add $imm,%ebx` uses its own address as the PIC reference
            base = cur + int(nm.group(1), 16)
            print("%s: %s   [ebx base = 0x%x]" % (m.group(1), asm, base))
            continue
        if base is not None and "%ebx)" in asm:
            def rep(mm):
                addr = base + int(mm.group(1), 16)
                lit = img.cstr(addr)
                if lit is None:
                    return mm.group(0)
                return "%s(%%ebx)=0x%x %r" % (mm.group(1), addr, lit)
            asm = DISP.sub(rep, asm)
        print("%s: %s" % (m.group(1), asm))


if __name__ == "__main__":
    main()
