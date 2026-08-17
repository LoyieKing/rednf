#!/bin/sh
ROOT="${DNF_TC_ROOT:-$(cd "$(dirname "$0")/../../.." && pwd)/toolchains}"
export GCC_EXEC_PREFIX="${ROOT}/c6root/usr/libexec/gcc/"
export LD_LIBRARY_PATH="${ROOT}/c6root/usr/lib64:${ROOT}/c6root/usr/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
exec "${ROOT}/c6root/usr/bin/g++" -B "${ROOT}/cc1plus446bin/" "$@"
