#!/bin/bash
# Build and run the independent AntibotInterface factory probe.
#
#   ./build_and_run.sh [rebuilt-libtss_sdk.so.1.0.394]
#
# The default target is build-server/libtss_sdk.so.1.0.394.  This script never
# loads reference/libtss_sdk.so.1.0.394.orig: the probe must exercise the
# rebuilt library only.  ORIG addresses/slot order are encoded as symbol names
# and expected return values in the probe source/comments.
#
# Exit: 0 = probe PASS; 1 = probe mismatch; 2 = build/load/setup failure.
set -euo pipefail

HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
LIB="${1:-$ROOT/build-server/libtss_sdk.so.1.0.394}"
CXX="${CXX:-g++}"
BUILD="$HERE/build"

if [ ! -f "$LIB" ]; then
    echo "missing rebuilt library: $LIB" >&2
    exit 2
fi

# Never accept the frozen ORIG oracle, even if a caller passes its path.
ORIG_SHA256="8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7"
LIB_SHA256="$(sha256sum "$LIB" | cut -d' ' -f1)"
if [ "$LIB_SHA256" = "$ORIG_SHA256" ]; then
    echo "refusing frozen ORIG library: $LIB" >&2
    exit 2
fi

echo "target library: $LIB"
echo "target sha256:  $LIB_SHA256"

mkdir -p "$BUILD"

"$CXX" -m32 -std=gnu++98 -O2 -fno-stack-protector -D_GNU_SOURCE \
    -o "$BUILD/check_antibot_factory" \
    "$HERE/check_antibot_factory_probe.cpp" -ldl

exec "$BUILD/check_antibot_factory" "$LIB"
