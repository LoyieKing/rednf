#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Compare two dwarf_model.py dumps (ORIG vs reconstructed) and report
memory-layout / signature / object-size differences.

Usage: diff_model.py orig.json ours.json out.json [out.tsv]

Comparison rules
----------------
types   size, every flattened member (path, offset, size, layout key, bit
        position) and base-class offsets. Member *spelling* differences are
        reported separately and are not layout failures.
funcs   mangled symbol set; for shared symbols the non-artificial parameter
        and return layout keys; code size reported as informational only.
objects ELF OBJECT sizes (this is how vtable/typeinfo/global sizes compare).

CU mapping: ORIG CU -> reconstructed TU is derived from shared function
symbols, because the reconstructed files are split into *_agent.cpp slices
whose names no longer match the ORIG source file.
"""
from __future__ import print_function

import json
import re
import sys
from collections import Counter, defaultdict

# `stat`/`stat64` vary by feature macro inside ORIG itself; the generic type
# comparison keeps ignoring them, while compare_abi() checks each ORIG CU
# against the reconstructed CU that replaces it.
IGNORE_TYPES = {"stat"}
ABI_TYPES = ("stat", "stat64")


def load(path):
    with open(path) as f:
        return json.load(f)


def member_map(rec):
    return {m["path"]: m for m in rec.get("members", [])}


def _coarse(lk):
    """Canonical memory-layout key.

    - integer signedness/char/bool/enum of one width collapse to `scalarN`
    - anonymous-namespace components (`._NN::`) are CU-local and dropped
    - array element keys are canonicalised by the same rules
    """
    lk = re.sub(r"(?:^|::)\._\d+(?=::)", "", lk)
    lk = re.sub(r"(?<![A-Za-z0-9_])(?:u?int|bool|char|enum)(\d+)(?![A-Za-z0-9_])",
                r"scalar\1", lk)
    return lk


def _canon_refs(lk, types, size):
    """Canonicalise aggregate type references for memory-layout comparison.

    `agg:Name` and ORIG's `anon<size>` describe the same byte region; the named
    type's own layout is audited separately, so at this level only its size is
    relevant. Arrays of aggregates are canonicalised the same way.
    """
    if lk.startswith("agg:"):
        return "anon%d" % (size or 0)
    m = re.match(r"array\(([^;]*);(.*)\)$", lk)
    if m:
        inner = m.group(2)
        if inner.startswith("agg:"):
            sub = types.get(inner[4:])
            if sub is not None:
                inner = "anon%d" % sub["size"]
        return "array(%s;%s)" % (m.group(1), _canon_refs(inner, types, 0))
    return _coarse(lk)


def leaf_sig(rec, types, depth=0, base=0, seen=()):
    """Name/nesting independent byte signature.

    Aggregate members are expanded recursively (with the same ELF's type
    table), so `std::map` vs the flattened `_Rb_tree` it contains, or a named
    nested struct vs the same fields declared anonymously, compare equal.
    """
    out = []
    for m in rec.get("members", []):
        lk = m.get("lkey", "")
        if lk.startswith("agg:") and depth < 6:
            name = lk[4:]
            sub = types.get(name)
            key = name
            if sub is None:
                q = re.sub(r"(?:^|::)\._\d+(?=::)", "", name)
                cands = [k for k in types
                         if re.sub(r"(?:^|::)\._\d+(?=::)", "", k) == q]
                if len(cands) == 1:
                    key = cands[0]
                    sub = types[key]
            if sub is not None and key not in seen:
                out.extend(leaf_sig(sub, types, depth + 1,
                                    base + m.get("offset", 0),
                                    seen + (key,)))
                continue
        out.append((base + m.get("offset", 0), m.get("size") or -1,
                    _canon_refs(lk, types, m.get("size")),
                    m.get("bit_size") or -1, m.get("bit_offset") or -1))
    return tuple(sorted(out))


def byte_spans(rec):
    """Set of byte ranges covered by members (union-aware)."""
    spans = set()
    for m in rec.get("members", []):
        off = m.get("offset") or 0
        size = m.get("size") or 0
        if size > 0:
            spans.add((off, off + size))
    return spans


def layout_diff(a, b, atypes=None, btypes=None):
    """Return (hard, soft) lists of differences between richer layouts."""
    hard = []
    soft = []
    if a["size"] != b["size"]:
        hard.append({"kind": "size", "orig": a["size"], "ours": b["size"]})
    am, bm = member_map(a), member_map(b)
    apos = [(m.get("offset"), m.get("size"), m.get("lkey"),
             m.get("bit_size"), m.get("bit_offset"))
            for m in a.get("members", [])]
    bpos = [(m.get("offset"), m.get("size"), m.get("lkey"),
             m.get("bit_size"), m.get("bit_offset"))
            for m in b.get("members", [])]
    positional_equal = apos == bpos
    member_hard = []
    for path in sorted(set(am) | set(bm)):
        x, y = am.get(path), bm.get(path)
        if x is None:
            member_hard.append({"kind": "missing_member", "path": path,
                                "orig": None, "ours": _brief(y)})
            continue
        if y is None:
            member_hard.append({"kind": "extra_member", "path": path,
                                "orig": _brief(x), "ours": None})
            continue
        for key in ("offset", "size"):
            if x.get(key) != y.get(key):
                member_hard.append({"kind": "member_" + key, "path": path,
                                    "orig": x.get(key), "ours": y.get(key)})
        if x.get("bit_size") != y.get("bit_size") or \
                x.get("bit_offset") != y.get("bit_offset"):
            member_hard.append({"kind": "member_bitfield", "path": path,
                                "orig": (x.get("bit_size"),
                                         x.get("bit_offset")),
                                "ours": (y.get("bit_size"),
                                         y.get("bit_offset"))})
        if x.get("lkey") != y.get("lkey"):
            member_hard.append({"kind": "member_layout_key", "path": path,
                                "orig": x.get("lkey"), "ours": y.get("lkey")})
        elif x.get("skey") != y.get("skey"):
            soft.append({"kind": "spelling", "path": path,
                         "orig": x.get("skey"), "ours": y.get("skey")})
    if positional_equal:
        # Same offsets/sizes/layout keys in the same order: only member names
        # differ. Keep it as evidence, but not as a layout failure.
        soft.extend(dict(d, kind="member_name." + d["kind"])
                    for d in member_hard)
    elif atypes is not None and btypes is not None and \
            leaf_sig(a, atypes) == leaf_sig(b, btypes):
        # Different nesting/naming only (named struct vs anonymous expansion,
        # std::map vs its _Rb_tree, ...): the byte layout is identical.
        soft.extend(dict(d, kind="nested_layout." + d["kind"])
                    for d in member_hard)
    elif byte_spans(a) == byte_spans(b):
        # Same byte ranges covered (anonymous-union/nesting shape differences
        # only); member names/types at those offsets are compared elsewhere.
        soft.extend(dict(d, kind="byte_equivalent." + d["kind"])
                    for d in member_hard)
    else:
        hard.extend(member_hard)
    ab = [(b.get("offset"), b.get("lkey"), b.get("virtual")) for b in
          a.get("bases", [])]
    bb = [(b.get("offset"), b.get("lkey"), b.get("virtual")) for b in
          b.get("bases", [])]
    if ab != bb:
        hard.append({"kind": "bases", "orig": ab, "ours": bb})
    if len(a.get("vfuncs", [])) != len(b.get("vfuncs", [])):
        hard.append({"kind": "vfunc_count", "orig": len(a.get("vfuncs", [])),
                     "ours": len(b.get("vfuncs", []))})
    if a.get("tag") != b.get("tag"):
        soft.append({"kind": "tag", "orig": a.get("tag"), "ours": b.get("tag")})
    return hard, soft


def _brief(m):
    if m is None:
        return None
    return {"path": m.get("path"), "offset": m.get("offset"),
            "size": m.get("size"), "lkey": m.get("lkey")}


def _brief_type(r):
    return {"size": r.get("size"), "cu": r.get("cu", ""),
            "members": len(r.get("members", [])),
            "bases": len(r.get("bases", [])),
            "vfuncs": len(r.get("vfuncs", []))}


def _cu_base(cu):
    return (cu or "").replace("\\", "/").rsplit("/", 1)[-1]


def _positions(rec):
    return [(m.get("offset"), m.get("size"), m.get("lkey"),
             m.get("bit_size"), m.get("bit_offset"))
            for m in rec.get("members", [])]


def abi_layout_diff(orig_rec, our_rec):
    """Strict size/member-offset diff for one ABI-sensitive type layout."""
    diffs = []
    if orig_rec.get("size") != our_rec.get("size"):
        diffs.append({"kind": "size", "orig": orig_rec.get("size"),
                      "ours": our_rec.get("size")})
    op, up = _positions(orig_rec), _positions(our_rec)
    if op != up:
        first = None
        for index in range(max(len(op), len(up))):
            left = op[index] if index < len(op) else None
            right = up[index] if index < len(up) else None
            if left != right:
                first = index
                break
        diffs.append({"kind": "members", "first_index": first,
                      "orig_count": len(op), "ours_count": len(up),
                      "orig": op, "ours": up})
    return diffs


def compare_abi(orig, ours, cu_map):
    """Compare per-CU stat/stat64 layouts using the existing CU mapping.

    The ORIG library legitimately contains both an 88-byte and a 96-byte
    `stat`; the question is whether each reconstructed CU that replaces an
    ORIG CU uses the same layout as that CU.  `types`/`variants` cannot answer
    this because they keep only the richest/first representative definition.
    """
    report = {
        "available": ("abi_types" in orig and "abi_types" in ours),
        "types": list(ABI_TYPES),
        "checked": 0,
        "mismatches": [],
        "unmapped": [],
    }
    if not report["available"]:
        return report
    orig_abi = orig.get("abi_types") or {}
    our_abi = ours.get("abi_types") or {}
    reverse = defaultdict(set)
    for ucu, ocu in cu_map.items():
        reverse[_cu_base(ocu)].add(_cu_base(ucu))
    seen = set()
    for name in ABI_TYPES:
        our_by_cu = {_cu_base(rec.get("cu")): rec
                     for rec in (our_abi.get(name) or [])}
        for orec in (orig_abi.get(name) or []):
            ocu = _cu_base(orec.get("cu"))
            candidates = set(reverse.get(ocu, ())) | {ocu}
            any_layout = False
            for ucu in sorted(candidates):
                urec = our_by_cu.get(ucu)
                if urec is None:
                    continue
                any_layout = True
                report["checked"] += 1
                diffs = abi_layout_diff(orec, urec)
                if not diffs:
                    continue
                key = (name, ocu, ucu, orec.get("size"),
                       urec.get("size"))
                if key in seen:
                    continue
                seen.add(key)
                report["mismatches"].append({
                    "name": name, "orig_cu": ocu, "our_cu": ucu,
                    "orig_size": orec.get("size"),
                    "ours_size": urec.get("size"),
                    "diffs": diffs,
                })
            if not any_layout:
                report["unmapped"].append({
                    "name": name, "orig_cu": ocu,
                    "candidates": sorted(candidates),
                })
    return report


def main(argv):
    if len(argv) < 4:
        print(__doc__, file=sys.stderr)
        return 2
    orig = load(argv[1])
    ours = load(argv[2])
    out_json = argv[3]
    out_tsv = argv[4] if len(argv) > 4 else None

    report = {"types": {}, "funcs": {}, "objects": {}}

    # -- CU mapping (shared functions) -----------------------------------
    shared_funcs = set(orig["funcs"]) & set(ours["funcs"])
    votes = defaultdict(Counter)
    for mangled in shared_funcs:
        ocu = orig["funcs"][mangled].get("cu", "")
        ucu = ours["funcs"][mangled].get("cu", "")
        votes[ucu][ocu.rsplit("/", 1)[-1]] += 1
    cu_map = {}
    for ucu, counter in votes.items():
        if counter:
            cu_map[ucu] = counter.most_common(1)[0][0]
    mapped_orig_cus = set(cu_map.values())

    # -- types -----------------------------------------------------------
    otypes, utypes = orig["types"], ours["types"]
    size_mismatch = []
    member_mismatch = []
    spelling = []
    compared = 0
    for name in sorted(set(otypes) & set(utypes)):
        if name in IGNORE_TYPES:
            continue
        a, b = otypes[name], utypes[name]
        hard, soft = layout_diff(a, b, otypes, utypes)
        compared += 1
        if any(d["kind"] == "size" for d in hard):
            size_mismatch.append({"name": name, "orig": _brief_type(a),
                                  "ours": _brief_type(b),
                                  "other": [d for d in hard
                                            if d["kind"] != "size"]})
        elif hard:
            member_mismatch.append({"name": name, "orig": _brief_type(a),
                                    "ours": _brief_type(b), "diffs": hard})
        if soft:
            spelling.append({"name": name, "diffs": soft})

    missing_types = []
    for name, rec in sorted(otypes.items()):
        if name in utypes or name in IGNORE_TYPES:
            continue
        cu = rec.get("cu", "").rsplit("/", 1)[-1]
        if cu in mapped_orig_cus:
            missing_types.append({"name": name, "size": rec["size"],
                                  "cu": rec.get("cu")})
    ours_only = []
    for name, rec in sorted(utypes.items()):
        if name in otypes:
            continue
        ours_only.append({"name": name, "size": rec["size"],
                          "cu": rec.get("cu", "").rsplit("/", 1)[-1]})

    # internal ODR conflicts in the reconstructed library
    our_conflicts = []
    for name, variants in ours.get("variants", {}).items():
        if name in IGNORE_TYPES:
            continue
        if len(variants) < 2:
            continue
        ref = otypes.get(name)
        rows = []
        for v in variants:
            bad = None
            if ref is not None:
                hard, _ = layout_diff(ref, v, otypes, utypes)
                bad = bool(hard)
            rows.append({"size": v["size"],
                         "cu": v["cu"].rsplit("/", 1)[-1],
                         "members": len(v["members"]),
                         "differs_from_orig": bad})
        # Raw DWARF signatures also differ when one CU emits a named nested
        # aggregate and another flattens an anonymous definition (GCC 4.1 vs
        # GCC 13).  That is not a layout conflict: compare the variants with
        # the same leaf/byte-span canonicalisation used against ORIG.
        layout_conflict = False
        for i in range(len(variants)):
            for j in range(i + 1, len(variants)):
                a, b = variants[i], variants[j]
                hard, _ = layout_diff(a, b, utypes, utypes)
                if a["size"] != b["size"] or hard:
                    layout_conflict = True
        if layout_conflict:
            our_conflicts.append({"name": name, "variants": rows,
                                  "layout_conflict": layout_conflict})

    report["types"] = {
        "compared": compared,
        "size_mismatch": size_mismatch,
        "member_mismatch": member_mismatch,
        "missing_in_ours": missing_types,
        "ours_only": ours_only,
        "our_internal_conflicts": our_conflicts,
        "spelling_notes": spelling,
        "cu_map": cu_map,
    }

    # -- functions -------------------------------------------------------
    ofuncs, ufuncs = orig["funcs"], ours["funcs"]
    sig_mismatch = []
    sig_spelling = []

    def canon_lkeys(lkeys, types):
        """ABI layout key: by-value aggregates only carry size/class."""
        out = []
        for lk in lkeys or []:
            if lk.startswith("agg:"):
                r = types.get(lk[4:])
                out.append("anon%d" % r["size"] if r else lk)
            else:
                out.append(lk)
        return out

    for mangled in sorted(shared_funcs):
        a, b = ofuncs[mangled], ufuncs[mangled]
        diffs = []
        if canon_lkeys(a.get("param_lkeys"), otypes) != \
                canon_lkeys(b.get("param_lkeys"), utypes) or \
                canon_lkeys([a["ret_lkey"]], otypes) != \
                canon_lkeys([b["ret_lkey"]], utypes):
            diffs.append({"kind": "signature", "orig_params": a["params"],
                          "ours_params": b["params"],
                          "orig_ret": a["ret"], "ours_ret": b["ret"]})
        elif a["params"] != b["params"] or a["ret"] != b["ret"]:
            sig_spelling.append(
                {"mangled": mangled, "pretty": a["pretty"],
                 "orig": a["params"] + [a["ret"]],
                 "ours": b["params"] + [b["ret"]]})
        if diffs:
            sig_mismatch.append({"mangled": mangled, "pretty": a["pretty"],
                                 "diffs": diffs})
    missing_funcs = []
    for mangled, rec in sorted(ofuncs.items()):
        if mangled in ufuncs:
            continue
        cu = rec.get("cu", "").rsplit("/", 1)[-1]
        if cu in mapped_orig_cus:
            missing_funcs.append({"mangled": mangled, "pretty": rec["pretty"],
                                  "size": rec.get("size"), "cu": cu})
    extra_funcs = [{"mangled": m, "pretty": ufuncs[m]["pretty"],
                    "cu": ufuncs[m].get("cu", "").rsplit("/", 1)[-1]}
                   for m in sorted(set(ufuncs) - set(ofuncs))]
    size_rows = []
    for mangled in sorted(shared_funcs):
        a, b = ofuncs[mangled], ufuncs[mangled]
        if a.get("size") and b.get("size"):
            size_rows.append({"mangled": mangled, "pretty": a["pretty"],
                              "orig": a["size"], "ours": b["size"],
                              "ratio": round(float(b["size"]) / a["size"], 3)})
    size_rows.sort(key=lambda r: -abs(1.0 - r["ratio"]))
    report["funcs"] = {
        "shared": len(shared_funcs),
        "orig_only": len(set(ofuncs) - set(ufuncs)),
        "ours_only": len(set(ufuncs) - set(ofuncs)),
        "signature_mismatch": sig_mismatch,
        "signature_spelling": sig_spelling,
        "missing_in_ours": missing_funcs,
        "extra_in_ours": extra_funcs,
        "size_deltas": size_rows[:200],
    }

    # -- objects ---------------------------------------------------------
    oobj, uobj = orig["objects"], ours["objects"]
    obj_size = []
    for name in sorted(set(oobj) & set(uobj)):
        a, b = oobj[name], uobj[name]
        if a["type"] != b["type"]:
            obj_size.append({"name": name, "kind": "type_change",
                             "orig": a["type"], "ours": b["type"]})
        elif a["type"] == "STT_OBJECT" and a["size"] != b["size"]:
            obj_size.append({"name": name, "kind": "size",
                             "orig": a["size"], "ours": b["size"],
                             "delta": b["size"] - a["size"]})
    obj_missing = [{"name": n, "size": oobj[n]["size"],
                    "type": oobj[n]["type"]}
                   for n in sorted(set(oobj) - set(uobj))]
    report["objects"] = {
        "shared": len(set(oobj) & set(uobj)),
        "size_mismatch": obj_size,
        "missing_in_ours": obj_missing,
    }
    report["abi"] = compare_abi(orig, ours, cu_map)

    with open(out_json, "w") as f:
        json.dump(report, f, indent=1, sort_keys=True)

    # -- console / TSV summary ------------------------------------------
    t, fn, ob = report["types"], report["funcs"], report["objects"]
    print("types compared=%d size_mismatch=%d member_mismatch=%d "
          "missing=%d ours_only=%d internal_conflicts=%d spelling=%d" %
          (t["compared"], len(t["size_mismatch"]), len(t["member_mismatch"]),
           len(t["missing_in_ours"]), len(t["ours_only"]),
           len(t["our_internal_conflicts"]), len(t["spelling_notes"])))
    print("funcs shared=%d orig_only=%d ours_only=%d sig_mismatch=%d "
          "missing_in_mapped_cus=%d" %
          (fn["shared"], fn["orig_only"], fn["ours_only"],
           len(fn["signature_mismatch"]), len(fn["missing_in_ours"])))
    print("objects shared=%d size_mismatch=%d missing=%d" %
          (ob["shared"], len(ob["size_mismatch"]), len(ob["missing_in_ours"])))
    abi = report["abi"]
    print("abi checked=%d mismatch=%d unmapped=%d available=%s" %
          (abi["checked"], len(abi["mismatches"]), len(abi["unmapped"]),
           abi["available"]))

    if out_tsv:
        with open(out_tsv, "w") as f:
            f.write("kind\tname/orig_size\tours\tcu\n")
            for row in t["size_mismatch"]:
                f.write("type_size\t%s\t%s\t%s|%s\n" %
                        (row["name"], row["ours"]["size"],
                         row["orig"]["cu"].rsplit("/", 1)[-1],
                         row["ours"]["cu"].rsplit("/", 1)[-1]))
            for row in t["member_mismatch"]:
                first = row["diffs"][0]
                f.write("type_member\t%s\t%s\t%s\n" %
                        (row["name"], first.get("path"),
                         row["ours"]["cu"].rsplit("/", 1)[-1]))
            for row in t["our_internal_conflicts"]:
                for v in row["variants"]:
                    f.write("odr_conflict\t%s\t%s\t%s\n" %
                            (row["name"], v["size"], v["cu"]))
            for row in fn["signature_mismatch"]:
                f.write("fn_sig\t%s\t%s\t-\n" % (row["mangled"],
                                                  row["pretty"]))
            for row in fn["missing_in_ours"]:
                f.write("fn_missing\t%s\t%s\t%s\n" %
                        (row["mangled"], row["size"], row["cu"]))
            for row in ob["size_mismatch"]:
                f.write("obj_size\t%s\t%s\t-\n" % (row["name"], row["orig"]))
            for row in abi["mismatches"]:
                f.write("abi\t%s\t%s\t%s->%s\n" %
                        (row["name"], row["orig_size"],
                         row["orig_cu"], row["our_cu"]))
            for row in abi["unmapped"]:
                f.write("abi_unmapped\t%s\t-\t%s\n" %
                        (row["name"], row["orig_cu"]))
        print("tsv=" + out_tsv)
    print("json=" + out_json)
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
