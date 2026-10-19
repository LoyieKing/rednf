#!/bin/bash
# Compare per-symbol code sizes between ORIG shared object and a recon object.
# Usage: sizecmp.sh <our.o> <grep-regex>
# Prints "size_orig size_new ratio name" for symbols present in both, worst
# ratio first, so structural divergences surface before compiler noise.
set -u
L=/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394
O="$1"; R="$2"

norm() {
  local f="$1" extra="$2" addr size kind rest
  while read -r addr size kind rest; do
    [ -z "$rest" ] && continue
    printf '%s\t%d\n' "$rest" "$((16#$size))"
  done < <(nm -S -C --defined-only $extra "$f" 2>/dev/null | grep -E "$R" | sort -u)
}

norm "$L" -D > /tmp/sc_orig.$$.txt
norm "$O" "" > /tmp/sc_new.$$.txt

awk -F'\t' '
  NR==FNR { o[$1]=$2; next }
  ($1 in o) {
    a=o[$1]; n=$2; seen++
    if (a == n) { same++; next }
    r = (a>0) ? n/a : 0
    printf "%6d %6d  %.3f  %s\n", a, n, r, $1
  }
  END { printf "matched=%d identical=%d differing=%d\n", seen, same, seen-same }
' /tmp/sc_orig.$$.txt /tmp/sc_new.$$.txt | sort -k3,3 -g | head -40
