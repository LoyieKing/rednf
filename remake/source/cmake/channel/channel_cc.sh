#!/bin/sh
# channel 专用编译包装：复现 ORIG 构建的 __FILE__ 字符串（basename）。
# ORIG 从源目录以相对路径编译，__FILE__ 展开为 "ScriptRawData.cpp"、
# "GlobalInstance.h" 等短名；CMake 默认传绝对路径，导致 assert/日志
# 的文件名实参两侧不一致。本包装把 "-c /abs/path/TU.cpp" 改写为
# "cd 到源目录 + -c TU.cpp"，并把 -o/-MF/-MT 保持为绝对路径。
ORIG_PWD=$(pwd)
CFILE=""
ARGS=""
PREV=""
for a in "$@"; do
    case "$a" in
    -c|-o|-MF|-MT)
        PREV="$a"
        ARGS="$ARGS $a"
        continue
        ;;
    esac
    case "$PREV" in
    -c)
        CFILE="$a"
        ARGS="$ARGS $a"
        ;;
    -o|-MF|-MT)
        case "$a" in
        /*) ARGS="$ARGS $a" ;;
        *)  ARGS="$ARGS $ORIG_PWD/$a" ;;
        esac
        ;;
    *)
        ARGS="$ARGS $a"
        ;;
    esac
    PREV="$a"
done
if [ -n "$CFILE" ]; then
    CDIR=$(CDPATH= cd -- "$(dirname "$CFILE")" && pwd)
    CBASE=$(basename "$CFILE")
    ARGS=$(printf '%s' "$ARGS" | sed "s# -c $CFILE# -c $CBASE#")
    cd "$CDIR" || exit 1
fi
exec /home/loyieking/dnf_workspace/dnf_decompile/source/toolchain/cmake/dnf_c6446r_gxx.sh $ARGS
