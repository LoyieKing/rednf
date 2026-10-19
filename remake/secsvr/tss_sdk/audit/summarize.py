#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Turn the audit outputs into per-TU work items.

Inputs (audit/report):
  diff.json        type/function/object diff (diff_model.py)
  var_diff.tsv     per-CU static variable inventory (var_diff.py)
  init_orig.json   ORIG .ctors blocks (static_init.py)
  init_ours.json   reconstructed .init_array blocks (static_init.py)
  orig_model.json / our_model.json

Output:
  workitems.json   machine-readable per-TU slices
  SUMMARY.md       human-readable inventory (numbers, categories, method)
"""
from __future__ import print_function

import json
import re
import sys
from collections import Counter, defaultdict

from diff_model import layout_diff
from accept import evaluate_init


def basename(cu):
    return (cu or "").replace("\\", "/").rsplit("/", 1)[-1]


def call_counts(rows):
    out = Counter()
    for r in rows:
        for c in r.get("calls", []):
            out[c] += 1
    return out


def main(argv):
    root = argv[1] if len(argv) > 1 else "audit/report"
    orig = json.load(open("/tmp/orig_model.json"))
    ours = json.load(open("/tmp/our_model.json"))
    diff = json.load(open(root + "/diff.json"))
    init_o = json.load(open(root + "/init_orig.json"))
    init_u = json.load(open(root + "/init_ours.json"))

    otypes, utypes = orig["types"], ours["types"]
    cu_map = diff["types"]["cu_map"]
    abi = diff.get("abi") or {}
    orig_basename_to_ours = defaultdict(set)
    for ucu, ocu in cu_map.items():
        orig_basename_to_ours[ocu].add(basename(ucu))
    # Reused sources keep the ORIG basename but may share no function symbol.
    for cu_name in init_u["groups"]:
        orig_basename_to_ours[basename(cu_name)].add(basename(cu_name))

    # ---- per-TU type layout deviations (each reconstructed variant) ------
    type_items = []
    for name, variants in ours.get("variants", {}).items():
        ref = otypes.get(name)
        if ref is None:
            continue
        for v in variants:
            hard, soft = layout_diff(ref, v, otypes, utypes)
            if hard:
                type_items.append({
                    "type": name, "our_cu": basename(v["cu"]),
                    "orig_size": ref["size"], "our_size": v["size"],
                    "orig_members": len(ref["members"]),
                    "our_members": len(v["members"]),
                    "diffs": hard[:40]})
    # types whose richest reconstructed record differs from ORIG
    richest_items = []
    for row in diff["types"]["size_mismatch"]:
        richest_items.append({
            "type": row["name"], "our_cu": basename(row["ours"]["cu"]),
            "orig_size": row["orig"]["size"], "our_size": row["ours"]["size"],
            "kind": "size"})
    for row in diff["types"]["member_mismatch"]:
        richest_items.append({
            "type": row["name"], "our_cu": basename(row["ours"]["cu"]),
            "orig_size": row["orig"]["size"], "our_size": row["ours"]["size"],
            "kind": "members", "first": row["diffs"][0]})

    # ---- static variables per TU ---------------------------------------
    static_items = []
    var_map = defaultdict(lambda: {"missing": [], "present": []})
    with open(root + "/var_diff.tsv") as f:
        head = f.readline()
        for line in f:
            p = line.rstrip("\n").split("\t")
            if len(p) < 8:
                continue
            ocu, ucu, name, link, typ, size, addr, present = p
            key = ucu or ("<unmapped>" if not ocu else "<unmapped>:" + ocu)
            if present == "1":
                var_map[key]["present"].append(name)
            else:
                var_map[key]["missing"].append(name)
    for key, v in var_map.items():
        if v["missing"]:
            static_items.append({"our_cu": key,
                                 "missing": sorted(set(v["missing"])),
                                 "present": sorted(set(v["present"]))})

    # ---- static-init block presence and call-set comparison ------------
    our_inits = defaultdict(list)
    for cu, rows in init_u["groups"].items():
        our_inits[basename(cu)].extend(rows)
    orig_inits = defaultdict(list)
    for cu, rows in init_o["groups"].items():
        orig_inits[basename(cu)].extend(rows)
    _init_hard, _init_soft, init_gate = evaluate_init(
        init_o, init_u, orig, ours, cu_map)
    same_tu = init_gate["same_tu"]

    init_items = []
    for ocu, rows in sorted(orig_inits.items()):
        targets = sorted(orig_basename_to_ours.get(ocu, set()))
        our_rows = [r for t in targets for r in our_inits.get(t, [])]
        oc = call_counts(rows)
        if our_rows:
            uc = call_counts(our_rows)
            init_items.append({
                "orig_cu": ocu, "our_cu": ",".join(targets),
                "our_syms": [r["sym"] for r in our_rows],
                "orig_calls": sum(oc.values()),
                "our_calls": sum(uc.values()),
                "call_delta": {k: [oc.get(k, 0), uc.get(k, 0)]
                               for k in set(oc) | set(uc)
                               if oc.get(k, 0) != uc.get(k, 0)},
                "orig_strings": sorted({x["str"] for rr in rows
                                        for x in rr.get("refs", [])
                                        if x.get("str")})[:40],
                "orig_target_size": [rr.get("target_size") for rr in rows]})
        else:
            init_items.append({
                "orig_cu": ocu, "our_cu": ",".join(targets) or "<unmapped>",
                "status": "missing_init_block",
                "orig_calls": sum(oc.values()),
                "orig_strings": sorted({x["str"] for rr in rows
                                        for x in rr.get("refs", [])
                                        if x.get("str")})[:40],
                "orig_target_size": [rr.get("target_size") for rr in rows]})

    # ---- functions -----------------------------------------------------
    missing_funcs = defaultdict(list)
    for row in diff["funcs"]["missing_in_ours"]:
        missing_funcs[row["cu"]].append(row["pretty"])

    work = {
        "type_layout": type_items,
        "richest_type": richest_items,
        "odr_conflicts": diff["types"]["our_internal_conflicts"],
        "static_vars": static_items,
        "static_init": init_items,
        "funcs_missing": {k: v for k, v in sorted(missing_funcs.items())},
        "objects": diff["objects"],
        "missing_types": diff["types"]["missing_in_ours"],
        "cu_map": cu_map,
        "abi": abi,
    }
    with open(root + "/workitems.json", "w") as f:
        json.dump(work, f, indent=1, sort_keys=True)

    # ---- summary -------------------------------------------------------
    lines = []
    add = lines.append
    add("# TSS SDK layout / static-initialization audit\n")
    add("Generated by `audit/summarize.py`. Model input: ORIG "
        "`reference/libtss_sdk.so.1.0.394.orig` vs `build-server-dbg/` "
        "(227 TUs, all built with `-g`).\n")
    add("## Method\n")
    add("- `audit/dwarf_model.py` extracts every aggregate layout "
        "(size, flattened members with offsets/sizes/layout keys, bases, "
        "virtual functions), every function signature (mangled name, return, "
        "non-artificial parameters, size, owning CU), every absolute-address "
        "variable and every ELF object/function symbol. It also keeps "
        "`abi_types` (per-CU `stat`/`stat64` layouts) because the richest-type "
        "record would otherwise drop a legitimate 88-byte variant.\n")
    add("- `audit/diff_model.py` compares ORIG and reconstruction with the same "
        "code path. Layout comparison is name-independent: anonymous "
        "aggregates are flattened, scalar signedness/char/bool/enum of one "
        "width is canonicalised, and a recursive leaf signature decides "
        "whether a nesting/naming difference is a real byte-layout change. "
        "`compare_abi()` checks each ORIG CU's `stat`/`stat64` size and member "
        "offsets against its mapped reconstructed CU.\n")
    add("- `audit/static_init.py` follows ORIG `.ctors` thunks into each TU's "
        "`__static_initialization_and_destruction_0` and records the ordered "
        "constructor calls, `__cxa_atexit` registrations, data/string "
        "references (resolved through DWARF variables and ELF symbols) and "
        "stack immediates. The same decoder reads the reconstructed "
        "`.init_array`.\n")
    add("- `audit/var_diff.py` compares the per-CU set of namespace-scope "
        "objects with absolute addresses; these are exactly the objects a "
        "static-initialization block constructs.\n")
    add("")
    add("## Numbers\n")
    add("| item | count |\n|---|---:|")
    add("| types compared | %d |" % diff["types"]["compared"])
    add("| type size mismatches (richest record) | %d |"
        % len(diff["types"]["size_mismatch"]))
    add("| type member mismatches (richest record) | %d |"
        % len(diff["types"]["member_mismatch"]))
    add("| per-TU type layout deviations | %d |" % len(type_items))
    add("| ODR conflicts inside the reconstruction | %d |"
        % len(diff["types"]["our_internal_conflicts"]))
    add("| ORIG types absent from reconstruction (mapped CUs) | %d |"
        % len(diff["types"]["missing_in_ours"]))
    add("| shared functions | %d |" % diff["funcs"]["shared"])
    add("| function signature mismatches | %d |"
        % len(diff["funcs"]["signature_mismatch"]))
    add("| ORIG functions absent from reconstruction (mapped CUs) | %d |"
        % len(diff["funcs"]["missing_in_ours"]))
    add("| ELF object size mismatches | %d |"
        % len(diff["objects"]["size_mismatch"]))
    add("| per-CU stat/stat64 ABI layouts | checked %d; mismatches %d; "
        "unmapped %d |" %
        (abi.get("checked", 0), len(abi.get("mismatches", [])),
         len(abi.get("unmapped", []))))
    add("| ORIG static-init blocks | %d |" % init_o["count"])
    add("| reconstructed static-init blocks | %d |" % init_u["count"])
    add("| ORIG static-init blocks with primary reconstructed block | %d/%d |"
        % (same_tu, init_o["count"]))
    add("| ORIG static-init blocks without primary reconstructed block | %d |"
        % init_gate["no_same_tu"])
    add("| missing per-TU static variables | %d |"
        % sum(len(x["missing"]) for x in static_items))
    add("")
    add("## Static-initialization finding\n")
    add("Every ORIG TSS SDK TU carries a block that constructs "
        "`COMM_STAT_ITEM_WITH_NAME[16]` (`ZEN_STATUS_ITEM_WITHNAME`, ids "
        "0x238d..0x239c), `std::ios_base::Init __ioinit`, and its own "
        "`Singleton<T>::ObjectCreator` / module globals. The reconstructed "
        "block must carry one independent status array and `__ioinit` per ORIG "
        "CU; a combined reconstruction file that represents several ORIG CUs "
        "must not reuse one array for all of them. `accept.py` maps each ORIG "
        "CU to its primary reconstructed block and checks the constructor / "
        "dtor / atexit multiset; split shared singletons are evidence, not a "
        "skip. Current report: %d ORIG init blocks, %d reconstructed blocks, "
        "%d/%d ORIG blocks with a primary reconstructed block. Hard gaps are "
        "listed by `python3 audit/accept.py` under `init`; the summary here "
        "uses that gate's primary mapping and does not replace it.\n" %
        (init_o["count"], init_u["count"], same_tu, init_o["count"]))
    add("## Work-item files\n")
    add("- `workitems.json` — per-TU type deviations, ODR conflicts, missing "
        "static variables, static-init blocks, per-CU stat/stat64 ABI and "
        "missing functions.\n")
    add("- `diff.tsv` — flat list (types, functions, objects).\n")
    add("- `var_diff.tsv` — per-CU static variable inventory.\n")
    add("- `init_orig.json` / `init_ours.json` — decoded init blocks.\n")
    add("")
    add("## Auditor self-test (negative control)\n")
    add("A copy of `dpinterface.h` with one extra `int extra_probe_` member was "
        "compiled and audited: the extractor reports `DPInterface` size 16 vs "
        "ORIG 12, so the size/member comparison is not vacuous. The probe "
        "object's other type counts are unchanged.\n")
    add("")
    add("## Documented compiler boundaries\n")
    add("- `std::*` / `__gnu_cxx::*`: GCC 4.1.2 libstdc++ vs GCC 13 "
        "libstdc++ layouts (e.g. `std::ofstream` inside `ZEN_Trace_LogMsg`, "
        "`std::map`/`_Rb_tree` nesting). Reported separately, not counted as "
        "reconstruction deviations. The ORIG 364-byte `ZEN_Trace_LogMsg` "
        "definition still exists once in the c5-built `zen_trace_log_msg.cpp` "
        "reuse TU; host TUs use the single declaration header.\n")
    add("- `stat` / `stat64`: per-CU ABI category, not an excluded "
        "boundary. ORIG legitimately contains both an 88-byte and a 96-byte "
        "`stat`, selected per TU by feature macros; both are legal ORIG "
        "variants. `diff_model.compare_abi()` maps each ORIG CU to its "
        "reconstructed CU (current `cu_map` plus same basename) and compares "
        "byte size and member offsets. Current: checked %d, mismatches %d, "
        "unmapped %d. A reconstructed CU that picks the wrong ORIG variant is "
        "a hard `abi` FAIL, not a harmless compiler boundary.\n" %
        (abi.get("checked", 0), len(abi.get("mismatches", [])),
         len(abi.get("unmapped", []))))
    add("- `CFrog`, `ipc_perm`, `re_pattern_buffer`, `des_ks_struct`: ORIG "
        "has genuinely different definitions in different CUs/headers; these "
        "remain outside the layout gate. They are not used to excuse `stat` "
        "or any per-CU ABI mismatch.\n")
    with open(root + "/SUMMARY.md", "w") as f:
        f.write("\n".join(lines) + "\n")

    print("type_items=%d richest=%d odr=%d static_vars=%d init_items=%d "
          "missing_funcs=%d" %
          (len(type_items), len(richest_items),
           len(diff["types"]["our_internal_conflicts"]), len(static_items),
           len(init_items), sum(len(v) for v in missing_funcs.values())))
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
