#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Per-CU global/static variable inventory: ORIG vs reconstruction.

Static-initialization blocks construct exactly the namespace-scope objects that
DWARF records as variables with absolute addresses. Comparing those lists per
CU is the evidence for which static objects a reconstructed TU is missing.

Usage: var_diff.py orig_model.json our_model.json diff_report.json out.tsv
"""
from __future__ import print_function

import json
import sys
from collections import defaultdict


def vars_by_cu(model):
    out = defaultdict(list)
    for addr, lst in model["vars"].items():
        for v in lst:
            cu = (v.get("cu") or "").replace("\\", "/").rsplit("/", 1)[-1]
            out[cu].append({"addr": int(addr), "name": v.get("pretty"),
                            "link": v.get("name"), "type": v.get("type"),
                            "size": v.get("size")})
    return out


def main(argv):
    if len(argv) < 4:
        print(__doc__, file=sys.stderr)
        return 2
    orig = json.load(open(argv[1]))
    ours = json.load(open(argv[2]))
    report = json.load(open(argv[3]))
    out_path = argv[4] if len(argv) > 4 else None

    ov = vars_by_cu(orig)
    uv = vars_by_cu(ours)
    # our CU basename -> ORIG CU basename, from the function-symbol mapping
    cu_map = report["types"]["cu_map"]
    reverse = defaultdict(set)
    for ucu, ocu in cu_map.items():
        reverse[ocu].add(ucu.replace("\\", "/").rsplit("/", 1)[-1])

    rows = []
    for ocu in sorted(ov):
        targets = reverse.get(ocu, set())
        olist = ov[ocu]
        ulist = []
        for t in targets:
            ulist.extend(uv.get(t, []))
        unames = {v["name"] for v in ulist if v["name"]}
        ulinks = {v["link"] for v in ulist if v["link"]}
        for v in olist:
            present = (v["name"] in unames) or (v["link"] and v["link"] in ulinks)
            rows.append({"cu": ocu, "our_cu": ",".join(sorted(targets)),
                         "name": v["name"], "link": v["link"],
                         "type": v["type"], "size": v["size"],
                         "orig_addr": v["addr"], "present": present})
    missing = [r for r in rows if not r["present"]]
    print("orig vars=%d covered=%d missing=%d" %
          (len(rows), len(rows) - len(missing), len(missing)))
    per_cu = defaultdict(list)
    for r in missing:
        per_cu[(r["cu"], r["our_cu"])].append(r)
    for (ocu, ucu), items in sorted(per_cu.items(),
                                    key=lambda kv: -len(kv[1])):
        print("%-40s -> %-34s missing %d: %s" %
              (ocu, ucu or "<unmapped>", len(items),
               ", ".join(sorted({i["name"] for i in items}))[:160]))
    if out_path:
        with open(out_path, "w") as f:
            f.write("orig_cu\tour_cu\tname\tlink\ttype\tsize\torig_addr\tpresent\n")
            for r in rows:
                f.write("%s\t%s\t%s\t%s\t%s\t%s\t0x%x\t%d\n" %
                        (r["cu"], r["our_cu"], r["name"] or "", r["link"] or "",
                         r["type"] or "", r["size"], r["orig_addr"],
                         r["present"]))
        print("tsv=" + out_path)
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
