#!/usr/bin/env python3
# Dump NUL-terminated strings at ELF virtual addresses: vstr.py <addr> [<addr> ...]
import sys
from elftools.elf.elffile import ELFFile

f = ELFFile(open("/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394", "rb"))
segs = []
for s in f.iter_segments():
    if s.header.p_type == "PT_LOAD":
        segs.append((s.header.p_vaddr, s.header.p_filesz, s.header.p_offset))

def read(addr):
    for va, sz, off in segs:
        if va <= addr < va + sz:
            f.stream.seek(off + (addr - va))
            data = f.stream.read(2000)
            return data.split(b"\0")[0].decode("latin-1")
    return None

for a in sys.argv[1:]:
    if a.startswith("0x"):
        a = a[2:]
    addr = int(a, 16)
    print("%08x %r" % (addr, read(addr)))
