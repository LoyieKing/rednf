#!/bin/bash
# =============================================================================
# compile_reuse.sh -- reproducible per-CU reuse compile matrix for libtss_sdk
#
# Usage (inside WSL, from the repository root):
#     bash tools/tss_sdk_recon/reuse/compile_reuse.sh
#
# For every CU listed in tools/tss_sdk_recon/source_reuse_map.json (the 129 CUs
# that have basename-level candidates in the reconstructed dnf_secsvr tree) this
# script tries the candidate source paths in order until one compiles with the
# vendor-matched c5root GCC 4.1.2 toolchain.  Artifacts:
#
#   reuse/obj/<sanitized>.o        object of the first candidate that compiles
#   reuse/log/<sanitized>.log      every attempt + full compiler output
#   reuse/log/<sanitized>.res      machine-readable per-CU record
#   reuse/log/_full_<tree>.roots   full root list of each tree (pass 0 baseline)
#   reuse/log/_min_<tree>.roots    set-minimised roots, verified on the corpus
#   reuse/log/_tree_state.txt      tree file counts + newest file at build time
#   reuse/reuse_matrix.tsv         per-CU score (written by score_reuse.py)
#   reuse/REPORT.md                summary (written by score_reuse.py)
#
# Pipeline
#   pass 0  derive the tasks list and the per-tree root lists / tree fingerprint
#   pass 1  compile every CU with the full include-root list of its candidate tree
#   pass 2  per tree: greedily drop include roots that no compiled CU of that tree
#           needs (set-minimisation); a root is only dropped when the test CUs
#           recompile AND produce the same defined symbols as with the full list,
#           then the reduced list is re-verified on the whole corpus of that tree
#   pass 3  recompile every successful CU with the minimised roots; a CU that
#           regresses falls back to the full root list (recorded in its .res)
#   pass 4  score_reuse.py regenerates the TSV and the report from obj/ + log/
#
# The dnf_secsvr tree is never modified: only -I search paths are added.
# Environment overrides: SECSVR, C5ROOT, ORIG, JOBS
# =============================================================================
set -u

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TSS="$(cd "$HERE/.." && pwd)"
MAP="$TSS/source_reuse_map.json"
SECSVR="${SECSVR:-/home/loyieking/dnf_workspace/dnf_secsvr}"
C5="${C5ROOT:-/home/loyieking/dnf_workspace/dnf_remake/toolchains/c5root}/usr/bin"
ORIG="${ORIG:-/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394}"
OBJD="$HERE/obj"
LOGD="$HERE/log"
TASKS="$HERE/tasks.tsv"
JOBS="${JOBS:-8}"

# Base flags = the flags the existing reuse_build.sh uses for this tree.
BASE_FLAGS="-m32 -O2 -fPIC -fno-stack-protector -std=gnu++98"
# Tried in order; a CU records the first variant that compiles.  -fno-implement-inlines
# keeps out-of-line copies of inline functions out of the object (the recon build
# needs it for some TUs); -fno-exceptions is the last resort for C-style sources.
FLAGSETS=("" "-fno-implement-inlines" "-fno-implement-inlines -fno-exceptions")

# ---------------------------------------------------------------------------
# helpers
# ---------------------------------------------------------------------------
tree_slug() { printf '%s' "$1" | tr '/' '_'; }

