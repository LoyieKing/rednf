#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Extract full class/struct/union layouts from a binary's DWARF for one TU.

Handles GCC 4.1 style: class declarations in namespace tree + "definition"
DIEs (possibly anonymous, or with DW_AT_specification) containing members and
subprograms. Anonymous structs get names from referencing typedefs.

Output JSON:
  { "types": { name: {name, ns, tag, size, members, subprograms, anon} },
    "anon":  { offset: {...same...}},
    "typedef_to_anon": {typedef_name: anon_key} }
"""
from __future__ import print_function

import json
import sys
from elftools.elf.elffile import ELFFile


def _str(die, name):
    a = die.attributes.get(name)
    if a is None:
        return None
    try:
        return a.value.decode("utf-8", "replace")
    except Exception:
        return None


def _int(die, name):
    a = die.attributes.get(name)
    if a is None:
        return None
    v = a.value
    if isinstance(v, (bytes, bytearray, list)):
        return None
    return v


def _off(die):
    a = die.attributes.get("DW_AT_data_member_location")
    if a is None:
        return None
    v = a.value
    if isinstance(v, (bytes, bytearray, list)):
        # DW_OP_plus_uconst: [35, uleb128...]
        if len(v) >= 2 and v[0] == 35:
            off = 0
            shift = 0
            for b in v[1:]:
                off |= (b & 0x7f) << shift
                if not (b & 0x80):
                    return off
                shift += 7
        return None
    return v


def resolve_ref(die, name, dwarf):
    a = die.attributes.get(name)
    if a is None:
        return None
    try:
        if a.form == "DW_FORM_ref_addr":
            return dwarf.get_DIE_from_refaddr(a.value)
        cu_base = die.cu.cu_offset
        return dwarf.get_DIE_from_refaddr(cu_base + a.value)
    except Exception:
        return None


def type_desc(die, dwarf, depth=0):
    """Return dict describing a type DIE for member generation."""
    if die is None or depth > 12:
        return {"name": "?", "kind": "?", "size": None, "dims": []}
    tag = die.tag
    if tag == "DW_TAG_base_type":
        return {"name": _str(die, "DW_AT_name") or "?", "kind": "base",
                "size": _int(die, "DW_AT_byte_size"), "dims": []}
    if tag == "DW_TAG_typedef":
        n = _str(die, "DW_AT_name")
        t = resolve_ref(die, "DW_AT_type", dwarf)
        inner = type_desc(t, dwarf, depth + 1)
        if n:
            return {"name": n, "kind": "typedef", "size": inner["size"],
                    "dims": inner["dims"], "target": inner["name"]}
        return inner
    if tag == "DW_TAG_pointer_type":
        t = resolve_ref(die, "DW_AT_type", dwarf)
        inner = type_desc(t, dwarf, depth + 1)
        return {"name": inner["name"] + "*", "kind": "ptr", "size": 4, "dims": []}
    if tag == "DW_TAG_reference_type":
        t = resolve_ref(die, "DW_AT_type", dwarf)
        inner = type_desc(t, dwarf, depth + 1)
        return {"name": inner["name"] + "&", "kind": "ref", "size": 4, "dims": []}
    if tag == "DW_TAG_const_type":
        t = resolve_ref(die, "DW_AT_type", dwarf)
        inner = type_desc(t, dwarf, depth + 1)
        inner["name"] = "const " + inner["name"]
        return inner
    if tag == "DW_TAG_volatile_type":
        t = resolve_ref(die, "DW_AT_type", dwarf)
        return type_desc(t, dwarf, depth + 1)
    if tag == "DW_TAG_array_type":
        t = resolve_ref(die, "DW_AT_type", dwarf)
        inner = type_desc(t, dwarf, depth + 1)
        dims = []
        for c in die.iter_children():
            if c.tag == "DW_TAG_subrange_type":
                cnt = _int(c, "DW_AT_count")
                up = _int(c, "DW_AT_upper_bound")
                if cnt is not None:
                    dims.append(cnt)
                elif up is not None:
                    dims.append(up + 1)
                else:
                    dims.append(None)
        return {"name": inner["name"], "kind": "array", "size": inner["size"],
                "dims": dims + inner["dims"]}
    if tag in ("DW_TAG_structure_type", "DW_TAG_class_type", "DW_TAG_union_type"):
        n = _str(die, "DW_AT_name")
        spec = resolve_ref(die, "DW_AT_specification", dwarf)
        if not n and spec is not None:
            n = _str(spec, "DW_AT_name")
        if not n:
            return {"name": "<anon@%x>" % die.offset, "kind": "anon",
                    "size": _int(die, "DW_AT_byte_size"), "dims": []}
        return {"name": n, "kind": tag.replace("DW_TAG_", ""),
                "size": _int(die, "DW_AT_byte_size"), "dims": []}
    if tag == "DW_TAG_enumeration_type":
        return {"name": _str(die, "DW_AT_name") or "<enum>", "kind": "enum",
                "size": _int(die, "DW_AT_byte_size"), "dims": []}
    return {"name": tag.replace("DW_TAG_", ""), "kind": "?", "size": None, "dims": []}


def main():
    binary = sys.argv[1]
    target = sys.argv[2]
    outfile = sys.argv[3] if len(sys.argv) > 3 else "/tmp/dwarf.json"
    with open(binary, "rb") as f:
        elf = ELFFile(f)
        dwarf = elf.get_dwarf_info()
        cus = []
        for cu in dwarf.iter_CUs():
            top = cu.get_top_DIE()
            cu_file = top.attributes.get("DW_AT_name")
            cu_file = cu_file.value.decode("utf-8", "replace") if cu_file else ""
            if target in cu_file:
                cus.append((cu, cu_file))
    print("CUs:", len(cus), file=sys.stderr)

    ns_at = {}          # die offset -> namespace list
    decl_name = {}      # die offset -> declared name
    typedefs = {}       # typedef name -> [(die offset, ns)]
    typedef_target = {}  # target die offset -> [typedef names]

    for cu, cu_file in cus:
        def walk(die, ns):
            tag = die.tag
            if tag == "DW_TAG_namespace":
                n = _str(die, "DW_AT_name") or ""
                ns = ns + [n]
            ns_at[die.offset] = list(ns)
            nm = _str(die, "DW_AT_name")
            if tag in ("DW_TAG_structure_type", "DW_TAG_class_type", "DW_TAG_union_type"):
                if nm:
                    decl_name[die.offset] = nm
            if tag == "DW_TAG_typedef" and nm:
                typedefs.setdefault(nm, []).append((die.offset, list(ns)))
                t = resolve_ref(die, "DW_AT_type", dwarf)
                if t is not None:
                    typedef_target.setdefault(t.offset, []).append(nm)
            for c in die.iter_children():
                walk(c, ns)
        walk(cu.get_top_DIE(), [])

    types = {}
    anon = {}
    used_keys = set()

    def canon(die):
        """Return (name, ns, anon_flag) for a type DIE."""
        nm = _str(die, "DW_AT_name")
        ns = ns_at.get(die.offset, [])
        if nm:
            return nm, ns, False
        spec = resolve_ref(die, "DW_AT_specification", dwarf)
        if spec is not None:
            snm = _str(spec, "DW_AT_name")
            if snm:
                return snm, ns_at.get(spec.offset, []), False
        tns = typedef_target.get(die.offset)
        if tns:
            tn = tns[0]
            # namespace of the typedef occurrence (first one)
            tn_ns = typedefs.get(tn, [None])[0][1] if typedefs.get(tn) else []
            # GCC linkage uses the typedef name at namespace scope; for our
            # purposes map to the typedef name with the namespace of the first
            # enclosing scope that is a namespace chain, falling back to ns.
            return tn, tn_ns, True
        return "<anon@%x>" % die.offset, ns, True

    for cu, cu_file in cus:
        def collect(die):
            tag = die.tag
            if tag in ("DW_TAG_structure_type", "DW_TAG_class_type", "DW_TAG_union_type"):
                bs = _int(die, "DW_AT_byte_size")
                has_def = any(c.tag in ("DW_TAG_member", "DW_TAG_subprogram")
                              for c in die.iter_children())
                if bs is not None and has_def:
                    name, ns, anonflag = canon(die)
                    key = "::".join(ns + [name]) or "<anon@%x>" % die.offset
                    if key in used_keys:
                        return
                    used_keys.add(key)
                    rec = {
                        "name": name,
                        "ns": ns,
                        "tag": tag.replace("DW_TAG_", ""),
                        "size": bs,
                        "anon": anonflag,
                        "members": [],
                        "subprograms": [],
                    }
                    for c in die.iter_children():
                        if c.tag == "DW_TAG_member":
                            t = resolve_ref(c, "DW_AT_type", dwarf)
                            td = type_desc(t, dwarf)
                            rec["members"].append({
                                "name": _str(c, "DW_AT_name"),
                                "off": _off(c),
                                "type": td["name"],
                                "kind": td["kind"],
                                "tsize": td["size"],
                                "dims": td["dims"],
                                "line": _int(c, "DW_AT_decl_line"),
                            })
                        elif c.tag == "DW_TAG_subprogram":
                            fname = _str(c, "DW_AT_name")
                            if fname:
                                rec["subprograms"].append({
                                    "name": fname,
                                    "line": _int(c, "DW_AT_decl_line"),
                                })
                    if anonflag:
                        anon["<anon@%x>" % die.offset] = rec
                    else:
                        types[key] = rec
            for c in die.iter_children():
                collect(c)

        collect(cu.get_top_DIE())

    out = {"types": types, "anon": anon, "typedefs": typedefs}
    with open(outfile, "w") as f:
        json.dump(out, f, indent=1)
    print("wrote", outfile, "named:", len(types), "anon:", len(anon), file=sys.stderr)


if __name__ == "__main__":
    main()
