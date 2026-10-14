#!/bin/sh
# ============================================================
# df_game_r 32 位链接驱动（2026-08-17）
# 宿主 g++ 无 32 位 multilib（gcc 11 缺 crt1.o）；c6root(4.4.7) 自带的 32 位
# 运行库位于 c6root/usr/lib，但驱动默认在宿主 /lib /usr/lib 找不到 32 位 libc，
# 且 -m32 的 crtbegin/crtend 只在 c6root 的 4.4.4/32 gcc 目录。
# 本包装以 c6root g++ 为驱动，手动补 --sysroot/-B/-L 与 crt 对象，产出
# 与 ORIG（GCC 4.4.x）对齐的 32 位静态 ELF。
# 由 source/cmake/game/CMakeLists.txt 的 CMAKE_CXX_LINK_EXECUTABLE 调用。
# 用法（CMake 展开后）：
#   dnf_game_link.sh <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LIBRARIES>
# ============================================================
ROOT="$(cd "$(dirname "$0")/../../.." && pwd)"   # .../dnf_remake
TOOLCHAIN="${DNF_TC_ROOT:-$ROOT/../toolchains}"
C6="$TOOLCHAIN/c6root"
G32="$C6/usr/lib/gcc/x86_64-redhat-linux/4.4.4/32"

export GCC_EXEC_PREFIX="${C6}/usr/libexec/gcc/"
export LD_LIBRARY_PATH="${C6}/usr/lib64:${C6}/usr/lib"

# 参数原样保留（cmake 传 <FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LIBRARIES>）
BITS=32
for a in "$@"; do [ "$a" = "-m64" ] && BITS=64; done
args=""
for a in "$@"; do
    # 请求 64 位时丢掉全局 CMAKE_CXX_FLAGS 里的 -m32（否则后者在后、覆盖 -m64，
    # 出现 `i386:x86-64 architecture ... incompatible with i386 output`）
    if [ "$BITS" = "64" ] && [ "$a" = "-m32" ]; then continue; fi
    args="$args $a"
done

# [2026-09-14] 兼作 64 位服务（stun）的链接驱动：c6root 同时带 64 位 EL6 根
#   （usr/lib64/crt1.o + lib64/libc.so.6 = glibc 2.12）。宿主 g++ 会把产物打到宿主
#   glibc 2.39（GLIBC_2.34），在 CentOS 7(2.17) 里跑不起来；用 c6root 驱动则 ≤2.12。
if [ "$BITS" = "64" ]; then
    G64="$C6/usr/lib/gcc/x86_64-redhat-linux/4.4.7"
    PRE64="$C6/usr/lib64/crt1.o $C6/usr/lib64/crti.o $G64/crtbegin.o"
    POST64="$G64/crtend.o $C6/usr/lib64/crtn.o"
    exec "${C6}/usr/bin/g++" --sysroot="$C6" -B"$G64" \
        -L"$C6/usr/lib64" -L"$C6/lib64" -nostdlib \
        $PRE64 $args $POST64 -lgcc -lc -lgcc
fi

# crt 前缀 / 后缀。
PRE="$C6/usr/lib/crt1.o $C6/usr/lib/crti.o $G32/crtbegin.o"
POST="$G32/crtend.o $C6/usr/lib/crtn.o"

# 链接顺序：crt1/crti/crtbegin → [全部对象与归档] → crtend/crtn，
# 末尾以 -lgcc -lc -lgcc -lgcc_eh 收口（-nostdlib 下 libc/gcc 运行时需显式给出）。
# -o <TARGET> 由 $args 提供。
# shellcheck disable=SC2086
exec "${C6}/usr/bin/g++" -m32 --sysroot="$C6" -B"$G32" \
    -L"$C6/usr/lib" -L"$C6/lib" -nostdlib \
    $PRE $args $POST -lgcc -lc -lgcc -lgcc_eh