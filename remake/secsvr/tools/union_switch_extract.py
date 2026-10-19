#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Extract union dispatch tables (case value -> branch class) from secagent.

Reads the leading switch chain of each union class method:
  - case 0: `mov %ecx,%eax / or %edx,%eax / je T`
  - case N: `mov %edx,%eax / xor $N,%eax / or %ecx,%eax / je T`
For visualize, skips the leading printVariable call to find the branch call.
Also resolves the printVariable name strings from .rodata.
"""
import mmap
import re
import subprocess
import sys

BIN = "/mnt/d/Docs/my_sources/dnf_workspace/dnf_installer/build/dnf_data/home/template/neople/secsvr/zergsvr/secagent"


def disasm(start, end):
    out = subprocess.run(
        ["objdump", "-d", "--start-address=%d" % start, "--stop-address=%d" % end, BIN],
        capture_output=True, text=True).stdout
    lines = []
    for ln in out.splitlines():
        m = re.match(r"\s*([0-9a-f]+):\s+([0-9a-f ]+)\s+(\S.*)$", ln)
        if m:
            lines.append((int(m.group(1), 16), m.group(3)))
    return lines


def nm_table():
    out = subprocess.run(["nm", BIN], capture_output=True, text=True).stdout
    t = {}
    for ln in out.splitlines():
        m = re.match(r"^([0-9a-f]+)\s+[TtWw]\s+(.*)$", ln)
        if m:
            t[int(m.group(1), 16)] = m.group(2)
    return t


def resolve_sym(nm, addr):
    best = None
    for a, n in sorted(nm.items()):
        if a <= addr:
            best = (a, n)
        else:
            break
    if best is None:
        return None, 0
    return best[1], addr - best[0]


def parse_switch(lines):
    """Return {case_value: target_addr} from the leading switch chain."""
    cases = {}
    # locate first chain entry
    i = 0
    while i < len(lines):
        if re.match(r"xor\s+\$0x[0-9a-f]+,%eax", lines[i][1]):
            if any("mov    %edx,%eax" in lines[j][1] for j in range(max(0, i - 4), i)):
                break
        i += 1
    if i >= len(lines):
        return cases
    # check for case 0: mov %ecx,%eax / or %edx,%eax / je T before the chain
    j = max(0, i - 8)
    for k in range(j, i):
        if re.match(r"mov\s+%ecx,%eax", lines[k][1]):
            for kk in range(k + 1, min(k + 6, i + 1)):
                if re.search(r"or\s+%edx,%eax", lines[kk][1]):
                    for kkk in range(kk + 1, min(kk + 7, len(lines))):
                        je = re.match(r"je\s+([0-9a-f]+)", lines[kkk][1])
                        if je:
                            cases[0] = int(je.group(1), 16)
                            break
                    break
            break
    while i < len(lines):
        m = re.match(r"xor\s+\$0x([0-9a-f]+),%eax", lines[i][1])
        if not m:
            break
        target = None
        for j in range(i + 1, min(i + 8, len(lines))):
            if re.search(r"or\s+%ecx,%eax", lines[j][1]):
                for k in range(j + 1, min(j + 6, len(lines))):
                    je = re.match(r"je\s+([0-9a-f]+)", lines[k][1])
                    if je:
                        target = int(je.group(1), 16)
                        i = k + 1
                        break
                if target is None:
                    # last case: jne <default-tail> followed by handler inline
                    for k in range(j + 1, min(j + 6, len(lines))):
                        jne = re.match(r"jne\s+([0-9a-f]+)", lines[k][1])
                        if jne:
                            # handler starts right after the jne
                            nxt = lines[k + 1][0] if k + 1 < len(lines) else None
                            if nxt is not None and not re.match(r"jmp\s+", lines[k + 1][1]):
                                target = nxt
                                i = k + 1
                            break
                break
        if target is None:
            break
        cases[int(m.group(1), 16)] = target
        while i < len(lines) and not re.match(r"mov\s+%edx,%eax", lines[i][1]):
            if re.match(r"xor\s+\$0x[0-9a-f]+,%eax", lines[i][1]):
                break
            if "call" in lines[i][1] or "jne" in lines[i][1] or "mov    %esi,%eax" in lines[i][1]:
                break
            i += 1
        # skip leading mov %edx,%eax of next entry
        if i < len(lines) and re.match(r"mov\s+%edx,%eax", lines[i][1]):
            i += 1
    return cases


def parse_ranges(lines):
    """Return list of (lo, hi, target) for range tests in the switch prologue."""
    out = []
    for i in range(len(lines)):
        if re.match(r"mov\s+%edx,%esi", lines[i][1]) and re.match(r"mov\s+%ecx,%edi", lines[i + 1][1]) if i + 1 < len(lines) else False:
            j = i + 1
            while j < min(i + 6, len(lines)):
                if re.match(r"add\s+\$0xfffffffe,%esi", lines[j][1]):
                    break
                j += 1
            if j >= min(i + 6, len(lines)):
                continue
            lo = 2
            # adc $0xffffffff,%edi
            k = j + 1
            if k < len(lines) and re.match(r"adc\s+\$0xffffffff,%edi", lines[k][1]):
                pass
            else:
                continue
            # cmp $0x0,%edi ; ja NEXT ; cmp $0xN,%esi ; ja NEXT ; handler
            hi = None
            handler = None
            for k in range(j + 2, min(j + 10, len(lines))):
                m = re.match(r"cmp\s+\$0x([0-9a-f]+),%esi", lines[k][1])
                if m and re.match(r"ja\s+", lines[k + 1][1]) if k + 1 < len(lines) else False:
                    hi = lo + int(m.group(1), 16)
                    handler = lines[k + 2][0] if k + 2 < len(lines) else None
                    break
            if hi is not None and handler is not None:
                out.append((lo, hi, handler))
            break
    return out


def find_branch_call(lines, target, nm, method):
    """Return (symbol, addr) of the branch handler call at/after target."""
    idx = None
    for n, (a, mnem) in enumerate(lines):
        if a >= target and re.match(r"call\s+", mnem):
            idx = n
            break
    if idx is None:
        return None, None
    for a, mnem in lines[idx:idx + 6]:
        m = re.match(r"call\s+([0-9a-f]+)\s*<([^>]*)>", mnem)
        if m:
            sym = m.group(2)
            if method == "visualize" and "TdrBufUtil13printVariable" in sym:
                continue
            return sym, a
        m = re.match(r"call\s+([0-9a-f]+)$", mnem)
        if m:
            sym, off = resolve_sym(nm, int(m.group(1), 16))
            if method == "visualize" and sym and "TdrBufUtil13printVariable" in sym:
                continue
            return sym, a
    return None, None


def ebx_base(lines):
    """Estimate ebx value for rodata resolution: first add delta after get_pc_thunk."""
    for i, (a, mnem) in enumerate(lines):
        if "call   809a9a8" in mnem:
            for j in range(i + 1, min(i + 3, len(lines))):
                m = re.match(r"add\s+\$0x([0-9a-f]+),%ebx", lines[j][1])
                if m:
                    return lines[j][0] + int(m.group(1), 16)
    return None


def rodata_str(vaddr):
    with open(BIN, "rb") as f:
        mm = mmap.mmap(f.fileno(), 0, access=mmap.ACCESS_READ)
        out = subprocess.run(["readelf", "-S", BIN], capture_output=True, text=True).stdout
        for ln in out.splitlines():
            m = re.match(r"\s*\[\s*\d+\]\s+(\S+)\s+\S+\s+([0-9a-f]+)\s+([0-9a-f]+)\s+([0-9a-f]+)\s+", ln)
            if not m:
                continue
            sec_addr = int(m.group(2), 16)
            sec_off = int(m.group(3), 16)
            sec_size = int(m.group(4), 16)
            if sec_addr <= vaddr < sec_addr + sec_size:
                mm.seek(sec_off + (vaddr - sec_addr))
                data = mm.read(256)
                end = data.find(b"\0")
                if end >= 0:
                    return data[:end].decode("utf-8", "replace")
                return data.decode("utf-8", "replace")
    return None


def find_name_str(lines, target, ebx):
    """Find lea disp(%ebx),%eax before the printVariable call at target."""
    idx = None
    for n, (a, mnem) in enumerate(lines):
        if a >= target and re.match(r"call\s+", mnem):
            idx = n
            break
    if idx is None:
        return None
    found = None
    for a, mnem in reversed(lines[max(0, idx - 25):idx]):
        m = re.match(r"lea\s+(-?0x[0-9a-f]+)\(%ebx\),%eax", mnem)
        if m:
            found = m
            break
    if found:
        disp = int(found.group(1), 16)
        if disp >= 0x80000000:
            disp -= 0x100000000
        return rodata_str(ebx + disp)
    return None


def main():
    symfile = sys.argv[1] if len(sys.argv) > 1 else "/tmp/fm_symbols.txt"
    nm = nm_table()
    with open(symfile) as f:
        syms = [ln.split() for ln in f.read().splitlines()]
    unions = {}
    for row in syms:
        addr = int(row[0], 16)
        name = " ".join(row[3:])
        m = re.match(r"sec_proto::(\w+)::(construct|pack|unpack|visualize)\(long long", name)
        if m:
            unions.setdefault(m.group(1), {})[m.group(2)] = addr
    for cls, methods in sorted(unions.items()):
        print("### %s" % cls)
        for meth, start in sorted(methods.items()):
            end = min((a for a in methods.values() if a > start), default=start + 0x3000)
            lines = disasm(start, end)
            ebx = ebx_base(lines)
            cases = parse_switch(lines)
            ranges = parse_ranges(lines) if meth == "construct" else []
            print("  %-10s (%d cases, %d ranges)" % (meth, len(cases), len(ranges)))
            for cv in sorted(cases):
                tgt = cases[cv]
                sym, calladdr = find_branch_call(lines, tgt, nm, meth)
                name_str = find_name_str(lines, tgt, ebx) if meth == "visualize" else None
                extra = " name=%r" % name_str if name_str else ""
                print("    case 0x%x -> %s%s" % (cv, sym, extra))
            for lo, hi, tgt in ranges:
                sym, calladdr = find_branch_call(lines, tgt, nm, meth)
                print("    range 0x%x..0x%x -> %s" % (lo, hi, sym))
            if meth == "visualize":
                # list all name strings in order
                for a, mnem in lines:
                    m = re.match(r"lea\s+(-?0x[0-9a-f]+)\(%ebx\),%eax", mnem)
                    if m and ebx:
                        disp = int(m.group(1), 16)
                        if disp >= 0x80000000:
                            disp -= 0x100000000
                        s = rodata_str(ebx + disp)
                        if s and s.startswith("[") and s.endswith("]"):
                            print("      vizname %s @%x" % (repr(s), a))


if __name__ == "__main__":
    main()
