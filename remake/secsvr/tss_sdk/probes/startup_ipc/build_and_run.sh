#!/bin/sh
# Build and run commands for startup_ipc_probe.cpp.
#
# Not executed as part of the source-repair task.  The lead runs it after the
# rebuilt libtss_sdk.so.1.0.394 exists.  ORIG and BUILT are loaded in separate
# processes; each JSON report is independent.
#
# Key comparisons (orig vs rebuilt):
#   bus: instance allocation 10292; fresh header 4,2,8388608,8388608;
#        pipe frames survive cleanup/resume; bus.resume.tail_size_of_pipe2 and
#        tail_size_of_room2 are 0 (ORIG 0x1b4af9-0x1b4b19 copies local->mmap);
#        partial init, restore mismatch, missing/short file failures all return
#        -1 and release the dequechunk objects.
#   status: fresh multi_thread allocates 28-byte ZEN_Thread_Mutex; guard
#        false/true switches to 8-byte ZEN_Null_Mutex and back; cleanup frees
#        76-byte object, two 12-byte shm_vector objects and the mutex;
#        resume keeps the raw shared counter; mirror failure leaves sandy and
#        clears copy; sandy failure clears both; shm_open failure returns -1.
#   allocator: canary_bad must be 0 (a 4-byte thread-mutex model would overrun
#        its requested size); untracked_delete / record_overflow are reported.
#
# A BUILT failure on bus.resume.tail_* means the source still has the
# mmap->local restore branch instead of ORIG's local->mmap.
set -eu

HERE=$(cd "$(dirname "$0")" && pwd)
ROOT=$(cd "$HERE/../.." && pwd)
ORIG=${ORIG:-"$ROOT/reference/libtss_sdk.so.1.0.394.orig"}
BUILT=${BUILT:-"$ROOT/build-server/libtss_sdk.so.1.0.394"}
CXX=${CXX:-g++}
OUT=${OUT:-"$HERE/build"}

mkdir -p "$OUT"
"$CXX" -m32 -std=gnu++98 -O2 -Wall -Wextra -rdynamic \
    -o "$OUT/startup_ipc_probe" \
    "$HERE/startup_ipc_probe.cpp" -ldl -lrt -pthread

set +e
"$OUT/startup_ipc_probe" --lib "$ORIG" > "$OUT/orig.json"
ORIG_RC=$?
"$OUT/startup_ipc_probe" --lib "$BUILT" > "$OUT/rebuilt.json"
BUILT_RC=$?
set -e

printf '%s\n' "orig:    $OUT/orig.json (rc=$ORIG_RC)"
printf '%s\n' "rebuilt: $OUT/rebuilt.json (rc=$BUILT_RC)"
exit $((ORIG_RC || BUILT_RC))
