#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Classify byte/char/int arrays in formmog classes from pack disassembly.

For each class, disassembles pack(TdrWriteBuf&, unsigned int) and inspects
memmove/strlen call sites to decide whether each array member is
fixed (constant size memmove), sized (size from a scalar member), or string.
"""
import json
import re
import subprocess
import sys

BIN = "/mnt/d/Docs/my_sources/dnf_workspace/dnf_installer/build/dnf_data/home/template/neople/secsvr/zergsvr/secagent"
SYMF = "/tmp/fm_symbols.txt"


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


def main():
    dwarf = json.load(open("/tmp/fm_dwarf.json"))
    types = dwarf["types"]
    with open(SYMF) as f:
        syms = [ln.split() for ln in f.read().splitlines()]
    addrs = {}
    for row in syms:
        m = re.match(r"sec_proto::(\w+)::pack\(tsf4g_tdr::TdrWriteBuf&, unsigned int\) const", " ".join(row[3:]))
        if m:
            addrs[m.group(1)] = int(row[0], 16)
    # build sorted address list to bound each function
    all_addrs = sorted(addrs.values())

    for cls, start in sorted(addrs.items()):
        v = types.get("sec_proto::" + cls)
        if not v or "union_type" in v["tag"]:
            continue
        end = min((a for a in all_addrs if a > start), default=start + 0x5000)
        lines = disasm(start, end)
        arrays = [m for m in v["members"] if m["kind"] == "array" and m["type"] in
                  ("uint8_t", "int8_t", "char", "int32_t", "uint16_t")]
        if not arrays:
            continue
        info = {}
        for m in arrays:
            info[m["name"]] = {"off": m["off"], "size": m["dims"][0], "type": m["type"]}
        # scan for strlen calls and memmove calls
        result = {}
        i = 0
        while i < len(lines):
            a, mnem = lines[i]
            m = re.match(r"call\s+80995fc <strlen@plt>", mnem)
            if m:
                # find lea this+off before
                src = None
                for j in range(max(0, i - 12), i):
                    mm = re.match(r"(?:add|lea)\s+\$0x([0-9a-f]+),%eax", lines[j][1])
                    if mm:
                        src = int(mm.group(1), 16)
                if src is not None:
                    for name, mm in info.items():
                        if mm["off"] == src:
                            result[name] = "string"
            m = re.match(r"call\s+80993ec <memmove@plt>", mnem)
            if m:
                # size arg at esp+8: constant or register
                size_reg = None
                const_size = None
                src_off = None
                for j in range(i - 1, max(0, i - 14), -1):
                    mm = re.match(r"movl\s+\$0x([0-9a-f]+),0x8\(%esp\)", lines[j][1])
                    if mm:
                        const_size = int(mm.group(1), 16)
                        break
                    mm = re.match(r"mov\s+%(\w+),0x8\(%esp\)", lines[j][1])
                    if mm:
                        size_reg = mm.group(1)
                        break
                # find src: add/lea this+off
                for j in range(i - 1, max(0, i - 14), -1):
                    mm = re.match(r"add\s+\$0x([0-9a-f]+),%(\w+)", lines[j][1])
                    if mm and (mm.group(2) == "eax" or size_reg is None):
                        pass
                    mm2 = re.match(r"(?:lea|add)\s+\$0x([0-9a-f]+),%(\w+)", lines[j][1])
                    if mm2:
                        src_off = int(mm2.group(1), 16)
                        break
                if const_size is not None:
                    for name, mm in info.items():
                        if mm["off"] == src_off and mm["size"] == const_size:
                            result.setdefault(name, "fixed")
                elif size_reg is not None:
                    # find scalar load for size_reg from this+X
                    szfield = None
                    for j in range(i - 1, max(0, i - 45), -1):
                        for pat in (r"movzbl\s+(?:0x([0-9a-f]+))?\(%(\w+)\),%(\w+)",
                                    r"movzwl\s+(?:0x([0-9a-f]+))?\(%(\w+)\),%(\w+)",
                                    r"mov\s+(?:0x([0-9a-f]+))?\(%(\w+)\),%(\w+)"):
                            mm = re.match(pat, lines[j][1])
                            if mm and mm.group(3) == size_reg:
                                off = int(mm.group(1), 16) if mm.group(1) else 0
                                for sv in (x for x in v["members"] if x["kind"] == "typedef"):
                                    if sv["off"] == off:
                                        szfield = sv["name"]
                                break
                        if szfield:
                            break
                    for name, mm in info.items():
                        if mm["off"] == src_off:
                            result[name] = "sized:" + (szfield or "?")
            i += 1
        print("== %s" % cls)
        for name, mm in info.items():
            print("    %-24s %-8s %s -> %s" % (name, mm["type"], mm["size"], result.get(name, "UNKNOWN")))


if __name__ == "__main__":
    main()
