#!/bin/bash
# Disassemble a demangled function from an object/shared file, bounded range.
# Usage: obj_fn.sh <file> <demangled-substring> [byte-length-hex]
f="$1"; pat="$2"; size="${3:-0x400}"
line=$(nm -C "$f" | grep -F -- "$pat" | head -1)
if [ -z "$line" ]; then echo "NOT FOUND: $pat"; exit 1; fi
addr=${line%% *}
start=$((16#$addr))
stop=$((start + size))
echo "== $pat @ 0x$addr =="
objdump -d -C --start-address=$start --stop-address=$stop "$f" | sed -n '/>:/,$p'
