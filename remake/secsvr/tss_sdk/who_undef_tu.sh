#!/bin/bash
# Locate the recon TU object that undefines a demangled name.
# Usage: who_undef_tu.sh "<demangled>"
pat="$1"
for o in /tmp/recon_*.o; do
  [ -e "$o" ] || continue
  if nm -C --undefined-only "$o" 2>/dev/null | grep -qF -- "$pat"; then
    echo "$o"
  fi
done
