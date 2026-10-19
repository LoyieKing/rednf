#!/bin/bash
# List global (non-local) defined symbols of a recon object that do NOT exist
# in the ORIG dynamic symbol table.  Usage: extra_globals.sh <our.o> [regex]
ORIG=/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394
f="$1"; rx="${2:-.}"
nm -C --defined-only "$f" | grep -E "^[0-9a-f]+ [A-Z] " | sed 's/^[0-9a-f]* [A-Z] //' | sort -u \
  | grep -E "$rx" > /tmp/eg_new.txt
nm -DC --defined-only "$ORIG" 2>/dev/null | sed 's/^[0-9a-f]* //' | sed 's/^[A-Za-z] //' | sort -u > /tmp/eg_orig.txt
echo "== globals only in recon =="
comm -23 /tmp/eg_new.txt /tmp/eg_orig.txt
echo "== counts (recon/globals-checked) =="
wc -l < /tmp/eg_new.txt
