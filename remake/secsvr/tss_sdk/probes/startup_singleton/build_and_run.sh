#!/bin/sh
# Build and run commands for startup_singleton_probe.cpp.
#
# Not executed as part of the source-repair task.  The lead runs it after the
# rebuilt libtss_sdk.so.1.0.394 exists.
#
# ORIG is loaded in its own process and the rebuilt library in another; each
# JSON report is independent.  Compare the normalized fields:
#   manager.obj_symbol_size == 56, manager.registered_plugin_count == 6,
#   manager slots 1..6 non-null with plugin_id == slot id, slot 7 null,
#   comm_svc.obj_symbol_size == 40, comm_svc.plugin_mgr_equals_manager == true,
#   both object_creator_symbol_found / guard_set true.
set -eu

HERE=$(cd "$(dirname "$0")" && pwd)
ROOT=$(cd "$HERE/../.." && pwd)
ORIG=${ORIG:-"$ROOT/reference/libtss_sdk.so.1.0.394.orig"}
BUILT=${BUILT:-"$ROOT/build-server/libtss_sdk.so.1.0.394"}
CXX=${CXX:-g++}
OUT=${OUT:-"$HERE/build"}

mkdir -p "$OUT"
"$CXX" -m32 -std=gnu++98 -O2 -Wall -Wextra \
    -o "$OUT/startup_singleton_probe" \
    "$HERE/startup_singleton_probe.cpp" -ldl

"$OUT/startup_singleton_probe" --lib "$ORIG" > "$OUT/orig.json"
"$OUT/startup_singleton_probe" --lib "$BUILT" > "$OUT/rebuilt.json"

# Mutation controls: each must PASS only when the defect is actually present.
# Build the mutants from copies of the sources (never modify the tree):
#   - no get_obj roots: remove the six g_obj_* definitions
#   - one-byte manager: use a manager model with no data members
#   - no object_creator: drop the two explicit object_creator instantiations
# Then run, for example:
#   ./startup_singleton_probe --lib mutant-no-roots.so --expect no-roots
#   ./startup_singleton_probe --lib mutant-one-byte.so --expect one-byte-manager
#   ./startup_singleton_probe --lib mutant-no-creator.so --expect no-object-creator

printf '%s\n' "orig:    $OUT/orig.json"
printf '%s\n' "rebuilt: $OUT/rebuilt.json"
