#!/bin/bash
# Coverage of ORIG dynsym by (recon link object + reused objects).
# Usage: combined_cover.sh <recon.o> <reuse-obj-dir>
ORIG=/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394
recon="$1"; dir="$2"
nm -DC --defined-only "$ORIG" 2>/dev/null | sed -E 's/^[0-9a-f]+ [A-Za-z] //' | sort -u > /tmp/cc_orig.$$.txt
{ nm -C --defined-only "$recon" 2>/dev/null | grep -E '^[0-9a-f]+ [A-Za-z] ' | sed -E 's/^[0-9a-f]+ [A-Za-z] //'
  for o in "$dir"/*.o; do
    nm -C --defined-only "$o" 2>/dev/null | grep -E '^[0-9a-f]+ [A-Za-z] ' | sed -E 's/^[0-9a-f]+ [A-Za-z] //'
  done
} | sort -u > /tmp/cc_have.$$.txt
comm -23 /tmp/cc_orig.$$.txt /tmp/cc_have.$$.txt > /tmp/cc_missing.$$.txt
printf 'orig=%s have=%s covered=%s missing=%s\n' \
  "$(wc -l < /tmp/cc_orig.$$.txt)" "$(wc -l < /tmp/cc_have.$$.txt)" \
  "$(comm -12 /tmp/cc_orig.$$.txt /tmp/cc_have.$$.txt | wc -l)" \
  "$(wc -l < /tmp/cc_missing.$$.txt)"
echo "== missing grouped by top scope =="
sed -E 's/^(([A-Za-z_][A-Za-z0-9_]*::){0,2}[^(:<]*).*/\1/' /tmp/cc_missing.$$.txt \
  | sort | uniq -c | sort -rn | head -25
