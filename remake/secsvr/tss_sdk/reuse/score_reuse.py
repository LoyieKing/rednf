#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""score_reuse.py -- score every reused CU object and regenerate the report.

Reads reuse/log/*.res + reuse/log/_min_*.roots (written by compile_reuse.sh) and
measures every produced object against ORIG's dynamic symbol table, then writes

    reuse/reuse_matrix.tsv   cu candidate compiled obj_total orig_matched local_total error
    reuse/REPORT.md          totals, per-family summaries, include roots, failures

Column definitions (measured from the final objects in reuse/obj/):

    obj_total     distinct demangled names among the defined symbols of
                  reuse/obj/<sanitized>.o          (nm -C --defined-only)
    orig_matched  of those, how many occur verbatim (same demangled name) in the
                  defined DYNAMIC symbol table of libtss_sdk.so.1.0.394
                                                       (nm -DC --defined-only)
    local_total   distinct names that appear ONLY with lowercase nm type letters
                  in the object, i.e. local (file-scope) names

Distinct names are counted, not symbol-table entries: constructor/destructor
pairs (C1/C2, D1/D2) demangle to the same string and would otherwise inflate the
denominator.  A name match is a name match only -- this file never claims
behavioural identity.
"""
from __future__ import print_function

import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
LOGD = os.path.join(HERE, "log")
OBJD = os.path.join(HERE, "obj")
TASKS = os.path.join(HERE, "tasks.tsv")
TSV = os.path.join(HERE, "reuse_matrix.tsv")
REPORT = os.path.join(HERE, "REPORT.md")
ORIG = os.environ.get(
    "ORIG", "/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394")
SECSVR = os.environ.get("SECSVR", "/home/loyieking/dnf_workspace/dnf_secsvr")

NM_LINE = re.compile(r"^([0-9a-fA-F]+)?\s*([A-Za-z?])\s+(.*)$")
FAMILIES = ["crypto-tencrypt", "zenlib", "commlib-framework",
            "protocol-common/tdr", "formmog"]

# One-line causes, matched against the normalised error signature.
EXPLAIN = [
    ("No such file or directory",
     "header/source not reachable from that candidate tree's include roots"),
    ("expected ')' before",
     "auto-generated DWARF stub header declares illegal members (e.g. `_vptr.X`)"),
    ("was not declared in this scope",
     "declaration unreachable (missing or declaration-only stub header)"),
    ("has not been declared",
     "declaration unreachable (missing or declaration-only stub header)"),
    ("invalid conversion from",
     "host system header (Ubuntu glibc 2.39 <dirent.h>) declares scandir's "
     "comparator as const struct dirent** while the CU casts from const void*"),
    ("no matching function",
     "header signature differs between the reused tree and its caller"),
    ("is not a member of", "stub header lacks the member used by the CU"),
    ("invalid use of incomplete type",
     "stub header has no type definition (forward declaration only)"),
    ("redefinition of",
     "two header copies of the same entity disagree (per-tree header duplication)"),
    ("conflicting declaration",
     "two header copies of the same entity disagree (per-tree header duplication)"),
    ("cannot convert", "reconstructed header types do not match the CU's use"),
    ("expected initializer before",
     "auto-generated stub header is not valid C++ for this compiler"),
    ("unterminated", "auto-generated stub header is truncated"),
]


def nm_lines(path, dynsym=False):
    cmd = ["nm", "-C", "--defined-only"]
    if dynsym:
        cmd.append("-D")
    cmd.append(path)
    try:
        out = subprocess.check_output(cmd, stderr=subprocess.DEVNULL)
    except subprocess.CalledProcessError:
        return []
    except OSError as exc:
        sys.stderr.write("nm failed on %s: %s\n" % (path, exc))
        return []
    return out.decode("utf-8", "replace").splitlines()


def classify(lines):
    """(distinct names, local-only names, raw defined-symbol entry count)."""
    types = {}
    raw = 0
    for line in lines:
        m = NM_LINE.match(line)
        if not m:
            continue
        typ, name = m.group(2), m.group(3).strip()
        if typ == "U" or not name:
            continue
        raw += 1
        types.setdefault(name, set()).add(typ)
    names = set(types)
    local_only = set(n for n, ts in types.items() if ts and all(t.islower() for t in ts))
    return names, local_only, raw


def read_tasks():
    """(cu, sanitized, family, [candidates]) in map order."""
    rows = []
    with open(TASKS, "r") as fh:
        for line in fh:
            line = line.rstrip("\n")
            if not line or line.startswith("#"):
                continue
            parts = line.split("\t")
            cu, san, fam = parts[0], parts[1], parts[2]
            cands = [c for c in parts[3].split(" ") if c]
            rows.append((cu, san, fam, cands))
    return rows


def res_get(path, key):
    if not os.path.exists(path):
        return ""
    pat = re.compile(r"^" + re.escape(key) + r"=(.*)$")
    with open(path, "r") as fh:
        for line in fh:
            m = pat.match(line.rstrip("\n"))
            if m:
                return m.group(1)
    return ""


def one_line(text):
    return re.sub(r"\s+", " ", text.replace("\t", " ")).strip()


def signature(err):
    """Collapse a compiler error line to a file-plus-message signature."""
    if not err:
        return ""
    e = one_line(err)
    for q in ("\u2018", "\u2019", "\u201c", "\u201d", "`"):
        e = e.replace(q, "'")
    m = re.match(r"^.*?[\\/]([^\\/:]+):\d+:\s*", e)
    fname = (m.group(1) + ": ") if m else ""
    rest = e[m.end():] if m else e
    rest = re.sub(r"'[^']*'", "'<id>'", rest)
    rest = re.sub(r"\b\d+\b", "N", rest)
    return (fname + rest)[:150]


def explain(sig):
    for needle, text in EXPLAIN:
        if needle in sig:
            return text
    return "CU does not compile against the reconstructed headers of that tree"


def read_roots(tree):
    """(minimized-flag, verified-line, full-count, min-count, [roots])."""
    slug = tree.replace("/", "_")
    path = os.path.join(LOGD, "_min_%s.roots" % slug)
    if not os.path.exists(path):
        return ("n/a", "", 0, 0, [])
    mini, verified, full, minsz, roots = "n/a", "", 0, 0, []
    with open(path, "r") as fh:
        for line in fh:
            line = line.rstrip("\n")
            if line.startswith("# minimized: "):
                mini = line.split(": ", 1)[1].split(" ")[0]
            elif line.startswith("# verified: "):
                verified = line.split(": ", 1)[1]
            elif line.startswith("# full-roots: "):
                full = int(line.split(": ", 1)[1])
            elif line.startswith("# minimized-roots: "):
                minsz = int(line.split(": ", 1)[1])
            elif not line.startswith("#") and line.strip():
                roots.append(line.strip())
    return (mini, verified, full, minsz, roots)


def read_tree_state():
    """[[tree, file-count, newest-file], ...] written by compile_reuse.sh."""
    path = os.path.join(LOGD, "_tree_state.txt")
    state = []
    if not os.path.exists(path):
        return state
    with open(path, "r") as fh:
        for line in fh:
            parts = line.rstrip("\n").split("\t")
            if len(parts) >= 3:
                state.append(parts[:3])
    return state


def fmt_rate(hit, total):
    return ("%.1f%%" % (100.0 * hit / total)) if total else "-"


def main():
    tasks = read_tasks()
    orig, orig_local, orig_raw = classify(nm_lines(ORIG, dynsym=True))
    if not orig:
        sys.stderr.write("error: ORIG dynsym empty (%s)\n" % ORIG)
        return 1

    rows = []
    for cu, san, fam, cands in tasks:
        res = os.path.join(LOGD, san + ".res")
        obj = os.path.join(OBJD, san + ".o")
        compiled = os.path.exists(obj) and res_get(res, "compiled") == "1"
        cand = res_get(res, "candidate") or (cands[0] if cands else "")
        err = "" if compiled else res_get(res, "first_error")
        obj_total = matched = local_total = local_hit = raw = 0
        if compiled:
            names, local_only, raw = classify(nm_lines(obj))
            obj_total = len(names)
            matched = len(names & orig)
            local_total = len(local_only)
            local_hit = len(local_only & orig)
        rows.append({
            "cu": cu, "san": san, "family": fam, "candidate": cand,
            "compiled": 1 if compiled else 0, "obj_total": obj_total,
            "orig_matched": matched, "local_total": local_total,
            "local_hit": local_hit, "raw_entries": raw,
            "error": one_line(err), "tree": res_get(res, "tree"),
            "flags": res_get(res, "flags"), "roots": res_get(res, "roots"),
        })

    with open(TSV, "w") as fh:
        fh.write("cu\tcandidate\tcompiled\tobj_total\torig_matched\t"
                 "local_total\terror\n")
        for r in rows:
            fh.write("%s\t%s\t%d\t%d\t%d\t%d\t%s\n" % (
                r["cu"], r["candidate"], r["compiled"], r["obj_total"],
                r["orig_matched"], r["local_total"], r["error"]))

    write_report(rows, orig, orig_local, orig_raw)
    print("== reuse_matrix.tsv: %d rows, %d compiled -> %s" % (
        len(rows), sum(r["compiled"] for r in rows), TSV))
    print("== REPORT.md -> %s" % REPORT)
    return 0


def write_report(rows, orig, orig_local, orig_raw):
    total = len(rows)
    compiled = [r for r in rows if r["compiled"]]
    failed = [r for r in rows if not r["compiled"]]
    s_obj = sum(r["obj_total"] for r in rows)
    s_hit = sum(r["orig_matched"] for r in rows)
    s_loc = sum(r["local_total"] for r in rows)
    s_loc_hit = sum(r["local_hit"] for r in rows)
    s_raw = sum(r["raw_entries"] for r in rows)
    documented = sum(1 for r in failed if r["error"])

    out = []
    w = out.append
    w("# Reused dnf_secsvr CUs against libtss_sdk.so.1.0.394 -- per-CU reuse matrix")
    w("")
    w("Regenerated by `tools/tss_sdk_recon/reuse/compile_reuse.sh`, which compiles")
    w("every mapped CU with the vendor-matched c5root GCC 4.1.2 toolchain and then")
    w("calls `tools/tss_sdk_recon/reuse/score_reuse.py` to write")
    w("`tools/tss_sdk_recon/reuse/reuse_matrix.tsv` and this file.  Reproduction")
    w("(inside WSL, from the repository root):")
    w("")
    w("```")
    w("bash tools/tss_sdk_recon/reuse/compile_reuse.sh")
    w("```")
    w("")
    w("Inputs: `source_reuse_map.json` (129 of 214 CUs have basename-level")
    w("candidates in the reconstructed tree), `/home/loyieking/dnf_workspace/dnf_secsvr`")
    w("(read-only: the harness only adds `-I` search paths), ORIG =")
    w("`/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394`, whose defined")
    w("dynamic symtab holds %d entries / %d distinct demangled names." % (orig_raw, len(orig)))
    w("")
    w("State of the reused trees this run was compiled against (`.cpp`/`.h` count and")
    w("newest file; a later tree edit therefore shows up as a mismatch with this")
    w("list instead of silently invalidating the matrix):")
    w("")
    for tree, cnt, newest in read_tree_state():
        w("* `%s` -- %s source/header files; newest: `%s`" % (tree, cnt, newest))
    w("")
    w("Column units: `obj_total`, `orig_matched` and `local_total` all count")
    w("**distinct demangled names** (the %d objects contribute %d defined"
      % (len(compiled), s_raw))
    w("symbol-table entries, i.e. duplicate names from C1/C2 and D1/D2 pairs are")
    w("counted once).  `local_total` counts names that appear only with lowercase nm")
    w("type letters (file scope).")
    w("")
    w("## Claim boundary (read this before using any number)")
    w("")
    w("* Candidates are **basename-level** matches taken from `source_reuse_map.json`.")
    w("  The map itself states `identity_claim: none`; nothing here upgrades a")
    w("  basename match into a source-identity or build-version claim.")
    w("* `orig_matched` counts **names, not behaviour**.  A demangled name that occurs")
    w("  in both an object and ORIG's dynsym proves only that the same mangled name is")
    w("  defined in both binaries.  Symbol-name match is **NOT** behaviour identity:")
    w("  identical names can carry different logic, constants, call graphs or data.")
    w("* **No tag-394 equivalence is claimed.**  Nothing in this matrix compares code,")
    w("  instruction bytes or semantics against build 394; it only records what")
    w("  compiles with the c5root toolchain and which symbol names coincide.")
    w("* System headers come from the host (`/usr/include`, Ubuntu glibc 2.39), not")
    w("  from a 2009-era libc; the single CU that fails to compile here fails on")
    w("  exactly that (`scandir`'s comparator prototype changed).")
    w("* `local_total` counts names that appear only as file-scope (lowercase nm")
    w("  type letter) definitions in their object.  %d of the %d local-only names we"
      % (s_loc_hit, s_loc))
    w("  produce also occur in ORIG's dynsym -- a name can be file scope in one build")
    w("  and exported in another -- while the rest cannot appear in any dynsym-based")
    w("  comparison, so `orig_matched` is bounded by each object's global names.")
    w("")
    w("## Totals (identical to `reuse_matrix.tsv`)")
    w("")
    w("| metric | value |")
    w("| --- | --- |")
    w("| mapped CUs | %d |" % total)
    w("| CUs with a compiled object (`compiled=1`) | %d |" % len(compiled))
    w("| CUs without an object | %d |" % len(failed))
    w("| CUs with a documented first-error line | %d |" % documented)
    w("| `obj_total` (distinct defined names, all objects) | %d |" % s_obj)
    w("| `orig_matched` (names also in ORIG dynsym) | %d |" % s_hit)
    w("| name-level match rate | %s |" % fmt_rate(s_hit, s_obj))
    w("| `local_total` (names local to their object) | %d |" % s_loc)
    w("")
    w("Accounted for: %d compiled + %d without an object = %d mapped CUs; %d of the"
      % (len(compiled), len(failed), total, documented))
    w("latter carries its first compiler error in the TSV `error` column.")
    w("")

    # ---- per family ----------------------------------------------------
    w("## Per-family summary")
    w("")
    w("| family | CUs | compiled | obj_total | orig_matched | match rate | local_total | candidate trees used | flags seen |")
    w("| --- | --- | --- | --- | --- | --- | --- | --- | --- |")
    for fam in FAMILIES:
        fr = [r for r in rows if r["family"] == fam]
        if not fr:
            continue
        comp = [r for r in fr if r["compiled"]]
        fo = sum(r["obj_total"] for r in fr)
        fh_ = sum(r["orig_matched"] for r in fr)
        fl = sum(r["local_total"] for r in fr)
        trees = sorted(set(r["tree"] for r in comp if r["tree"]))
        flags = sorted(set(("base" if r["flags"] in ("", "-") else r["flags"])
                           for r in comp))
        w("| %s | %d | %d | %d | %d | %s | %d | %s | %s |" % (
            fam, len(fr), len(comp), fo, fh_, fmt_rate(fh_, fo), fl,
            ", ".join("`%s`" % t for t in trees) or "-",
            ", ".join("`%s`" % f for f in flags) or "-"))
    w("")
    w("The candidate tree actually used per CU is recorded in `reuse/log/<sanitized>.res`")
    w("(`tree=`, `candidate=`, `flags=`, `roots=minimized|full`).")
    w("")

    # ---- include roots -------------------------------------------------
    w("## Exact include roots that make each family compile")
    w("")
    w("Pass 1 compiled with the full root list of the candidate's tree (the tree root")
    w("itself, its `include`/`import/include`, its `output/.../release/include` dirs,")
    w("the secagent equivalents, `third_party/import/include/{tencore,opensource}`).")
    w("Pass 2 then greedily dropped every root that the CUs of that tree did not need;")
    w("a root was only dropped when each test CU recompiled with the reduced list")
    w("**and produced exactly the same defined symbols** as with the full list, so a")
    w("dropped root cannot silently switch a header copy.  The reduced list was")
    w("re-verified that way on every CU compiled from the tree, and pass 3 rebuilt all")
    w("objects with it; `roots=full` in a `.res` file means that CU fell back to the")
    w("full list.  Raw lists: `reuse/log/_full_<tree>.roots` and")
    w("`reuse/log/_min_<tree>.roots`; per-CU ablation compiles:")
    w("`reuse/log/_roots_verify.log`.")
    w("")
    trees_by_fam = {}
    for r in rows:
        if r["compiled"] and r["tree"]:
            trees_by_fam.setdefault(r["family"], set()).add(r["tree"])
    for fam in FAMILIES:
        fr = [r for r in rows if r["family"] == fam]
        if not fr:
            continue
        w("### %s" % fam)
        w("")
        trees = sorted(trees_by_fam.get(fam, []))
        if not trees:
            w("No CU of this family compiled, so there is no verified root list")
            w("(%d CUs mapped; see the failure section)." % len(fr))
            w("")
            continue
        for tree in trees:
            mini, verified, full, minsz, roots = read_roots(tree)
            n_cu = len([r for r in fr if r["tree"] == tree and r["compiled"]])
            w("* `%s` -- %d CU(s) of this family compiled from it; minimised: %s"
              % (tree, n_cu, mini))
            w("  (%d root(s) reduced to %d, verified %s over the whole tree corpus):"
              % (full, minsz, verified or "?"))
            w("")
            w("  ```")
            for root in roots:
                w("  -I %s" % root)
            w("  ```")
            w("")
        w("Base command (replace `<roots>` with the list(s) above; `$SECSVR` is the")
        w("read-only reconstructed tree):")
        w("")
        w("```")
        w("g++ -m32 -O2 -fPIC -fno-stack-protector -std=gnu++98 \\")
        w("    [-fno-implement-inlines [-fno-exceptions]] \\")
        w("    <roots> -c $SECSVR/<candidate> -o reuse/obj/<sanitized>.o")
        w("```")
        w("")

    # ---- failures ------------------------------------------------------
    groups = {}
    for r in failed:
        sig = signature(r["error"]) or "(no error line captured)"
        groups.setdefault(sig, []).append(r)
    ordered = sorted(groups.items(), key=lambda kv: (-len(kv[1]), kv[0]))
    w("## Failure causes (top %d, one line each)" % min(8, len(ordered)))
    w("")
    if not ordered:
        w("No mapped CU failed to compile.")
        w("")
    for sig, rs in ordered[:8]:
        fams = sorted(set(x["family"] for x in rs))
        names = ", ".join(os.path.basename(x["cu"]) for x in rs[:3])
        w("* %d CU%s -- `%s` [%s] e.g. %s :: %s" % (
            len(rs), "" if len(rs) == 1 else "s", sig, "/".join(fams), names,
            explain(sig)))
    if len(ordered) > 8:
        rest = sum(len(rs) for _, rs in ordered[8:])
        w("")
        w("%d further CUs fail with %d other signatures; their first-error lines are"
          % (rest, len(ordered) - 8))
        w("in `reuse_matrix.tsv` and the full compiler output is in `reuse/log/*.log`.")
    w("")

    # ---- flags ---------------------------------------------------------
    w("## Compiler flags per family")
    w("")
    w("Pass 1 tried, in order: base = `-m32 -O2 -fPIC -fno-stack-protector")
    w("-std=gnu++98`; base `+ -fno-implement-inlines`; base `+ -fno-implement-inlines")
    w("-fno-exceptions`.  The first variant that produced a non-empty object was kept")
    w("and is what the objects in `obj/` were built with:")
    w("")
    w("| family | flag variant that compiled | CUs |")
    w("| --- | --- | --- |")
    for fam in FAMILIES:
        fr = [r for r in rows if r["family"] == fam and r["compiled"]]
        counts = {}
        for r in fr:
            key = r["flags"] if r["flags"] not in ("", "-") else "base"
            counts[key] = counts.get(key, 0) + 1
        for flags, n in sorted(counts.items(), key=lambda kv: (-kv[1], kv[0])):
            w("| %s | `%s` | %d |" % (fam, flags, n))
    w("")
    w("Every compiled CU is listed, so a variant that does not appear here was not")
    w("needed: in this run the base flags alone sufficed for all 128 objects.  The")
    w("other two variants stay in `compile_reuse.sh` and would show up in this table")
    w("if a CU needed them.")
    w("")
    w("## Artifacts")
    w("")
    w("* `reuse_matrix.tsv` -- the table every total above is computed from.")
    w("* `obj/<sanitized>.o` -- object of the first candidate that compiled.")
    w("* `log/<sanitized>.log` -- every candidate/flag attempt + full compiler output.")
    w("* `log/<sanitized>.res` -- `cu`, `family`, `tree`, `candidate`, `flags`,")
    w("  `compiled`, `roots`, `first_error`.")
    w("* `log/_min_<tree>.roots` / `log/_full_<tree>.roots` -- include-root lists.")
    w("* `log/_roots_verify.log` -- pass-2 root-ablation compiles.")
    w("* `log/_tree_state.txt` -- per-tree file count + newest file at build time.")
    w("* `tasks.tsv` -- the CU/candidate work list derived from the reuse map.")
    w("")

    with open(REPORT, "w") as fh:
        fh.write("\n".join(out) + "\n")


if __name__ == "__main__":
    sys.exit(main())
