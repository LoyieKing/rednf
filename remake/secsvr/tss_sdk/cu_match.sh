#!/bin/bash
# Compare a DWARF CU's function-symbol set with a recon/reused object's symbols.
# Usage: cu_match.sh <cu-substring> <obj.o>
# Demangles ORIG's mangled names with c++filt and normalises both sides.
cd "$(dirname "$0")"
ORIG=/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394
python3 cu_symbols.py "$ORIG" "$1" \
  | grep '^0x' | awk '{print $2}' | c++filt | sort -u > /tmp/cm_cu.$$.txt
nm -C --defined-only "$2" 2>/dev/null | grep -E '^[0-9a-f]+ [A-Za-z] ' \
  | sed -E 's/^[0-9a-f]+ [A-Za-z] //' | sort -u > /tmp/cm_obj.$$.txt
echo "== in CU, absent from object =="
comm -23 /tmp/cm_cu.$$.txt /tmp/cm_obj.$$.txt
echo "== in object, not in CU =="
comm -13 /tmp/cm_cu.$$.txt /tmp/cm_obj.$$.txt
printf 'cu=%s obj=%s matched=%s\n' \
  "$(wc -l < /tmp/cm_cu.$$.txt)" "$(wc -l < /tmp/cm_obj.$$.txt)" \
  "$(comm -12 /tmp/cm_cu.$$.txt /tmp/cm_obj.$$.txt | wc -l)"
