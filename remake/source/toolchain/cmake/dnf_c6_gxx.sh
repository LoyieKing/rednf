#!/bin/sh
# c6 变体（默认）：c6root gcc-4.4.7 驱动。
# 内部导出 LD_LIBRARY_PATH 供 cc1plus 使用——不能在工具链文件里 set(ENV{...})，
# 那会污染宿主 cmake/其它 C++ 工具（GLIBCXX 版本不兼容崩溃）。
ROOT="${DNF_TC_ROOT:-$(cd "$(dirname "$0")/../../.." && pwd)/toolchains}"
export LD_LIBRARY_PATH="${ROOT}/c6root/usr/lib64:${ROOT}/c6root/usr/lib"
exec "${ROOT}/c6root/usr/bin/g++" "$@"
