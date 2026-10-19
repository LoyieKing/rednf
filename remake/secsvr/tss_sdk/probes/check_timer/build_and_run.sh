#!/bin/bash
# Build and run the isolated check_timer differential probe.
#
#   ORIG        : reference/libtss_sdk.so.1.0.394.orig (or TSS_SDK_ORIG)
#   AFTER       : recon/.../dp_msg_handler_recv_agent.cpp as checked out
#   BEFORE      : copy with check_timer_before.patch (config packet 20/8)
#   STAT-BEFORE : copy with check_timer_stat_before.patch (DPClientStatPtc
#                 reads game_dynamic_decrypt_count_ instead of
#                 fix_encrypt_count_)
#
# The rebuilt DSOs are linked with -Bsymbolic so their own check_timer/stat
# bodies and helpers bind locally.  SendDpMsgToClient and
# send_error_report_msg are intentionally left undefined; both resolve to the
# probe executable's exported output-boundary hooks.
#
# No source file is modified by this script: BEFORE and STAT-BEFORE are
# produced in build/.
# Usage:
#   ./build_and_run.sh [--no-before] [--orig /path/to/libtss_sdk.so...]
# Pure failing-before check (expected exit 1):
#   ./build/check_timer_probe --orig reference/libtss_sdk.so.1.0.394.orig \
#       --after ./build/libcheck_timer_before.so
#
#
# Exit: 0 = AFTER matches ORIG and BEFORE fails as expected; 1 = mismatch;
#       2 = setup/load failure.

set -euo pipefail

HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
TU="$ROOT/recon/src/formmog/tss_sdk/plugins/proto_proc/dp_msg_handler_recv_agent.cpp"
ORIG="${TSS_SDK_ORIG:-$ROOT/reference/libtss_sdk.so.1.0.394.orig}"
BUILD="$HERE/build"
CXX="${CXX:-g++}"
PATCH="${PATCH:-patch}"

RUN_BEFORE=1
EXTRA=()
while [ $# -gt 0 ]; do
    case "$1" in
        --no-before)
            RUN_BEFORE=0
            shift
            ;;
        --orig)
            if [ $# -lt 2 ]; then
                echo "FATAL: --orig needs a path" >&2
                exit 2
            fi
            ORIG="$2"
            shift 2
            ;;
        *)
            EXTRA+=("$1")
            shift
            ;;
    esac
done

if [ ! -f "$TU" ]; then
    echo "FATAL: missing target TU: $TU" >&2
    exit 2
fi
if [ ! -f "$ORIG" ]; then
    echo "FATAL: missing ORIG backup: $ORIG (set TSS_SDK_ORIG)" >&2
    exit 2
fi

echo "ORIG: $ORIG"
sha256sum "$ORIG"

mkdir -p "$BUILD"

# Build the reconstructed TU with the host -m32 compiler.  Do not pass
# -fno-implement-inlines here: the TU relies on its in-class inline
# DPVMBaseEngine/CodeInfo helpers, and ORIG does not export all of them.
# The probe header asserts the ABI/layout used across the DSO boundary.
#
# The TU now uses the canonical tag-394 headers, which include each other as
# "src/..." (recon root), plus the shared secagent declarations used by the
# official host build (build_server.py).  Keep both roots explicit: the probe
# deliberately does not reuse build_server.py's object files.
CXXFLAGS=(-m32 -O2 -fPIC -fno-stack-protector -fno-strict-aliasing
          -std=gnu++98 -D_GLIBCXX_USE_CXX11_ABI=0)
INCLUDES=(-I"$ROOT/recon"
          -I"$ROOT/recon/include"
          -I"$ROOT/../source/secagent")

build_tu() {
    local src="$1"
    local so="$2"
    local obj
    obj="$BUILD/$(basename "$so" .so).o"
    "$CXX" "${CXXFLAGS[@]}" "${INCLUDES[@]}" -c "$src" -o "$obj"
    "$CXX" -m32 -shared -Wl,-Bsymbolic -o "$so" "$obj"
}


echo "== build after =="
build_tu "$TU" "$BUILD/libcheck_timer_after.so"

if [ "$RUN_BEFORE" = 1 ]; then
    echo "== build before (patched copy in build/) =="
    cp -f "$TU" "$BUILD/dp_msg_handler_recv_agent.before.cpp"
    "$PATCH" -s "$BUILD/dp_msg_handler_recv_agent.before.cpp" \
        < "$HERE/check_timer_before.patch"
    build_tu "$BUILD/dp_msg_handler_recv_agent.before.cpp" \
        "$BUILD/libcheck_timer_before.so"
    echo "== build stat-before (DPClientStatPtc pre-fix copy) =="
    cp -f "$TU" "$BUILD/dp_msg_handler_recv_agent.stat_before.cpp"
    "$PATCH" -s "$BUILD/dp_msg_handler_recv_agent.stat_before.cpp" \
        < "$HERE/check_timer_stat_before.patch"
    build_tu "$BUILD/dp_msg_handler_recv_agent.stat_before.cpp" \
        "$BUILD/libcheck_timer_stat_before.so"
fi

echo "== build probe =="
"$CXX" -m32 -O2 -std=gnu++11 -fno-stack-protector -D_GNU_SOURCE \
    -I"$HERE" -rdynamic -o "$BUILD/check_timer_probe" \
    "$HERE/check_timer_probe.cpp" -ldl

echo "== run probe =="
if [ "$RUN_BEFORE" = 1 ]; then
    TSS_SDK_ORIG="$ORIG" "$BUILD/check_timer_probe" \
        --orig "$ORIG" \
        --after "$BUILD/libcheck_timer_after.so" \
        --before "$BUILD/libcheck_timer_before.so" \
        --stat-before "$BUILD/libcheck_timer_stat_before.so" \
        "${EXTRA[@]}"
else
    TSS_SDK_ORIG="$ORIG" "$BUILD/check_timer_probe" \
        --orig "$ORIG" \
        --after "$BUILD/libcheck_timer_after.so" \
        "${EXTRA[@]}"
fi
