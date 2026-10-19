#!/bin/bash
# Symbol coverage of a `g++ -r` linked recon object against ORIG dynsym.
# Usage: coverage.sh <linked.o> <grep-regex>
# Prints missing ORIG symbols, extra recon symbols and both counts.
ORIG=/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394
o="$1"; rx="$2"
norm() { sed -E 's/^[0-9a-f]+ ([A-Za-z] )?//' | grep -E "$rx" | sort -u; }
nm -DC --defined-only "$ORIG" 2>/dev/null | norm > /tmp/cov_orig.$$.txt
nm -C --defined-only "$o" 2>/dev/null | grep -E '^[0-9a-f]+ [A-Za-z] ' | norm > /tmp/cov_new.$$.txt
echo "== ORIG-only (missing) =="
comm -23 /tmp/cov_orig.$$.txt /tmp/cov_new.$$.txt
echo "== recon-only (extra) =="
comm -13 /tmp/cov_orig.$$.txt /tmp/cov_new.$$.txt
printf 'counts orig=%s recon=%s covered=%s\n' \
  "$(wc -l < /tmp/cov_orig.$$.txt)" \
  "$(wc -l < /tmp/cov_new.$$.txt)" \
  "$(comm -12 /tmp/cov_orig.$$.txt /tmp/cov_new.$$.txt | wc -l)"
