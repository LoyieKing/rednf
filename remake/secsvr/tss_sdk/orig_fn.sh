#!/bin/bash
# Disassemble an ORIG function found by demangled-name substring, bounded range.
# Usage: orig_fn.sh <demangled-substring> [byte-length-hex]
L=/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394
pat="$1"; size="${2:-0x500}"
line=$(nm -DC --defined-only "$L" | grep -F -- "$pat" | head -1)
if [ -z "$line" ]; then echo "NOT FOUND: $pat"; exit 1; fi
addr=${line%% *}
start=$((16#$addr))
stop=$((start + size))
echo "== $pat @ 0x$addr .. 0x$(printf %x $stop) =="
objdump -d -C --start-address=$start --stop-address=$stop "$L" | sed -n '/>:/,$p'
