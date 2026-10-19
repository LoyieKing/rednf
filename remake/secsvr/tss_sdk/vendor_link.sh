#!/bin/bash
# Vendor build of every recon TU: compile with the c5root GCC 4.1.2 (same
# compiler family as ORIG), then combine with the host ld -r.  This removes the
# mixed-libstdc++ artifact from the symbol-shape measurement.
# Usage: vendor_link.sh [out.o]
cd "$(dirname "$0")/recon" || exit 1
OUT="${1:-/tmp/tss_vendor_all.o}"
OBJS=""
fail=0
for f in $(awk -f ../extract_sources.awk link_check.sh); do
  [ -e "$f" ] || { echo "MISSING $f"; fail=1; continue; }
  base=$(printf '%s' "$f" | tr '/.' '__')
  o="/tmp/vrec_$base.o"
  case "$f" in
    *.c) extra="-x c++" ;;
    *) extra="" ;;
  esac
  if sh ../c5build.sh "recon/$f" "$o" $extra >/tmp/vrec.log 2>&1; then
    OBJS="$OBJS $o"
  else
    echo "COMPILE-FAIL $f"
    grep -m2 "error:" /tmp/vrec.log
    fail=1
  fi
done
g++ -m32 -r -o "$OUT" $OBJS || { echo "LD-FAIL"; exit 1; }
echo "wrote $OUT"
nm -C --defined-only "$OUT" | grep -E '^[0-9a-f]+ [A-Za-z] ' \
  | sed -E 's/^[0-9a-f]+ [A-Za-z] //' | sort -u > /tmp/vl_have.txt
nm -DC --defined-only /opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394 \
  2>/dev/null | sed -E 's/^[0-9a-f]+ [A-Za-z] //' | sort -u > /tmp/vl_orig.txt
comm -23 /tmp/vl_orig.txt /tmp/vl_have.txt > /tmp/vl_missing.txt
printf 'orig=%s have=%s covered=%s missing=%s compile_fail=%d\n' \
  "$(wc -l < /tmp/vl_orig.txt)" "$(wc -l < /tmp/vl_have.txt)" \
  "$(comm -12 /tmp/vl_orig.txt /tmp/vl_have.txt | wc -l)" \
  "$(wc -l < /tmp/vl_missing.txt)" "$fail"
