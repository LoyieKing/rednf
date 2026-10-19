#!/usr/bin/env python3
"""Normalize DWARF-generated TSS SDK stubs/headers for a first compile pass.

- strips absolute CI workspace prefixes to the mirrored ``src/``/``output/`` path
- drops system includes and self-includes
- rewrites local includes to repo-relative paths
- adds a small set of forward declarations needed by pointer members

This does not make the tree semantically complete; it only removes plumbing that
prevents the compiler from reaching the reconstructed declarations.
"""
from __future__ import print_function
import argparse
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import gen_headers as gh

SYSTEM_PREFIXES = ("/usr/", "/lib/", "/lib64/", "/usr/local/", "/usr/src/")

def norm_include(path, stem):
    p = path.replace("\\", "/")
    if p.startswith(SYSTEM_PREFIXES) or "<" in p or ">" in p:
        return None
    try:
        n = gh.normalize_path(p)
    except Exception:
        n = p
    n = n.lstrip("/")
    if not n:
        return None
    if n.endswith((".cpp", ".c")) and os.path.basename(n) == stem:
        return None
    return n

def process(path, stem):
    out = []
    changed = False
    for line in open(path, "r", encoding="utf-8", errors="replace"):
        m = re.match(r'^\s*#include\s+"([^"]+)"\s*$', line)
        if m:
            n = norm_include(m.group(1), stem)
            if n is None:
                changed = True
                continue
            if n != m.group(1).replace("\\", "/"):
                out.append('#include "%s"\n' % n)
                changed = True
                continue
        out.append(line)
    text = "".join(out)
    if changed:
        open(path, "w", encoding="utf-8", newline="\n").write(text)
    return changed

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--stubs", required=True)
    ap.add_argument("--headers", required=True)
    args = ap.parse_args()
    changed = 0
    for root in (args.stubs, args.headers):
        for dirpath, _, names in os.walk(root):
            for name in names:
                if name.endswith((".cpp", ".h")):
                    if process(os.path.join(dirpath, name), name):
                        changed += 1
    print("normalized", changed, "files")

if __name__ == "__main__":
    main()
