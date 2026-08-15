#!/bin/sh
# dbmw 专用 4.1.2-52 逐 TU 包装器：
# 由 dnf_c6444r_gxx.sh 在命令行包含 -D_DNF_DBMW_C5_TU 时转调本脚本。
# 原 dbmw 全局 flags 是 4.4.4 头文件 + 4.4.7 gcc 内建头；本脚本把这些
# 4.4/c6 include 全部剥掉，换成 c5r52tool(4.1.2) gcc 内建头 + c5root C++ 头。

ROOT="${DNF_TC_ROOT:-/tmp}"
export LD_LIBRARY_PATH="${ROOT}/c5r52tool/usr/lib64:${ROOT}/c5r52tool/usr/lib:${ROOT}/c5root/usr/lib64:${ROOT}/c5root/usr/lib"

C5_GCC="${ROOT}/c5r52tool/usr/lib/gcc/x86_64-redhat-linux/4.1.2/include"
C5_CXX="${ROOT}/c5root/usr/include/c++/4.1.2"
C5_CXX_ARCH="${ROOT}/c5root/usr/include/c++/4.1.2/x86_64-redhat-linux"
C5_CXX_BACK="${ROOT}/c5root/usr/include/c++/4.1.2/backward"
C5_SYS="${ROOT}/c5root/usr/include"

# 过滤 4.4/c6 头文件与 c5root 旧 include（后面统一补 c5 include），并替换 std。
new_args=
for arg in "$@"; do
    case "$arg" in
        -isystem${ROOT}/lsd44/v4.4.4/*|-isystem${ROOT}/lsd44/v4.4.6/*|-isystem${ROOT}/c6root/*|-isystem${ROOT}/c5root/*)
            ;;
        -nostdinc)
            ;;
        -std=gnu++0x)
            new_args="$new_args -std=gnu++98"
            ;;
        -B${ROOT}/cc1plus446bin/*|-B${ROOT}/cc1plus444bin/*)
            ;;
        *)
            new_args="$new_args $arg"
            ;;
    esac
done

exec "${ROOT}/c5r52tool/usr/bin/g++" \
    -m32 -O0 -D_GNU_SOURCE -std=gnu++98 -nostdinc \
    -isystem "$C5_GCC" \
    -isystem "$C5_CXX" \
    -isystem "$C5_CXX_ARCH" \
    -isystem "$C5_CXX_BACK" \
    -isystem "$C5_SYS" \
    $new_args
