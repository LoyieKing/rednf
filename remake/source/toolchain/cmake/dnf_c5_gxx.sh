#!/bin/sh
# c5 变体：c5root gcc-4.1.2 驱动。
ROOT="${DNF_TC_ROOT:-$(cd "$(dirname "$0")/../../.." && pwd)/toolchains}"
export LD_LIBRARY_PATH="${ROOT}/c5root/usr/lib64:${ROOT}/c5root/usr/lib"
exec "${ROOT}/c5root/usr/bin/g++" "$@"
