#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Inventory static-initialization blocks (global ctors) of one ELF.

The ORIG library is GCC 4.1: `.ctors` holds one `_GLOBAL__I_*` thunk per TU
that tail-calls that TU's `__static_initialization_and_destruction_0(1,0xffff)`.
The reconstructed library may use `.init_array` with `_GLOBAL__sub_I_*`
entries; both forms are handled.

For every init function the tool records the ordered call targets, the
`__cxa_atexit` registrations, the data/string addresses it references (with the
DWARF variable or ELF symbol at that address) and the stack immediates, plus
the DWARF CU that owns the code. That is enough to reconstruct the block and to
compare reconstructed vs ORIG side effects.

Usage: static_init.py <elf> <out.json> [--limit-lines N]
"""
from __future__ import print_function

import json
import re
import struct
import subprocess
import sys
from collections import OrderedDict

from elftools.elf.elffile import ELFFile

LINE = re.compile(r"^\s*([0-9a-f]+):\s+((?:[0-9a-f]{2} )+)\s*(.*)$")
ADDR_SYM = re.compile(r"^\s*([0-9a-f]+)\s+<([^>]+)>\s*$")


def decoded(v):
    if isinstance(v, (bytes, bytearray)):
        return v.decode("utf-8", "replace")
    return v


def astr(die, key):
    a = die.attributes.get(key)
    return None if a is None else decoded(a.value)


class Image(object):
    def __init__(self, path):
        self.path = path
        self.f = open(path, "rb")
        self.elf = ELFFile(self.f)
        self.sections = []
        for sec in self.elf.iter_sections():
            if not sec["sh_size"]:
                continue
            self.sections.append((sec["sh_addr"], sec["sh_addr"] + sec["sh_size"],
                                  sec["sh_offset"], sec.name,
                                  bool(sec["sh_flags"] & 0x4)))
        self.sym = []
        self.by_addr = {}
        for secname in (".symtab", ".dynsym"):
            sec = self.elf.get_section_by_name(secname)
            if sec is None:
                continue
            for s in sec.iter_symbols():
                if not s.name or s["st_shndx"] == "SHN_UNDEF":
                    continue
                rec = (s.name, s["st_value"], s["st_size"],
                       s["st_info"]["type"])
                self.sym.append(rec)
                self.by_addr.setdefault(s["st_value"], []).append(rec)
        self.sym.sort(key=lambda r: r[1])

    def read(self, addr, n=256):
        for lo, hi, off, _name, _alloc in self.sections:
            if lo <= addr < hi:
                self.f.seek(off + (addr - lo))
                return self.f.read(min(n, hi - lo))
        return None

    def cstr(self, addr):
        b = self.read(addr)
        if not b:
            return None
        z = b.find(b"\0")
        if z < 0:
            z = min(len(b), 120)
        try:
            s = b[:z].decode("utf-8")
        except UnicodeDecodeError:
            return None
        if not s or any(ord(c) < 0x20 for c in s):
            return None
        return s

    def containing(self, addr):
        lo, hi = 0, len(self.sym)
        while lo < hi:
            mid = (lo + hi) // 2
            if self.sym[mid][1] <= addr:
                lo = mid + 1
            else:
                hi = mid
        for i in range(lo - 1, max(-1, lo - 6), -1):
            name, a, size, typ = self.sym[i]
            if a == addr or (a < addr and size and addr < a + size):
                return name, a, size, typ
        return None

    def got_relocs(self):
        """GOT slot address -> symbol name (R_386_GLOB_DAT / R_386_32)."""
        out = {}
        out_text = subprocess.run(["readelf", "-rW", self.path],
                                  stdout=subprocess.PIPE, text=True).stdout
        for line in out_text.splitlines():
            p = line.split()
            if len(p) < 5 or not p[0].startswith("0"):
                continue
            try:
                off = int(p[0], 16)
            except ValueError:
                continue
            kind = p[2]
            if kind in ("R_386_GLOB_DAT", "R_386_32", "R_386_RELATIVE"):
                name = p[4] if kind != "R_386_RELATIVE" else "<relative>"
                out[off] = name
        return out


def dwarf_ranges(img):
    """(low, high, cu_name) for every subprogram definition, plus name map."""
    dwarf = img.elf.get_dwarf_info()
    ranges = []
    for cu in dwarf.iter_CUs():
        top = cu.get_top_DIE()
        cu_name = astr(top, "DW_AT_name") or ""
        stack = [top]
        while stack:
            die = stack.pop()
            for c in die.iter_children():
                if c.tag == "DW_TAG_subprogram":
                    low = c.attributes.get("DW_AT_low_pc")
                    if low is None:
                        continue
                    high = c.attributes.get("DW_AT_high_pc")
                    if high is None:
                        continue
                    size = (high.value if high.form != "DW_FORM_addr"
                            else high.value - low.value)
                    ranges.append((low.value, low.value + size, cu_name))
                elif c.tag in ("DW_TAG_lexical_block",):
                    continue
                else:
                    stack.append(c)
    ranges.sort()
    return ranges


def cu_for(ranges, addr):
    lo, hi = 0, len(ranges)
    best = None
    while lo < hi:
        mid = (lo + hi) // 2
        if ranges[mid][0] <= addr:
            best = ranges[mid]
            lo = mid + 1
        else:
            hi = mid
    if best and best[0] <= addr < best[1]:
        return best[2]
    # ranges is sorted by low; a later-starting range may still contain addr
    for r in ranges[max(0, lo - 4):lo + 4]:
        if r[0] <= addr < r[1]:
            return r[2]
    return None


def init_addresses(img):
    """Return [(init_addr, thunk_sym)] for .ctors/.init_array entries."""
    out = []
    for secname in (".init_array", ".ctors"):
        sec = img.elf.get_section_by_name(secname)
        if sec is None or not sec["sh_size"]:
            continue
        data = sec.data()
        for i in range(0, len(data) - 3, 4):
            val = struct.unpack_from("<I", data, i)[0]
            if val in (0, 0xFFFFFFFF):
                continue
            out.append((val, secname))
    return out


def disasm(img, start, stop):
    text = subprocess.run(["objdump", "-d", "-C", "-M", "intel",
                           "--start-address=%d" % start,
                           "--stop-address=%d" % stop, img.path],
                          stdout=subprocess.PIPE, text=True).stdout
    insns = []
    for line in text.splitlines():
        m = LINE.match(line)
        if not m:
            continue
        addr = int(m.group(1), 16)
        raw = m.group(2).strip().split()
        asm = m.group(3).strip()
        insns.append((addr, len(raw), asm))
    return insns


def decode_init(img, start, stop, relocs, vars_by_addr, limit):
    insns = disasm(img, start, stop)
    calls = []
    refs = []
    imms = []
    tail = []
    tail_addr = None
    regs = {}
    base = None
    thunk_pending = None
    for addr, ilen, asm in insns:
        if "get_pc_thunk" in asm:
            m = re.search(r"get_pc_thunk\.(\w+)", asm)
            reg = m.group(1) if m else "bx"
            regs["e" + reg] = None
            thunk_pending = "e" + reg
            continue
        if thunk_pending and asm.startswith("add "):
            m = re.match(r"add\s+(\w+),(0x[0-9a-f]+)", asm)
            if m and m.group(1) == thunk_pending:
                base = addr + int(m.group(2), 16)
                regs[thunk_pending] = base
                thunk_pending = None
                continue
        m = re.match(r"(call|jmp)\s+([0-9a-f]+)\s*(?:<([^>]+)>)?", asm)
        if m:
            op, direct, name = m.group(1), m.group(2), m.group(3)
            if op == "jmp" and not (start <= int(direct, 16) < stop):
                tail_addr = int(direct, 16)
            if name:
                target_name = re.sub(r"@plt$", "", name)
                target_name = re.sub(r"\+0x[0-9a-f]+$", "", target_name)
                if op == "call":
                    calls.append(target_name)
                else:
                    tail.append(target_name)
            elif op == "call":
                calls.append("<indirect>")
            continue
        if "call " in asm and re.search(r"call\s+e[a-z]{2}", asm):
            calls.append("<indirect>")
            continue
        m = re.match(r"lea\s+(\w+),\[(\w+)([+-]0x[0-9a-f]+)\]", asm)
        if m:
            reg, rbase, disp = m.group(1), m.group(2), int(m.group(3), 16)
            if rbase in regs and regs[rbase] is not None:
                target = regs[rbase] + disp if m.group(3)[0] == "+" else \
                    regs[rbase] - abs(disp)
                key = target
                regs[reg] = target
                name = img.containing(target)
                lit = img.cstr(target)
                var = vars_by_addr.get(target)
                refs.append({"addr": target,
                             "sym": name[0] if name else None,
                             "str": lit,
                             "var": var[0]["pretty"] if var else None})
            continue
        m = re.match(r"mov\s+(\w+),DWORD PTR \[(\w+)([+-]0x[0-9a-f]+)\]", asm)
        if m:
            reg, rbase, disp = m.group(1), m.group(2), int(m.group(3), 16)
            if rbase in regs and regs[rbase] is not None:
                slot = regs[rbase] + disp if m.group(3)[0] == "+" else \
                    regs[rbase] - abs(disp)
                name = relocs.get(slot)
                if name:
                    regs[reg] = ("got", name)
            continue
        m = re.match(r"mov\s+DWORD PTR \[esp([+-]0x[0-9a-f]+)?\],"
                     r"(0x[0-9a-f]+|[a-z0-9]+)", asm)
        if m:
            off = m.group(1) or "+0x0"
            val = m.group(2)
            if val.startswith("0x"):
                imms.append(int(val, 16))
            else:
                regv = regs.get(val)
                if isinstance(regv, tuple):
                    imms.append(regv[1])
                elif isinstance(regv, int):
                    imms.append(regv)
            continue
        m = re.match(r"mov\s+DWORD PTR \[esp([+-]0x[0-9a-f]+)?\],eax", asm)
        if m:
            regv = regs.get("eax")
            if isinstance(regv, tuple):
                imms.append(regv[1])
            elif isinstance(regv, int):
                imms.append(regv)
            continue
        m = re.match(r"push\s+(0x[0-9a-f]+|<([^>]+)>)", asm)
        if m:
            if m.group(2):
                imms.append(m.group(2))
            else:
                imms.append(int(m.group(1), 16))
            continue
    return {"calls": calls, "refs": refs, "imms": imms,
            "tail": tail, "tail_addr": tail_addr,
            "insn_count": len(insns)}


def main(argv):
    if len(argv) < 3:
        print(__doc__, file=sys.stderr)
        return 2
    path, out_path = argv[1], argv[2]
    img = Image(path)
    ranges = dwarf_ranges(img)
    relocs = img.got_relocs()
    vars_text = None
    vars_by_addr = {}
    if len(argv) > 4 and argv[3] == "--model":
        vars_text = argv[4]
    # reuse the dwarf_model dump when given; otherwise no variable names
    for a in argv[3:]:
        if a.endswith(".json"):
            with open(a) as f:
                m = json.load(f)
            vars_by_addr = {int(k): v for k, v in m.get("vars", {}).items()}
    entries = init_addresses(img)
    groups = OrderedDict()
    for addr, secname in entries:
        name = img.containing(addr)
        sym = name[0] if name else "<no symbol>"
        if sym in ("frame_dummy", "__do_global_dtors_aux",
                   "__do_global_ctors_aux"):
            continue
        # size from symtab when available
        stop = addr + 1
        for n, a, size, typ in img.sym:
            if a == addr and size:
                stop = addr + size
                break
        info = decode_init(img, addr, stop, relocs, vars_by_addr, 0)
        cu = cu_for(ranges, addr)
        if cu is None and sym.startswith("_GLOBAL__sub_I_"):
            cu = sym[len("_GLOBAL__sub_I_"):]
        if cu is None and sym.startswith("_GLOBAL__I_"):
            cu = sym[len("_GLOBAL__I_"):]
        row = {"addr": addr, "sym": sym, "section": secname,
               "size": stop - addr, "cu": cu}
        row.update(info)
        # GCC 4.1 lanes: _GLOBAL__I_* tail-jumps to that TU's
        # __static_initialization_and_destruction_0; decode the real body.
        if info.get("tail_addr"):
            tgt = info["tail_addr"]
            tsize = 0
            for n, a, size, typ in img.sym:
                if a == tgt and size:
                    tsize = size
                    break
            row["target_addr"] = tgt
            row["target_size"] = tsize
            if tsize:
                tinfo = decode_init(img, tgt, tgt + tsize, relocs,
                                    vars_by_addr, 0)
                row["calls"] = tinfo["calls"]
                row["refs"] = tinfo["refs"]
                row["imms"] = tinfo["imms"]
                row["insn_count"] = tinfo["insn_count"]
        # GCC 13 partitions init code into <sym>.cold; merge it so the call
        # multiset is comparable with GCC 4.1's single-function emission.
        cold = sym + ".cold"
        for n, a, size, typ in img.sym:
            if n == cold and size:
                cinfo = decode_init(img, a, a + size, relocs, vars_by_addr, 0)
                row["calls"].extend(cinfo["calls"])
                row["refs"].extend(cinfo["refs"])
                row["imms"].extend(cinfo["imms"])
                break
        groups.setdefault(cu or "<unknown>", []).append(row)
    with open(out_path, "w") as f:
        json.dump({"elf": path, "count": len(entries), "groups": groups},
                  f, indent=1, sort_keys=True)
    print("inits=%d groups=%d" % (len(entries), len(groups)))
    for cu, rows in groups.items():
        for r in rows:
            print("%-60s %5d calls=%d refs=%d imms=%d" %
                  ((cu or "<unknown>").rsplit("/", 1)[-1], r["size"],
                   len(r["calls"]), len(r["refs"]), len(r["imms"])))
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
