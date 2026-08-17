#!/bin/sh
# c6444r 变体：c6root 4.4.7 驱动 + Red Hat 4.4.4-13 cc1plus（monitor ORIG .comment 主体编译器）。
# 等价旧 /tmp/c6-g++-444r；头文件用 /tmp/lsd44/v4.4.4（dnf_helpers 按变体分发）。
ROOT="${DNF_TC_ROOT:-$(cd "$(dirname "$0")/../../.." && pwd)/toolchains}"
export LD_LIBRARY_PATH="${ROOT}/c6root/usr/lib64:${ROOT}/c6root/usr/lib"
for arg in "$@"; do
    if [ "$arg" = "-D_DNF_DBMW_C5_TU" ]; then
        exec "$(dirname "$0")/dnf_dbmw_c5_gxx.sh" "$@"
    fi
done
if [ -x "${ROOT}/cc1plus444bin/cc1plus" ]; then
    exec "${ROOT}/c6root/usr/bin/g++" -B "${ROOT}/cc1plus444bin/" "$@"
fi
exec "${ROOT}/c6root/usr/bin/g++" "$@"
