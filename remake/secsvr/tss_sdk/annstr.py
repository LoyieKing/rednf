#!/usr/bin/env python3
"""Annotate objdump i386 PIC code: resolve 'lea -0xNNN(%ebx)' data references to
NUL-terminated strings in libtss_sdk.so. Usage:
  annstr.py <start> <stop>
EBX base is auto-detected from 'add $0xNNNNN,%ebx' (pc-thunk pattern)."""
import re, subprocess, sys
from elftools.elf.elffile import ELFFile

SO = "/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394"
f = ELFFile(open(SO, "rb"))
segs = [(s.header.p_vaddr, s.header.p_filesz, s.header.p_offset)
        for s in f.iter_segments() if s.header.p_type == "PT_LOAD"]


def read(addr):
    for va, sz, off in segs:
        if va <= addr < va + sz:
            f.stream.seek(off + (addr - va))
            return f.stream.read(1500).split(b"\0")[0].decode("latin-1")
    return None


start, stop = sys.argv[1], sys.argv[2]
out = subprocess.run(["objdump", "-d", "-C", "--start-address=" + start,
                      "--stop-address=" + stop, SO],
                     capture_output=True, text=True).stdout
ebx = None
for line in out.splitlines():
    m = re.search(r"add\s+\$0x([0-9a-f]+),%ebx", line)
    if m and "get_pc_thunk" in out.splitlines()[0] or True:
        if m:
            # ebx = addr_of_next_insn + imm; approximate using the line address
            a = int(line.split(":")[0].strip(), 16)
            # instruction is 6 bytes
            ebx = a + int(m.group(1), 16)
            break
print("EBX=0x%x" % (ebx or 0))
for line in out.splitlines():
    m = re.search(r"lea\s+(-0x[0-9a-f]+)\(%ebx\),%e\w+", line)
    if m:
        va = (ebx + int(m.group(1), 16)) & 0xffffffff
        print(line + "   ; 0x%x %r" % (va, read(va)))
