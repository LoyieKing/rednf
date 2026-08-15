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

CXX="/usr/bin/g++"
ARCHIVE="/tmp/c6root/usr/lib/gcc/x86_64-redhat-linux/4.4.4/32/libstdc++.a"

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

exec $CXX $args
