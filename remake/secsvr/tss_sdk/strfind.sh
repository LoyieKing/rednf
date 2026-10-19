#!/bin/bash
# Check which recon log formats exist verbatim in the ORIG binary.
# Usage: strfind.sh   (reads one literal string per line on stdin)
ORIG=/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394
while IFS= read -r line; do
  [ -z "$line" ] && continue
  if grep -aqF -- "$line" "$ORIG"; then
    printf 'PRESENT  %s\n' "$line"
  else
    printf 'MISSING  %s\n' "$line"
  fi
done
