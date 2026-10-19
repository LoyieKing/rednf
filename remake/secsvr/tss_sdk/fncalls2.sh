#!/bin/bash
# Print the sorted unique call targets of one function, resolving object-file
# relocations to symbol names.  Usage: fncalls2.sh <file> <demangled-sub> <len>
f="$1"; pat="$2"; size="${3:-0x500}"
line=$(nm -C "$f" | grep -F -- "$pat" | head -1)
if [ -z "$line" ]; then echo "NOT FOUND: $pat"; exit 1; fi
addr=${line%% *}
start=$((16#$addr)); stop=$((start + size))
objdump -dr -C --start-address=$start --stop-address=$stop "$f" \
  | awk '
      function trim(s) { sub(/^[ \t]+/, "", s); sub(/[ \t]+$/, "", s); return s }
      /R_386_(PLT32|PC32|32)/ {
        rel = $0
        sub(/.*R_386_(PLT32|PC32|32)[ \t]+/, "", rel)
        if (pending != "") { print trim(rel); pending = "" }
        next
      }
      /^[ \t]*[0-9a-f]+:.*call[ \t]/ {
        if (pending != "") print pending
        t = $0
        sub(/.*</, "", t); sub(/>.*/, "", t)
        sub(/\+0x[0-9a-f]+$/, "", t)
        pending = t
        next
      }
      END { if (pending != "") print pending }' \
  | sort | uniq -c | sort -rn
