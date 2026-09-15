#!/bin/sh
# c412 变体：c5r52tool (4.1.2) 驱动 + 精确 4.1.2-52 cc1plus（CentOS 5.8 os 快照，
# 即 ORIG .comment 中的 "Red Hat 4.1.2-52"）。
# 由 dnf_dbmw_c5_gxx.sh 同源派生：剥离 4.4/c6 include，换 c5 gcc 内建头 + c5root C++ 头，
# -std=gnu++0x → gnu++98（4.1.2 无 C++0x 模式）。
ROOT="${DNF_TC_ROOT:-$(cd "$(dirname "$0")/../../.." && pwd)/toolchains}"
export LD_LIBRARY_PATH="${ROOT}/c5r52tool/usr/lib64:${ROOT}/c5r52tool/usr/lib:${ROOT}/c5root/usr/lib64:${ROOT}/c5root/usr/lib"

C5_GCC="${ROOT}/c5r52tool/usr/lib/gcc/x86_64-redhat-linux/4.1.2/include"
C5_CXX="${ROOT}/c5root/usr/include/c++/4.1.2"
C5_CXX_ARCH="${ROOT}/c5root/usr/include/c++/4.1.2/x86_64-redhat-linux"
C5_CXX_BACK="${ROOT}/c5root/usr/include/c++/4.1.2/backward"
C5_SYS="${ROOT}/c5root/usr/include"

new_args=
for arg in "$@"; do
    case "$arg" in
        -isystem${ROOT}/lsd44/*|-isystem${ROOT}/c6root/*|-isystem${ROOT}/c5root/*)
            ;;
        -nostdinc)
            ;;
        -std=gnu++0x)
            new_args="$new_args -std=gnu++98"
            ;;
        -B${ROOT}/cc1plus446bin/*|-B${ROOT}/cc1plus444bin/*|-B${ROOT}/cc1plus412bin/*)
            ;;
        *)
            new_args="$new_args $arg"
            ;;
    esac
done

exec "${ROOT}/c5r52tool/usr/bin/g++" \
    -m32 -O0 -D_GNU_SOURCE -std=gnu++98 -nostdinc \
    "-Dstatic_assert(x,y)=" \
    -isystem "$C5_GCC" \
    -isystem "$C5_CXX" \
    -isystem "$C5_CXX_ARCH" \
    -isystem "$C5_CXX_BACK" \
    -isystem "$C5_SYS" \
    -B "${ROOT}/cc1plus412bin/" \
    $new_args
