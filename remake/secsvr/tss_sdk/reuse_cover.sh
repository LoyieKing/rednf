#!/bin/bash
# How many of an object's defined symbols exist verbatim in ORIG dynsym.
# Usage: reuse_cover.sh <obj.o>
ORIG=/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394
o="$1"
nm -C --defined-only "$o" 2>/dev/null | grep -E '^[0-9a-f]+ [A-Za-z] ' \
  | sed -E 's/^[0-9a-f]+ [A-Za-z] //' | sort -u > /tmp/rc_obj.$$.txt
nm -DC --defined-only "$ORIG" 2>/dev/null | sed -E 's/^[0-9a-f]+ [A-Za-z] //' \
  | sort -u > /tmp/rc_orig.$$.txt
total=$(wc -l < /tmp/rc_obj.$$.txt)
hit=$(comm -12 /tmp/rc_obj.$$.txt /tmp/rc_orig.$$.txt | wc -l)
printf 'object=%s\nmatched=%s\nobj_total=%s\n' "$o" "$hit" "$total"
echo "== not in ORIG (first 15) =="
comm -23 /tmp/rc_obj.$$.txt /tmp/rc_orig.$$.txt | head -15
