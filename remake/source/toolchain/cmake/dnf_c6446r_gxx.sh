#!/bin/sh
# c6446r 变体：c6root 4.4.7 驱动 + 4.4.6 cc1plus（auction/point/channel/bridge 精确对齐）。
# 等价旧 /tmp/c6-g++-444r；cc1plus446bin 与 cc1plus444bin 任一存在即可。
ROOT="${DNF_TC_ROOT:-/tmp}"
export LD_LIBRARY_PATH="${ROOT}/c6root/usr/lib64:${ROOT}/c6root/usr/lib"
if [ -x "${ROOT}/cc1plus446bin/cc1plus" ]; then
    exec "${ROOT}/c6root/usr/bin/g++" -B "${ROOT}/cc1plus446bin/" "$@"
elif [ -x "${ROOT}/cc1plus444bin/cc1plus" ]; then
    exec "${ROOT}/c6root/usr/bin/g++" -B "${ROOT}/cc1plus444bin/" "$@"
fi
exec "${ROOT}/c6root/usr/bin/g++" "$@"
