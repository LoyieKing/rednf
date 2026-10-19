#!/bin/bash
# Count occurrences of each literal needle (one per stdin line) in a file.
# Usage: countneedles.sh <file> < needles.txt
f="$1"
while IFS= read -r line; do
  [ -z "$line" ] && continue
  printf '%-30s %s\n' "$line" "$(grep -cF -- "$line" "$f")"
done
