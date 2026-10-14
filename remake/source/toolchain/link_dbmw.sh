#!/bin/sh
# df_dbmw_r 专用链接包装（2026-08-11）
# 目标：让 libstdc++.a(4.4.4) 的 eh_globals.o 在 StackBuffer.o 之前被提取，
# 使 .tbss 顺序与 ORIG 一致：
#   gNumberToStringBuffer@0（DNFFunctionLib.o）
#   → eh_global@0x200（libstdc++ eh_globals.o，8 字节）
#   → g_stackBufferContext@0x208（StackBuffer.o，GS-0x4）
# 实现：在 DNFFunctionLib.cpp.o 之后插入 libstdc++.a(4.4.4) 一次；
# 末尾原有的 libstdc++ 归档照旧（已定义的符号不会再被提取）。
set -e

# [2026-09-14 修复] 原为宿主 g++ ⇒ 产物要求 GLIBC_2.34，在 CentOS 7(glibc 2.17) 里跑不起来
#   （ORIG 只要 GLIBC_2.4）。改用 c6root 4.4.7 驱动 + --sysroot 显式 crt/运行时
#   （与 toolchain/cmake/dnf_game_link.sh 同一套）。
_SELF_DIR="$(cd "$(dirname "$0")" && pwd)"
C6ROOT="${DNF_TC_ROOT:-$_SELF_DIR/../../../toolchains}/c6root"
CXX="${C6ROOT_DRIVER:-$C6ROOT/usr/bin/g++}"
G32="$C6ROOT/usr/lib/gcc/x86_64-redhat-linux/4.4.4/32"
export GCC_EXEC_PREFIX="$C6ROOT/usr/libexec/gcc/"
export LD_LIBRARY_PATH="$C6ROOT/usr/lib64:$C6ROOT/usr/lib"
DRIVER_OPTS="-m32 --sysroot=$C6ROOT -B$G32 -L$C6ROOT/usr/lib -L$C6ROOT/lib -nostdlib"
C6ROOT="$(cd "$(dirname "$0")/../../../toolchains" && pwd)/c6root"
ARCHIVE="$C6ROOT/usr/lib/gcc/x86_64-redhat-linux/4.4.4/32/libstdc++.a"

args=""
inserted=0
for a in "$@"; do
    args="$args $a"
    case "$a" in
    *DNFFunctionLib.cpp.o)
        if [ "$inserted" -eq 0 ]; then
            args="$args $ARCHIVE"
            inserted=1
        fi
        ;;
    esac
done

PRE="$C6ROOT/usr/lib/crt1.o $C6ROOT/usr/lib/crti.o $G32/crtbegin.o"
POST="$G32/crtend.o $C6ROOT/usr/lib/crtn.o"
exec $CXX $DRIVER_OPTS $PRE $args $POST -lgcc -lc -lgcc -lgcc_eh
