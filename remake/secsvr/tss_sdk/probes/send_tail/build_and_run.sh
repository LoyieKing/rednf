#!/bin/bash
# Build and run the isolated SendDpMsgToClient send-tail differential probe.
#
#   ORIG   : reference/libtss_sdk.so.1.0.394.orig (or TSS_SDK_ORIG)
#   AFTER  : recon/.../dp_msg_handler_send_agent.cpp as currently checked out
#   MUTANT : a build-directory copy with send_tail_mutant.patch applied
#
# The probe replaces only the external boundary: encrypt, the transport send
# call and the optional bill dump.  The length prefix, DPInterDataHead pack,
# encrypt-call arguments, send return propagation and user counter update all
# remain the implementation under test.  The mutant changes the declared
# packet_size by one byte; the probe must reject it.
#
# No source file is modified by this script: the mutant is produced in build/.
# Usage:
#   ./build_and_run.sh [--no-mutant] [--orig /path/to/libtss_sdk.so...]
#
# Exit: 0 = AFTER matches ORIG and MUTANT fails as expected; 1 = mismatch;
#       2 = setup/load failure.

set -euo pipefail

HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
TU="$ROOT/recon/src/formmog/tss_sdk/plugins/proto_proc/dp_msg_handler_send_agent.cpp"
ORIG="${TSS_SDK_ORIG:-$ROOT/reference/libtss_sdk.so.1.0.394.orig}"
BUILD="$HERE/build"
CXX="${CXX:-g++}"
PATCH="${PATCH:-patch}"

RUN_MUTANT=1
EXTRA=()
while [ $# -gt 0 ]; do
    case "$1" in
        --no-mutant)
            RUN_MUTANT=0
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

# Canonical tag-394 headers include each other as "src/..." from the recon
# root; the official host build (build_server.py) also adds recon/include and
# the secagent source root.
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
build_tu "$TU" "$BUILD/libsend_tail_after.so"

if [ "$RUN_MUTANT" = 1 ]; then
    echo "== build mutant (patched copy in build/) =="
    cp -f "$TU" "$BUILD/dp_msg_handler_send_agent.mutant.cpp"
    "$PATCH" -s "$BUILD/dp_msg_handler_send_agent.mutant.cpp" \
        < "$HERE/send_tail_mutant.patch"
    build_tu "$BUILD/dp_msg_handler_send_agent.mutant.cpp" \
        "$BUILD/libsend_tail_mutant.so"
fi

echo "== build probe =="
"$CXX" -m32 -O2 -std=gnu++11 -fno-stack-protector -D_GNU_SOURCE \
    -I"$HERE" -I"$HERE/../check_timer" -rdynamic \
    -o "$BUILD/send_tail_probe" "$HERE/send_tail_probe.cpp" -ldl

echo "== run probe =="
if [ "$RUN_MUTANT" = 1 ]; then
    TSS_SDK_ORIG="$ORIG" "$BUILD/send_tail_probe" \
        --orig "$ORIG" \
        --after "$BUILD/libsend_tail_after.so" \
        --mutant "$BUILD/libsend_tail_mutant.so" \
        "${EXTRA[@]}"
else
    TSS_SDK_ORIG="$ORIG" "$BUILD/send_tail_probe" \
        --orig "$ORIG" \
        --after "$BUILD/libsend_tail_after.so" \
        "${EXTRA[@]}"
fi
