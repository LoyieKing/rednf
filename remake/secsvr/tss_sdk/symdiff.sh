#!/bin/bash
# Compare demangled defined symbols of ORIG shared object against a recon object file.
# Usage: symdiff.sh <orig.so> <our.o> <grep-regex>
set -e
L="$1"; O="$2"; R="$3"
nm -C --defined-only -D "$L" 2>/dev/null | sed 's/^[0-9a-f]* //' | grep -E "$R" | sort -u > /tmp/sd_orig.$$.sym
nm -C "$O" 2>/dev/null | sed 's/^[0-9a-f]* //' | grep -E "$R" | sort -u > /tmp/sd_new.$$.sym
echo "== ORIG-only =="
comm -23 /tmp/sd_orig.$$.sym /tmp/sd_new.$$.sym
echo "== NEW-only =="
comm -13 /tmp/sd_orig.$$.sym /tmp/sd_new.$$.sym
echo "== counts (orig new) =="
wc -l < /tmp/sd_orig.$$.sym
wc -l < /tmp/sd_new.$$.sym
