#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Pair TdrBufUtil::printVariable/printArray arguments with their literal strings
in an annotated objdump listing (dis_annotate.py output).

Usage: visfmt.py <annotated.txt> [first-addr] [last-addr]
Prints, per function, one line per emitted print call:
    <func> @<addr> name=<literal> fmt=<literal|?> load=<0xNN/mnemonic|none>
The format/name argument values are resolved through the ebp spill slots the
compiler uses when it reuses the same literal pointer across several calls.
Read-only helper for the reconstructions.
"""
from __future__ import print_function

import re
import sys

FUNC = re.compile(r"^([0-9a-f]{8}) <(.*)>:$")
LIT = re.compile(r"lea\s+[-0-9a-fx]+\(%ebx\)=0x[0-9a-f]+ '(.*)',%e(ax|dx|cx|si|di|bx)")
STORE_SLOT = re.compile(r"mov\s+%e(ax|dx|cx|si|di|bx),(-0x[0-9a-f]+)\(%ebp\)")
LOAD_SLOT = re.compile(r"mov\s+(-0x[0-9a-f]+)\(%ebp\),%e(ax|dx|cx|si|di|bx)")
ARG_STORE = re.compile(r"mov\s+%e(ax|dx|cx|si|di|bx),(0x[0-9a-f]+)\(%esp\)")
FIELD_LOAD = re.compile(r"(movzbl|movzwl|movsbl|movswl|mov)\s+0x([0-9a-f]+)\(%e[a-z]{2}(?:,%e[a-z]{2},1)?\)")
CALL = re.compile(r"call\s+[0-9a-f]+ <(.*)>")
ADDR = re.compile(r"^\s*([0-9a-f]+):")


def main():
    path = sys.argv[1]
    lo = int(sys.argv[2], 16) if len(sys.argv) > 2 else 0
    hi = int(sys.argv[3], 16) if len(sys.argv) > 3 else 0xFFFFFFFF
    func = "?"
    pending = None
    slots = {}
    cur = {}
    last_load = None
    args = {}
    cur_addr = 0
    with open(path) as f:
        for raw in f:
            line = raw.rstrip("\n")
            m = FUNC.match(line.strip())
            if m:
                func = m.group(2)
                pending = None
                slots = {}
                cur = {}
                args = {}
                last_load = None
                continue
            a = ADDR.match(line)
            if not a:
                continue
            cur_addr = int(a.group(1), 16)
            if cur_addr < lo or cur_addr > hi:
                continue
            body = line[a.end():].strip()
            m = LIT.search(line)
            if m:
                pending = m.group(1)
                cur = {}
                continue
            m = STORE_SLOT.search(line)
            if m and pending is not None:
                slots[m.group(2)] = pending
                pending = None
                continue
            m = LOAD_SLOT.search(line)
            if m:
                cur = {m.group(2): slots.get(m.group(1), "?")}
                continue
            m = ARG_STORE.search(line)
            if m and m.group(2) in ("0xc", "0x10"):
                val = cur.get(m.group(1))
                if val is None:
                    val = pending if pending is not None else "?"
                args[int(m.group(2), 16)] = val
                continue
            m = FIELD_LOAD.search(line)
            if m:
                last_load = "%s@0x%s" % (m.group(1), m.group(2))
                continue
            m = CALL.search(line)
            if not m:
                continue
            callee = m.group(1)
            if "printVariable" in callee:
                print("%s @%08x name=%s fmt=%s load=%s" % (
                    func, cur_addr, args.get(12, "?"), args.get(16, "?"),
                    last_load or "none"))
                args = {}
                last_load = None
            elif "printArray" in callee:
                print("%s @%08x ARRAY name=%s load=%s" % (
                    func, cur_addr, args.get(12, "?"), last_load or "none"))
                args = {}
                last_load = None
            elif "textize" in callee:
                print("%s @%08x ELEM fmt=%s load=%s" % (
                    func, cur_addr, pending or "?", last_load or "none"))
                args = {}
                last_load = None
            else:
                args = {}


if __name__ == "__main__":
    main()