cand_tree() {   # ./source/secagent/src/commlib/x.cpp -> source/secagent
  local c="${1#./}" i
  i="${c%%/src/*}"
  if [ "$i" = "$c" ]; then
    case "$c" in
      */*) printf '%s' "${c%/*}" ;;
      *)   printf '%s' "." ;;
    esac
  else
    printf '%s' "$i"
  fi
}

# Ordered include-root list for a candidate tree, restricted to existing dirs.
tree_roots() {
  local tree="$1" r
  for r in \
    "$SECSVR/$tree" \
    "$SECSVR/$tree/include" \
    "$SECSVR/$tree/import/include" \
    "$SECSVR/$tree/output/commlib/framework/release/include" \
    "$SECSVR/$tree/output/commlib/zenlib/release/include" \
    "$SECSVR/$tree/output/protocol/common/release/include" \
    "$SECSVR/$tree/output/protocol/formmog/release/include" \
    "$SECSVR/$tree/output/formmog/appcomm/release/include" \
    "$SECSVR/$tree/output/formmog/secagent/release/include" \
    "$SECSVR/$tree/output/commsvr/secagent/release/include" \
    "$SECSVR/$tree/output/commsvr/gunnersvr/release/include" \
    "$SECSVR/$tree/output/commsvr/zergsvr/release/include" \
    "$SECSVR/source/secagent/output/commlib/framework/release/include" \
    "$SECSVR/source/secagent/output/commlib/zenlib/release/include" \
    "$SECSVR/source/secagent/output/protocol/common/release/include" \
    "$SECSVR/source/secagent/output/protocol/formmog/release/include" \
    "$SECSVR/source/secagent/output/formmog/appcomm/release/include" \
    "$SECSVR/source/secagent/output/formmog/secagent/release/include" \
    "$SECSVR/third_party/import/include/tencore" \
    "$SECSVR/third_party/import/include/opensource" ; do
    [ -d "$r" ] && printf '%s\n' "$r"
  done
}

# Write one tree's full root list atomically (documented artifact; only the
# sequential pass-0 writer uses this).
write_full_roots() {
  local tree="$1" slug rf
  slug="$(tree_slug "$tree")"
  rf="$LOGD/_full_$slug.roots"
  tree_roots "$tree" | awk '!s[$0]++' > "$rf.tmp.$$"
  mv -f "$rf.tmp.$$" "$rf"
  printf '%s\n' "$rf"
}

# Root list as an in-memory newline-separated string.  The parallel workers must
# not read a shared roots file: reuse/ lives on the 9p /mnt/d mount, and a read
# racing with a sibling's rewrite was observed to come back empty, which then
# compiled without any -I root (symptom: "inc/<x>.h: No such file or directory"
# on a clean candidate, cured by the next flagset retry).
full_roots_mem() { tree_roots "$1" | awk '!s[$0]++'; }

# Read a roots file written earlier in this run (pass 2/3, no concurrent writer),
# retrying a transiently empty read.
read_roots_file() {
  local f="$1" out="" i
  for i in 1 2 3 4 5; do
    out="$(sed -e '/^[[:space:]]*#/d' -e '/^[[:space:]]*$/d' "$f" 2>/dev/null)"
    [ -n "$out" ] && break
    sleep 0.2
  done
  printf '%s\n' "$out"
}

# compile one source with an in-memory roots list; stdout+stderr merged, rc = g++
run_compile() {
  local src="$1" out="$2" roots="$3"; shift 3
  local -a inc=()
  local r
  if [ -z "$roots" ]; then
    printf 'error: empty include-root list for %s\n' "$src"
    return 1
  fi
  for r in $roots; do inc+=("-I" "$r"); done
  "$C5/g++" $BASE_FLAGS "$@" "${inc[@]}" -c "$src" -o "$out" 2>&1
}

first_error_of() {   # read compiler output on stdin, print one canonical error line
  local line
  while IFS= read -r line; do
    case "$line" in
      *"error:"*|*"fatal error"*|*"Error "*) printf '%s\n' "$line"; return ;;
    esac
  done
  # no "error:" text (e.g. "No such file or directory" from the driver)
  printf '%s\n' "${1:-}"
}

err_clean() { printf '%s' "$1" | tr '\t\r\n' '   ' | cut -c1-300; }

res_get() { sed -n "s|^$2=||p" "$1" | head -1; }

res_write() {   # f cu family tree candidate flags compiled roots first_error
  {
    printf 'cu=%s\n'          "$2"
    printf 'family=%s\n'      "$3"
    printf 'tree=%s\n'        "$4"
    printf 'candidate=%s\n'   "$5"
    printf 'flags=%s\n'       "$6"
    printf 'compiled=%s\n'    "$7"
    printf 'roots=%s\n'       "$8"
    printf 'first_error=%s\n' "$9"
  } > "$1"
}

gen_tasks() {
  python3 - "$MAP" "$TASKS" <<'PY'
import json, sys

mp, out = sys.argv[1], sys.argv[2]
d = json.load(open(mp))


def fam(cu):
    if cu.startswith("src/commlib/zenlib/"):
        return "zenlib"
    if cu.startswith("src/commlib/framework/"):
        return "commlib-framework"
    if cu.startswith("src/protocol/common/"):
        return "protocol-common/tdr"
    if cu.startswith("src/protocol/formmog/"):
        return "formmog"
    return "crypto-tencrypt"


rows = []
for e in d["with_candidates_cus"]:
    cu = e["cu"]
    cands = [c[2:] if c.startswith("./") else c for c in e["candidates"]]
    if fam(cu) == "crypto-tencrypt":
        # documented preference: the tencrypt_new copies of the crypto CUs build,
        # while the secagent copies trip over source/secagent/include/TenCrypt.h
        # (an auto-generated DWARF stub with an illegal `_vptr.ITsLocal` member).
        tenc = [c for c in cands if c.startswith("tencrypt_new/")]
        rest = [c for c in cands if not c.startswith("tencrypt_new/")]
        cands = tenc + rest
    san = cu[:-4] if cu.endswith(".cpp") else cu
    san = san.replace("/", "__")
    rows.append((cu, san, fam(cu), "\t".join(cands)))

with open(out, "w") as f:
    f.write("#cu\tsanitized\tfamily\tcandidates (attempt order)\n")
    for r in rows:
        f.write("\t".join(r) + "\n")
sys.stderr.write("tasks: %d CUs\n" % len(rows))
PY
}

# ---------------------------------------------------------------------------
# worker: compile one CU (pass 1) / recompile one CU with minimised roots (pass 3)
# ---------------------------------------------------------------------------
worker() {
  local n="$1"
  local line cu san family cands
  line="$(awk -F'\t' -v n="$n" '!/^#/ && ++c == n {print; exit}' "$TASKS")"
  IFS=$'\t' read -r cu san family cands <<< "$line"
  local log="$LOGD/$san.log"
  local res="$LOGD/$san.res"
  local tmp="$OBJD/.tmp.$san.o"
  local -a allerr=()
  : > "$log"
  {
    printf '# cu: %s\n' "$cu"
    printf '# family: %s\n' "$family"
    printf '# candidate order: %s\n' "$cands"
    printf '# flagsets: base=[%s] variants=[-fno-implement-inlines] [-fno-implement-inlines -fno-exceptions]\n' "$BASE_FLAGS"
  } >> "$log"

  local cand tree roots csrc out rc fs first="" firstline="" ok=0 used="" usedflags="" usedtree=""
  for cand in $cands; do
    tree="$(cand_tree "$cand")"
    roots="$(full_roots_mem "$tree")"
    csrc="$SECSVR/$cand"
    for fs in "${FLAGSETS[@]}"; do
      printf '# attempt: candidate=%s flags=[%s] roots=%s\n' \
        "$cand" "$fs" "$(printf '%s\n' "$roots" | grep -c '^/')" >> "$log"
      if [ -f "$csrc" ]; then
        out="$(run_compile "$csrc" "$tmp" "$roots" $fs)"; rc=$?
      else
        out="g++: error: $csrc: No such file or directory"; rc=1
      fi
      [ -n "$out" ] && printf '%s\n' "$out" >> "$log"
      if [ $rc -eq 0 ] && [ -s "$tmp" ]; then
        ok=1; used="$cand"; usedflags="$fs"; usedtree="$tree"; break 2
      fi
      if [ -z "$first" ]; then
        firstline="$(printf '%s\n' "$out" | first_error_of "$(printf '%s\n' "$out" | head -1)")"
        first="$(err_clean "$firstline")"
      fi
    done
  done
  if [ $ok -eq 1 ]; then
    mv "$tmp" "$OBJD/$san.o"
    printf '# RESULT: compiled=1 candidate=%s flags=[%s] tree=%s\n' "$used" "$usedflags" "$usedtree" >> "$log"
    res_write "$res" "$cu" "$family" "$usedtree" "$used" "$usedflags" 1 full ""
  else
    rm -f "$tmp"
    printf '# RESULT: compiled=0 first_error=%s\n' "$first" >> "$log"
    printf '# FIRST_ERROR: %s\n' "$first" >> "$log"
    res_write "$res" "$cu" "$family" "$(cand_tree "$(printf '%s' "$cands" | awk '{print $1}')")" \
      "$(printf '%s' "$cands" | awk '{print $1}')" "-" 0 none "$first"
  fi
}

# ---------------------------------------------------------------------------
# pass 1 (parallel) and pass 3 (parallel) entry points
# ---------------------------------------------------------------------------
pass1() {
  local count
  count="$(awk '!/^#/ {n++} END{print n+0}' "$TASKS")"
  echo "== pass 1: compiling $count CUs with full include roots (jobs=$JOBS)"
  seq 1 "$count" | xargs -P "$JOBS" -n 1 bash "$0" --worker
}

# compile the CU described by a .res file with an in-memory roots list
compile_res() {
  local res="$1" roots="$2" tmp="$3"
  local cand flags csrc out rc
  cand="$(res_get "$res" candidate)"
  flags="$(res_get "$res" flags)"
  csrc="$SECSVR/$cand"
  [ -f "$csrc" ] || return 1
  out="$(run_compile "$csrc" "$tmp" "$roots" $flags)"; rc=$?
  printf '# verify: cu=%s roots=%s rc=%s\n%s\n' "$(res_get "$res" cu)" \
    "$(printf '%s\n' "$roots" | grep -c '^/')" "$rc" "$out" >> "$LOGD/_roots_verify.log"
  [ $rc -eq 0 ] && [ -s "$tmp" ]
}

# defined symbol name sets (demangled, both globals and locals) must be equal
sym_same() {
  nm -C --defined-only "$1" 2>/dev/null | sed -E 's/^[0-9a-f]+ ([A-Za-z?]) //' | sort -u > "$HERE/.sym_a"
  nm -C --defined-only "$2" 2>/dev/null | sed -E 's/^[0-9a-f]+ ([A-Za-z?]) //' | sort -u > "$HERE/.sym_b"
  cmp -s "$HERE/.sym_a" "$HERE/.sym_b"
  local rc=$?
  rm -f "$HERE/.sym_a" "$HERE/.sym_b"
  return $rc
}

# roots_equiv <full-roots-string> <trial-roots-string> <res-file...>: every listed
# CU must compile with the trial roots AND produce exactly the same defined
# symbols as with the full roots (a dropped root may otherwise silently switch a
# header copy and change the object while still compiling).
roots_equiv() {
  local full="$1" trial="$2"; shift 2
  local a="$LOGD/.eq_full.o" b="$LOGD/.eq_try.o" f
  for f in "$@"; do
    if ! compile_res "$f" "$full" "$a" || ! compile_res "$f" "$trial" "$b" \
       || ! sym_same "$a" "$b"; then
      rm -f "$a" "$b"
      return 1
    fi
  done
  rm -f "$a" "$b"
  return 0
}

reduce_tree() {   # pass 2 for one tree
  local tree="$1" slug full minf f r total fulln keep trial
  slug="$(tree_slug "$tree")"
  full="$(full_roots_mem "$tree")"
  minf="$LOGD/_min_$slug.roots"
  fulln="$(printf '%s\n' "$full" | grep -c '^/')"
  local -a corpus=() reps=()
  for f in "$LOGD"/*.res; do
    [ -f "$f" ] || continue
    [ "$(res_get "$f" compiled)" = "1" ] || continue
    [ "$(res_get "$f" tree)" = "$tree" ] || continue
    corpus+=("$f")
  done
  if [ ${#corpus[@]} -eq 0 ]; then
    { printf '# tree: %s\n# minimized: n/a\n# verified: 0/0 (no CU compiled from this tree)\n# full-roots: %s\n# minimized-roots: %s\n' \
        "$tree" "$fulln" "$fulln"; printf '%s\n' "$full"; } > "$minf"
    printf '   %-18s no compiled CUs; full roots kept\n' "$tree"
    return
  fi
  # representatives: the 4 largest objects (most complex include demands first)
  local n=0
  while IFS= read -r f; do
    reps+=("$f"); n=$((n+1)); [ $n -ge 4 ] && break
  done < <(for f in "${corpus[@]}"; do
             o="$OBJD/$(basename "$f" .res).o"
             printf '%s %s\n' "$(stat -c %s "$o" 2>/dev/null || echo 0)" "$f"
           done | sort -rn | awk '{print $2}')
  # greedy set-minimisation on the representatives
  keep="$full"
  while IFS= read -r r; do
    case "$r" in ''|'#'*) continue ;; esac
    trial="$(printf '%s\n' "$keep" | grep -vxF "$r")"
    [ -n "$trial" ] || continue
    if roots_equiv "$full" "$trial" "${reps[@]}"; then keep="$trial"; fi
  done < <(printf '%s\n' "$full")
  # verify the reduced set on the whole corpus of the tree
  local ok=0 bad=0
  for f in "${corpus[@]}"; do
    if roots_equiv "$full" "$keep" "$f"; then ok=$((ok+1)); else bad=$((bad+1)); fi
  done
  total=${#corpus[@]}
  if [ $bad -eq 0 ]; then
    { printf '# tree: %s\n# minimized: yes (greedy set-minimisation; every CU of this tree recompiled with the reduced list produced the same defined symbols as with the full list)\n# verified: %d/%d\n# full-roots: %s\n# minimized-roots: %s\n' \
        "$tree" "$ok" "$total" "$fulln" "$(printf '%s\n' "$keep" | grep -c '^/')"
      printf '%s\n' "$keep"; } > "$minf"
    printf '   %-18s roots %s -> %s, symbol-identical on %s/%s CUs\n' "$tree" \
      "$fulln" "$(printf '%s\n' "$keep" | grep -c '^/')" "$ok" "$total"
  else
    { printf '# tree: %s\n# minimized: no (reduced list changed or failed on %d/%d CUs; full list kept)\n# verified: %d/%d\n# full-roots: %s\n# minimized-roots: %s\n' \
        "$tree" "$bad" "$total" "$total" "$total" "$fulln" "$fulln"
      printf '%s\n' "$full"; } > "$minf"
    printf '   %-18s roots %s -> full (differs on %s/%s CUs)\n' "$tree" "$fulln" "$bad" "$total"
  fi
}

reduce_all() {
  echo "== pass 2: minimising include roots per candidate tree"
  local tree
  for tree in $(for f in "$LOGD"/*.res; do res_get "$f" tree; done | sort -u); do
    [ -n "$tree" ] && reduce_tree "$tree"
  done
}
# Pre-generate the full root list of every tree used, before pass 1 runs the
# workers, and record the tree state for the report.
gen_full_roots() {
  local tree
  for tree in $(awk -F'\t' '!/^#/ {print $4}' "$TASKS" | tr ' ' '\n' \
                 | sed -E 's|/src/.*||' | sort -u); do
    [ -n "$tree" ] || continue
    [ -d "$SECSVR/$tree" ] || continue
    write_full_roots "$tree" > /dev/null
  done
}

# Fingerprint the source trees this run compiles against, so a later tree edit
# can be told apart from a stale matrix.
tree_state() {
  local tree cnt newest
  : > "$LOGD/_tree_state.txt"
  for tree in $(awk -F'\t' '!/^#/ {print $4}' "$TASKS" | tr ' ' '\n' \
                 | sed -E 's|/src/.*||' | sort -u); do
    [ -n "$tree" ] || continue
    [ -d "$SECSVR/$tree" ] || continue
    cnt="$(find "$SECSVR/$tree" \( -name '*.cpp' -o -name '*.h' \) -type f | wc -l)"
    newest="$(find "$SECSVR/$tree" \( -name '*.cpp' -o -name '*.h' \) -type f \
                -printf '%TY-%Tm-%Td %TH:%TM %p\n' 2>/dev/null | sort | tail -1)"
    printf '%s\t%s\t%s\n' "$tree" "$cnt" "$newest" >> "$LOGD/_tree_state.txt"
  done
}


# pass 3: rebuild every compiled CU with the minimised roots of its tree
worker_recompile() {
  local n="$1"
  local line cu san family cands
  line="$(awk -F'\t' -v n="$n" '!/^#/ && ++c == n {print; exit}' "$TASKS")"
  IFS=$'\t' read -r cu san family cands <<< "$line"
  local res="$LOGD/$san.res"
  [ "$(res_get "$res" compiled)" = "1" ] || return 0
  local tree slug minf fullf tmp="$OBJD/.tmp.$san.o"
  tree="$(res_get "$res" tree)"
  flags="$(res_get "$res" flags)"
  candidate="$(res_get "$res" candidate)"
  slug="$(tree_slug "$tree")"
  minf="$LOGD/_min_$slug.roots"
  fullf="$LOGD/_full_$slug.roots"
  local log="$LOGD/$san.log" out rc
  local minroots fullroots
  minroots="$(read_roots_file "$minf")"
  fullroots="$(read_roots_file "$fullf")"
  {
    printf '# pass 3: rebuild with the set-minimised include roots of tree %s\n' "$tree"
    printf '# pass 3 attempt: candidate=%s flags=[%s] roots=%s\n' "$candidate" "$flags" "$(printf '%s\n' "$minroots" | grep -c '^/')"
  } >> "$log"
  out="$(run_compile "$SECSVR/$candidate" "$tmp" "$minroots" $flags)"; rc=$?
  printf '%s\n' "$out" >> "$log"
  if [ $rc -eq 0 ] && [ -s "$tmp" ]; then
    mv "$tmp" "$OBJD/$san.o"
    printf '# RESULT: compiled=1 candidate=%s flags=[%s] tree=%s roots=minimized\n' "$candidate" "$flags" "$tree" >> "$log"
    res_write "$res" "$cu" "$family" "$tree" "$candidate" "$flags" 1 minimized ""
    return 0
  fi
  # fall back to the full root list for this CU only
  printf '# pass 3 fallback: minimised roots failed, retrying with the full root list\n' >> "$log"
  out="$(run_compile "$SECSVR/$candidate" "$tmp" "$fullroots" $flags)"; rc=$?
  printf '%s\n' "$out" >> "$log"
  if [ $rc -eq 0 ] && [ -s "$tmp" ]; then
    mv "$tmp" "$OBJD/$san.o"
    printf '# RESULT: compiled=1 candidate=%s flags=[%s] tree=%s roots=full\n' "$candidate" "$flags" "$tree" >> "$log"
    res_write "$res" "$cu" "$family" "$tree" "$candidate" "$flags" 1 full ""
  else
    rm -f "$tmp"
    local e; e="$(err_clean "$(printf '%s\n' "$out" | first_error_of "$(printf '%s\n' "$out" | head -1)")")"
    printf '# RESULT: compiled=0 first_error=%s\n' "$e" >> "$log"
    printf '# FIRST_ERROR: %s\n' "$e" >> "$log"
    res_write "$res" "$cu" "$family" "$tree" "$candidate" "$flags" 0 none "$e"
  fi
}

pass3() {
  local count
  count="$(awk '!/^#/ {n++} END{print n+0}' "$TASKS")"
  echo "== pass 3: rebuilding successful CUs with minimised roots (jobs=$JOBS)"
  seq 1 "$count" | xargs -P "$JOBS" -n 1 bash "$0" --recompile
}

main() {
  [ -x "$C5/g++" ] || { echo "error: c5root g++ not found at $C5/g++" >&2; exit 1; }
  [ -f "$MAP" ]    || { echo "error: reuse map not found at $MAP" >&2; exit 1; }
  [ -f "$ORIG" ]   || { echo "error: ORIG lib not found at $ORIG" >&2; exit 1; }
  mkdir -p "$OBJD" "$LOGD"
  rm -f "$OBJD"/*.o "$OBJD"/.tmp.* "$OBJD"/.keep.* "$OBJD"/.probe.o "$LOGD"/*.log "$LOGD"/*.res "$LOGD"/_*.roots
  echo "== include roots: $SECSVR (dnf_secsvr is not modified)"
  gen_tasks
  tree_state
  gen_full_roots
  pass1
  rm -f "$OBJD"/.keep.*.o
  reduce_all
  pass3
  echo "== pass 4: scoring"
  python3 "$HERE/score_reuse.py"
}

case "${1:-}" in
  --worker)    worker "${2:?worker index}" ;;
  --recompile) worker_recompile "${2:?worker index}" ;;
  "")          main ;;
  *)           echo "usage: $0 [--worker N | --recompile N]" >&2; exit 2 ;;
esac
