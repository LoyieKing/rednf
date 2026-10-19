#!/bin/bash
# Assemble recon objects + reused objects + generated-protocol CUs into one
# relocatable object and report shape against ORIG.
# Usage: full_link.sh <recon-link.o> [out.o]
cd "$(dirname "$0")"
recon="$1"
out="${2:-/tmp/tss_full_all.o}"
objs="$recon"
for f in reuse/obj/*.o; do
  [ -e "$f" ] || continue
  skip=0
  while IFS= read -r ov; do
    case "$ov" in ""|\#*) continue;; esac
    case "$f" in *"$ov"*) skip=1;; esac
  done < reuse/OVERRIDES.txt
  [ "$skip" = 0 ] && objs="$objs $f"
done
if [ -f recon/vendor_only_tus.txt ]; then
  while IFS= read -r tus; do
    case "$tus" in ""|\#*) continue;; esac
    o="/tmp/fullvo_$(basename "$tus" .cpp).o"
    if sh c5build.sh "recon/$tus" "$o" >/tmp/fullvo.log 2>&1; then
      objs="$objs $o"
    else
      echo "SKIP-COMPILE(vendor) $tus"
    fi
  done < recon/vendor_only_tus.txt
fi
for f in recon/src/origsrc/*.cpp; do
  [ -e "$f" ] || continue
  case "$(basename "$f")" in test_*) continue;; esac
  base=$(basename "$f" .cpp)
  o="/tmp/fullsrc_$base.o"
  if sh c5build.sh "$f" "$o" -I recon/src/origsrc >/tmp/fullsrc.log 2>&1; then
    objs="$objs $o"
  else
    echo "SKIP-COMPILE $f ($(grep -c 'error:' /tmp/fullsrc.log) errors)"
  fi
done
rm -f /tmp/fl_have.txt /tmp/fl_orig.txt /tmp/fl_missing.txt
g++ -m32 -r -o "$out" $objs || { echo "LINK-FAIL"; exit 1; }
echo "wrote $out"
nm -C --defined-only "$out" | grep -E '^[0-9a-f]+ [A-Za-z] ' | sed -E 's/^[0-9a-f]+ [A-Za-z] //' | sort -u > /tmp/fl_have.txt
nm -DC --defined-only /opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394 2>/dev/null | sed -E 's/^[0-9a-f]+ [A-Za-z] //' | sort -u > /tmp/fl_orig.txt
comm -23 /tmp/fl_orig.txt /tmp/fl_have.txt > /tmp/fl_missing.txt
printf 'orig=%s have=%s covered=%s missing=%s\n' \
  "$(wc -l < /tmp/fl_orig.txt)" "$(wc -l < /tmp/fl_have.txt)" \
  "$(comm -12 /tmp/fl_orig.txt /tmp/fl_have.txt | wc -l)" \
  "$(wc -l < /tmp/fl_missing.txt)"
echo "== missing grouped =="
sed -E 's/^(([A-Za-z_][A-Za-z0-9_]*::){0,2}[^(:<]*).*/\1/' /tmp/fl_missing.txt | sort | uniq -c | sort -rn | head -15
