#!/bin/bash
# Try to build a shared object from all reconstructed+reused objects.
# Undefined references are allowed (they resolve against ORIG at runtime);
# duplicate strong definitions are reported.
cd "$(dirname "$0")"
recon="${1:-/tmp/tss_recon_all.o}"
out="${2:-/tmp/librecon.so}"
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
  o="/tmp/so_$base.o"
  sh c5build.sh "$f" "$o" -I recon/src/origsrc >/tmp/so_compile.log 2>&1 && objs="$objs $o"
done
g++ -m32 -shared -o "$out" $objs 2>/tmp/so_link.err
rc=$?
echo "link-rc=$rc"
grep -E "multiple definition|undefined reference" /tmp/so_link.err | head -10
[ "$rc" -eq 0 ] && ls -l "$out"
