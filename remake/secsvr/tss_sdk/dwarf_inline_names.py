#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""List the DW_TAG_inlined_subroutine callees of one CU, resolved through
DW_AT_abstract_origin -> DW_AT_specification, with the call site (file, line).

This names the inline functions of headers (e.g. tsf4g_tdr's TdrBuf.h) that the
compiler inlined into the CU's functions - the only place those names survive -
and shows which source line of the CU each call came from.

Usage: dwarf_inline_names.py <elf> <cu-name-substring> [function-name-substr]
"""
from __future__ import print_function

import sys

from elftools.elf.elffile import ELFFile


def s(die, name):
    a = die.attributes.get(name)
    if a is None:
        return None
    v = a.value
    if isinstance(v, (bytes, bytearray)):
        return v.decode("utf-8", "replace")
    return str(v)


def ref(die, attr, ctx):
    """Resolve a ref4/ref_addr attribute to a DIE; ctx = (dwarf, cu_offset)."""
    dwarf, cu_offset = ctx
    a = die.attributes.get(attr)
    if a is None:
        return None
    try:
        if a.form == "DW_FORM_ref_addr":
            return dwarf.get_DIE_from_refaddr(a.value)
        return dwarf.get_DIE_from_refaddr(cu_offset + a.value)
    except Exception:
        return None


def name_of(die):
    for attr in ("DW_AT_MIPS_linkage_name", "DW_AT_linkage_name", "DW_AT_name"):
        nm = s(die, attr)
        if nm:
            return nm
    return None


def callee_name(die, ctx):
    cur = die
    for _ in range(4):
        if cur is None:
            return None
        nm = name_of(cur)
        if nm:
            return nm
        cur = ref(cur, "DW_AT_abstract_origin", ctx) or \
            ref(cur, "DW_AT_specification", ctx)
    return None


def walk(die):
    for c in die.iter_children():
        yield c
        for gc in walk(c):
            yield gc


def main():
    elf_path, target = sys.argv[1], sys.argv[2]
    want = sys.argv[3] if len(sys.argv) > 3 else ""
    with open(elf_path, "rb") as f:
        dwarf = ELFFile(f).get_dwarf_info()
        for cu in dwarf.iter_CUs():
            top = cu.get_top_DIE()
            if target not in (s(top, "DW_AT_name") or ""):
                continue
            ctx = (dwarf, cu.cu_offset)
            lp = dwarf.line_program_for_CU(cu)
            files = [fe.name.decode("utf-8", "replace")
                     for fe in lp["file_entry"]]

            def fname(n):
                return files[n - 1] if n and n <= len(files) else "?"

            for die in walk(top):
                if die.tag != "DW_TAG_subprogram":
                    continue
                fname0 = name_of(die) or ""
                if want and want not in fname0:
                    continue
                rows = []
                for d2 in walk(die):
                    if d2.tag != "DW_TAG_inlined_subroutine":
                        continue
                    cf = d2.attributes.get("DW_AT_call_file")
                    cl = d2.attributes.get("DW_AT_call_line")
                    rows.append((cf.value if cf else None,
                                 cl.value if cl else None,
                                 callee_name(d2, ctx) or "<unresolved>"))
                if not rows:
                    continue
                low = die.attributes.get("DW_AT_low_pc")
                print("== %s @ 0x%x" % (fname0 or "?", low.value if low else 0))
                for df, cl, callee in sorted(set(rows)):
                    print("     %-52s from %s:%s" % (callee, fname(df), cl))
            break
    return


if __name__ == "__main__":
    main()
