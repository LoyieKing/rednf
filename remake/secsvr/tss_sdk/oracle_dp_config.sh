#!/bin/bash
# Independent differential oracle for the reconstructed dp_config.cpp.
#
#   ./oracle_dp_config.sh [--keep]
#
# Builds recon/src/closing/test_dp_config_oracle.cpp (test_* is excluded from
# build_server.py), then runs it against
#   reference/libtss_sdk.so.1.0.394.orig       (ORIG, read-only)
#   build-server/libtss_sdk.so.1.0.394         (reconstructed library)
#
# "pure" mode never opens the ORIG's fixed /tmp/dp_comm_config* paths.  "full"
# mode drives init_dp_config()/reload_dp_config() and is only executed inside a
# private user+mount namespace whose /tmp is a fresh tmpfs; it refuses to run
# without DPCFG_ORACLE_ISOLATED=1.  The live shared configuration and the ORIG
# library are never modified.
#
# The oracle does not build libtss_sdk itself; build/serve that first with
# build_server.py.  Nothing here is part of the production build.

set -u

ROOT=$(cd "$(dirname "$0")" && pwd)
cd "$ROOT" || exit 2

ORIG_LIB=${ORIG_LIB:-$ROOT/reference/libtss_sdk.so.1.0.394.orig}
RECON_LIB=${RECON_LIB:-$ROOT/build-server/libtss_sdk.so.1.0.394}
CXX=${CXX:-g++}
OUTDIR=${OUTDIR:-$ROOT/build/dpcfg_oracle}
KEEP=0
[ "${1:-}" = "--keep" ] && KEEP=1

for f in "$ORIG_LIB" "$RECON_LIB"; do
    if [ ! -f "$f" ]; then
        echo "MISSING_LIB $f" >&2
        exit 2
    fi
done

mkdir -p "$OUTDIR" || exit 2
ORIG_LIB=$(readlink -f "$ORIG_LIB")
RECON_LIB=$(readlink -f "$RECON_LIB")

CONFIG_INC=(-I "$ROOT/recon" -I "$ROOT/recon/include"
            -I "$ROOT/../source/secagent")
CXXFLAGS=(-m32 -std=gnu++98 -O2 -fPIC -D_GLIBCXX_USE_CXX11_ABI=0
          "${CONFIG_INC[@]}")
ORACLE=$OUTDIR/dpcfg_oracle

echo "ORIG_LIB=$ORIG_LIB"
echo "RECON_LIB=$RECON_LIB"
sha256sum "$ORIG_LIB" "$RECON_LIB" 2>/dev/null || true

# ---------------------------------------------------------------------------
# Build the harness (test-only; no production object is produced).
# ---------------------------------------------------------------------------
"$CXX" "${CXXFLAGS[@]}" -c \
    recon/src/closing/test_dp_config_oracle.cpp \
    -o "$OUTDIR/test_dp_config_oracle.o" || exit 2
"$CXX" -m32 "$OUTDIR/test_dp_config_oracle.o" -ldl -o "$ORACLE" || exit 2

# ---------------------------------------------------------------------------
# Run one mode against one library, keep only the deterministic RESULT lines.
# ---------------------------------------------------------------------------
run_one() {
    local lib=$1 mode=$2 out=$3
    "$ORACLE" --lib "$lib" --mode "$mode" >"$out" 2>"$out.err"
    local rc=$?
    if [ $rc -ne 0 ]; then
        echo "RUN_FAIL mode=$mode lib=$lib rc=$rc" >&2
        sed -n '1,40p' "$out.err" >&2
        return 1
    fi
    grep '^RESULT ' "$out" >"$out.result"
    return 0
}

compare() {
    local mode=$1 orig=$2 recon=$3
    if diff -u "$orig" "$recon" >"$OUTDIR/$mode.diff"; then
        echo "$mode: PASS ($(wc -l < "$orig") result lines)"
        return 0
    fi
    echo "$mode: DIFF"
    sed -n '1,120p' "$OUTDIR/$mode.diff"
    return 1
}

FAIL=0

# ---------------------------------------------------------------------------
# pure mode: no live shared memory.
# ---------------------------------------------------------------------------
if run_one "$ORIG_LIB" pure "$OUTDIR/orig.pure" &&
   run_one "$RECON_LIB" pure "$OUTDIR/recon.pure"; then
    compare pure "$OUTDIR/orig.pure.result" "$OUTDIR/recon.pure.result" || FAIL=1
else
    FAIL=1
fi

# ---------------------------------------------------------------------------
# full mode: private /tmp only.
# ---------------------------------------------------------------------------
detect_ns() {
    if unshare -Urm --propagation private true 2>/dev/null; then
        NS=(unshare -Urm --propagation private)
        return 0
    fi
    if command -v sudo >/dev/null 2>&1 &&
       sudo -n unshare -Urm --propagation private true 2>/dev/null; then
        NS=(sudo -n unshare -Urm --propagation private)
        return 0
    fi
    return 1
}

if detect_ns; then
    run_full_one() {
        local lib=$1 out=$2
        "${NS[@]}" sh -c \
            'mount -t tmpfs -o size=32m tmpfs /tmp && DPCFG_ORACLE_ISOLATED=1 "$1" --lib "$2" --mode full' \
            sh "$ORACLE" "$lib" >"$out" 2>"$out.err"
        local rc=$?
        if [ $rc -ne 0 ]; then
            echo "RUN_FAIL mode=full lib=$lib rc=$rc" >&2
            sed -n '1,40p' "$out.err" >&2
            return 1
        fi
        grep '^RESULT ' "$out" >"$out.result"
        return 0
    }
    if run_full_one "$ORIG_LIB" "$OUTDIR/orig.full" &&
       run_full_one "$RECON_LIB" "$OUTDIR/recon.full"; then
        compare full "$OUTDIR/orig.full.result" "$OUTDIR/recon.full.result" || FAIL=1
    else
        FAIL=1
    fi
else
    echo "full: SKIPPED (no unshare -Urm --propagation private / sudo -n ...)." >&2
    echo "full: run this script as root or grant a user+mount namespace;" >&2
    echo "full: refusing to touch /tmp/dp_comm_config* without isolation." >&2
    FAIL=1
fi

if [ $KEEP -eq 0 ]; then
    rm -f "$OUTDIR"/orig.pure.err "$OUTDIR"/recon.pure.err \
          "$OUTDIR"/orig.full.err "$OUTDIR"/recon.full.err
fi

if [ $FAIL -ne 0 ]; then
    echo "DP_CONFIG_ORACLE=FAIL"
    exit 1
fi
echo "DP_CONFIG_ORACLE=PASS"
exit 0
