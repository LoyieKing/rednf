#!/bin/bash
# List objects (link object + reuse objects) that undefine a given demangled name.
# Usage: who_undef.sh "<demangled>"
cd "$(dirname "$0")"
pat="$1"
for o in /tmp/tss_recon_all.o reuse/obj/*.o; do
  [ -e "$o" ] || continue
  if nm -C --undefined-only "$o" 2>/dev/null | grep -qF -- "$pat"; then
    echo "$o"
  fi
done
