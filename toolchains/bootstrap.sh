#!/usr/bin/env bash
# =====================================================================
# rednf 工具链从零准备脚本
# ---------------------------------------------------------------------
# remake 的构建依赖若干 CentOS 5/6 时代的 GCC 4.x 工具链与 32 位库。
# 这些都是第三方二进制，不入库；本脚本从 CentOS Vault 下载官方 RPM、
# 解包到 toolchains/ 下，并补齐少量链接用的库：
#
#   c6root/        CentOS 6.10 gcc-4.4.7-23 + cpp（x86_64 + i686）  ← 现代路线三方库也用它
#   c5root/        CentOS 5.11 gcc-4.1.2-55（x86_64 + i386）
#   c5r52tool/     CentOS 5.2  gcc-4.1.2-42 x86_64
#   c5r52i386/     CentOS 5.2  gcc-4.1.2-42 i386
#   cc1plus446bin/ CentOS 6.2  gcc-c++-4.4.6-3 的 cc1plus
#   cc1plus444bin/ CentOS 6.0  gcc-c++-4.4.4-13 的 cc1plus
#   lsd44/v4.4.6   CentOS 6.2  libstdc++-devel-4.4.6（头文件）
#   lsd44/v4.4.4   CentOS 6.0  libstdc++-devel-4.4.4 i686
#   zlib32/        CentOS 5.11 zlib-devel i386（libz.a + 头文件）
#   libcrypto32/   Debian i386 OpenSSL 3 libcrypto.a + 链接 shim（df_game_r 用）
#   nxencryption/  厂商 libnxencryption.so（从 .origin_1031 子模块复制）
#   bin/ toolchain.env  包装脚本与环境变量
#
# 依赖：bash curl(or wget) rpm2cpio cpio gcc；libcrypto32 还需要 dpkg-deb
#       （Debian/Ubuntu 自带；或自行设置 LIBCRYPTO32_A=/path/to/i386/libcrypto.a）。
# 镜像：默认 https://vault.centos.org；国内可用
#       DNF_MIRROR=https://mirrors.tuna.tsinghua.edu.cn/centos-vault
# 用法：bash toolchains/bootstrap.sh        （可重复执行，已完成的步骤会跳过）
# =====================================================================
set -euo pipefail

TC_ROOT="${DNF_TC_ROOT:-$(cd "$(dirname "$0")" && pwd)}"
REPO_ROOT="$(cd "$TC_ROOT/.." && pwd)"
MIRROR="${DNF_MIRROR:-https://vault.centos.org}"
RPM_DIR="$TC_ROOT/_rpm_cache"
mkdir -p "$RPM_DIR"

say() { printf '\033[1;34m[bootstrap]\033[0m %s\n' "$*"; }
die() { printf '\033[1;31m[bootstrap] ERROR:\033[0m %s\n' "$*" >&2; exit 1; }
for c in rpm2cpio cpio; do command -v $c >/dev/null || die "需要 $c（apt install rpm2cpio cpio）"; done
if command -v curl >/dev/null; then fetch() { curl -fsSL --retry 3 -o "$1" "$2"; }
elif command -v wget >/dev/null; then fetch() { wget -q -O "$1" "$2"; }
else die "需要 curl 或 wget"; fi

# extract <rpm> <dest-root>
extract() { mkdir -p "$2"; (cd "$2" && rpm2cpio "$1" | cpio -idm --quiet --no-absolute-filenames); }
# dl <rel> <arch> <subdir:Packages|CentOS> <pkg> <root>
dl() {
    local rel="$1" arch="$2" sub="$3" pkg="$4" root="$5"
    local rpm="$RPM_DIR/$pkg"
    if [ ! -s "$rpm" ]; then
        say "下载 $rel/$arch/$pkg"
        fetch "$rpm.part" "$MIRROR/$rel/os/$arch/$sub/$pkg" || die "下载失败: $MIRROR/$rel/os/$arch/$sub/$pkg"
        mv "$rpm.part" "$rpm"
    fi
    extract "$rpm" "$TC_ROOT/$root"
}
done_mark() { grep -qx "$2" "$TC_ROOT/$1/.bootstrap_done" 2>/dev/null; }
mark()      { mkdir -p "$TC_ROOT/$1"; echo "$2" >> "$TC_ROOT/$1/.bootstrap_done"; }
pkgset() { # pkgset <root> <rel> <arch> <sub> pkgs...
    local root="$1" rel="$2" arch="$3" sub="$4"; shift 4
    for p in "$@"; do done_mark "$root" "$p" || { dl "$rel" "$arch" "$sub" "$p" "$root"; mark "$root" "$p"; }; done
}

say "[1/9] c6root (CentOS 6.10 gcc-4.4.7-23)"
pkgset c6root 6.10 x86_64 Packages \
    gcc-4.4.7-23.el6.x86_64.rpm gcc-c%2B%2B-4.4.7-23.el6.x86_64.rpm cpp-4.4.7-23.el6.x86_64.rpm \
    libgcc-4.4.7-23.el6.x86_64.rpm libstdc%2B%2B-4.4.7-23.el6.x86_64.rpm \
    libstdc%2B%2B-devel-4.4.7-23.el6.x86_64.rpm glibc-2.12-1.212.el6.x86_64.rpm \
    glibc-devel-2.12-1.212.el6.x86_64.rpm glibc-headers-2.12-1.212.el6.x86_64.rpm \
    kernel-headers-2.6.32-754.el6.x86_64.rpm mpfr-2.4.1-6.el6.x86_64.rpm gmp-4.3.1-13.el6.x86_64.rpm
pkgset c6root 6.10 i386 Packages \
    libgcc-4.4.7-23.el6.i686.rpm libstdc%2B%2B-4.4.7-23.el6.i686.rpm \
    libstdc%2B%2B-devel-4.4.7-23.el6.i686.rpm glibc-2.12-1.212.el6.i686.rpm glibc-devel-2.12-1.212.el6.i686.rpm
# CentOS 6 的 4.4.7 包把头文件/库目录放在 4.4.4 下，并以 4.4.7 -> 4.4.4 链接
C6INC="$TC_ROOT/c6root/usr/include/c++/4.4.4"
[ -f "$C6INC/backward_warning.h" ] || cp "$C6INC/backward/backward_warning.h" "$C6INC/backward_warning.h"

say "[2/9] c5root (CentOS 5.11 gcc-4.1.2-55)"
pkgset c5root 5.11 x86_64 CentOS \
    gcc-4.1.2-55.el5.x86_64.rpm gcc-c%2B%2B-4.1.2-55.el5.x86_64.rpm libgcc-4.1.2-55.el5.x86_64.rpm \
    libstdc%2B%2B-4.1.2-55.el5.x86_64.rpm libstdc%2B%2B-devel-4.1.2-55.el5.x86_64.rpm \
    glibc-devel-2.5-123.x86_64.rpm glibc-headers-2.5-123.x86_64.rpm kernel-headers-2.6.18-398.el5.x86_64.rpm
pkgset c5root 5.11 i386 CentOS \
    libgcc-4.1.2-55.el5.i386.rpm libstdc%2B%2B-devel-4.1.2-55.el5.i386.rpm glibc-devel-2.5-123.i386.rpm

say "[3/9] c5r52tool / c5r52i386 (CentOS 5.2 gcc-4.1.2-42)"
pkgset c5r52tool 5.2 x86_64 CentOS \
    gcc-4.1.2-42.el5.x86_64.rpm gcc-c%2B%2B-4.1.2-42.el5.x86_64.rpm libgcc-4.1.2-42.el5.x86_64.rpm \
    libstdc%2B%2B-4.1.2-42.el5.x86_64.rpm libstdc%2B%2B-devel-4.1.2-42.el5.x86_64.rpm
pkgset c5r52i386 5.2 i386 CentOS \
    gcc-4.1.2-42.el5.i386.rpm gcc-c%2B%2B-4.1.2-42.el5.i386.rpm libgcc-4.1.2-42.el5.i386.rpm \
    libstdc%2B%2B-4.1.2-42.el5.i386.rpm libstdc%2B%2B-devel-4.1.2-42.el5.i386.rpm

say "[4/9] cc1plus446bin (CentOS 6.2 gcc-c++-4.4.6-3)"
if [ ! -x "$TC_ROOT/cc1plus446bin/cc1plus" ]; then
    dl 6.2 x86_64 Packages gcc-c%2B%2B-4.4.6-3.el6.x86_64.rpm cc1plus446_src
    install -Dm755 "$TC_ROOT/cc1plus446_src/usr/libexec/gcc/x86_64-redhat-linux/4.4.6/cc1plus" "$TC_ROOT/cc1plus446bin/cc1plus"
fi
say "[5/9] cc1plus444bin (CentOS 6.0 gcc-c++-4.4.4-13)"
if [ ! -x "$TC_ROOT/cc1plus444bin/cc1plus" ]; then
    dl 6.0 x86_64 Packages gcc-c%2B%2B-4.4.4-13.el6.x86_64.rpm cc1plus444_src
    install -Dm755 "$TC_ROOT/cc1plus444_src/usr/libexec/gcc/x86_64-redhat-linux/4.4.4/cc1plus" "$TC_ROOT/cc1plus444bin/cc1plus"
fi

say "[6/9] lsd44 (libstdc++-devel 4.4.6 / 4.4.4 头文件)"
[ -d "$TC_ROOT/lsd44/v4.4.6/usr/include/c++/4.4.6" ] || dl 6.2 x86_64 Packages libstdc%2B%2B-devel-4.4.6-3.el6.x86_64.rpm lsd44/v4.4.6
[ -d "$TC_ROOT/lsd44/v4.4.4/usr/include/c++/4.4.4" ] || dl 6.0 i386 Packages libstdc%2B%2B-devel-4.4.4-13.el6.i686.rpm lsd44/v4.4.4

say "[7/9] zlib32 (CentOS 5.11 zlib-devel i386)"
if [ ! -f "$TC_ROOT/zlib32/lib/libz.a" ]; then
    dl 5.11 i386 CentOS zlib-devel-1.2.3-7.el5.i386.rpm zlib32_src
    install -Dm644 "$TC_ROOT/zlib32_src/usr/lib/libz.a" "$TC_ROOT/zlib32/lib/libz.a"
    cp "$TC_ROOT/zlib32_src/usr/include/zlib.h" "$TC_ROOT/zlib32_src/usr/include/zconf.h" "$TC_ROOT/zlib32/"
fi

C6ENV=(env GCC_EXEC_PREFIX="$TC_ROOT/c6root/usr/libexec/gcc/" LD_LIBRARY_PATH="$TC_ROOT/c6root/usr/lib64:$TC_ROOT/c6root/usr/lib")
say "[8/9] libcrypto32 (i386 OpenSSL 3 静态库 + shim)"
if [ ! -f "$TC_ROOT/libcrypto32/lib/libcrypto.a" ]; then
    mkdir -p "$TC_ROOT/libcrypto32/lib"
    if [ -n "${LIBCRYPTO32_A:-}" ]; then
        cp "$LIBCRYPTO32_A" "$TC_ROOT/libcrypto32/lib/libcrypto.a"
    elif [ -f /usr/lib/i386-linux-gnu/libcrypto.a ]; then
        cp /usr/lib/i386-linux-gnu/libcrypto.a "$TC_ROOT/libcrypto32/lib/libcrypto.a"
    else
        command -v dpkg-deb >/dev/null && command -v apt-get >/dev/null || die "需要 Debian/Ubuntu 的 apt-get+dpkg-deb，或设置 LIBCRYPTO32_A"
        say "apt-get download libssl-dev:i386（需 dpkg --add-architecture i386 && apt-get update）"
        (cd "$RPM_DIR" && rm -f libssl-dev_*_i386.deb && apt-get download libssl-dev:i386) || die "下载 libssl-dev:i386 失败"
        rm -rf "$RPM_DIR/libssl"; dpkg-deb -x "$RPM_DIR"/libssl-dev_*_i386.deb "$RPM_DIR/libssl"
        cp "$RPM_DIR/libssl/usr/lib/i386-linux-gnu/libcrypto.a" "$TC_ROOT/libcrypto32/lib/libcrypto.a"
    fi
fi
mkdir -p "$TC_ROOT/libcrypto32/stub"
"${C6ENV[@]}" "$TC_ROOT/c6root/usr/bin/gcc" -m32 -O2 -c "$TC_ROOT/src/openssl3_c6_shim.c" -o "$TC_ROOT/libcrypto32/stub/openssl3_c6_shim.o"

say "[9/9] nxencryption（来自 .origin_1031）"
NX="$REPO_ROOT/.origin_1031/build/dnf_data/home/template/neople/game/libnxencryption.so"
if [ -f "$NX" ]; then install -Dm755 "$NX" "$TC_ROOT/nxencryption/libnxencryption.so"
else say "警告：未找到 $NX（先 git submodule update --init .origin_1031），df_game_r 将无法链接"; fi

say "生成包装脚本与 toolchain.env"
mkdir -p "$TC_ROOT/bin"
cat > "$TC_ROOT/bin/c6-g++" <<W
#!/bin/sh
export GCC_EXEC_PREFIX="$TC_ROOT/c6root/usr/libexec/gcc/"
export LD_LIBRARY_PATH="$TC_ROOT/c6root/usr/lib64:$TC_ROOT/c6root/usr/lib"
exec "$TC_ROOT/c6root/usr/bin/g++" "\$@"
W
cat > "$TC_ROOT/bin/c5-g++" <<W
#!/bin/sh
export GCC_EXEC_PREFIX="$TC_ROOT/c5root/usr/libexec/gcc/"
export PATH="$TC_ROOT/c5root/usr/libexec/gcc/x86_64-redhat-linux/4.1.2:\$PATH"
exec "$TC_ROOT/c5root/usr/bin/g++" "\$@"
W
cat > "$TC_ROOT/bin/c6-g++-446r" <<W
#!/bin/sh
export LD_LIBRARY_PATH="$TC_ROOT/c6root/usr/lib64:$TC_ROOT/c6root/usr/lib"
exec "$TC_ROOT/c6root/usr/bin/g++" -B "$TC_ROOT/cc1plus446bin/" "\$@"
W
cat > "$TC_ROOT/bin/c6-g++-444r" <<W
#!/bin/sh
export LD_LIBRARY_PATH="$TC_ROOT/c6root/usr/lib64:$TC_ROOT/c6root/usr/lib"
exec "$TC_ROOT/c6root/usr/bin/g++" -B "$TC_ROOT/cc1plus444bin/" "\$@"
W
chmod +x "$TC_ROOT"/bin/*
cat > "$TC_ROOT/c5root/env-m32.sh" <<W
# gcc 4.1.2 -m32 环境（source 使用）
export GCC_EXEC_PREFIX=$TC_ROOT/c5root/usr/libexec/gcc/
export LD_LIBRARY_PATH=$TC_ROOT/c5root/usr/lib:$TC_ROOT/c5root/usr/lib/gcc/x86_64-redhat-linux/4.1.1/32
export PATH=$TC_ROOT/c5root/usr/libexec/gcc/x86_64-redhat-linux/4.1.1:\$PATH
export CXX=$TC_ROOT/c5root/usr/bin/g++
export CXXFLAGS="-m32 -O0 -fno-enforce-eh-specs"
W
cat > "$TC_ROOT/toolchain.env" <<W
export DNF_TC_ROOT="$TC_ROOT"
export C6ROOT="$TC_ROOT/c6root"
export C5ROOT="$TC_ROOT/c5root"
export C5R52TOOL="$TC_ROOT/c5r52tool"
export C5R52I386="$TC_ROOT/c5r52i386"
export C6LIBSTDCXX446="$TC_ROOT/lsd44/v4.4.6"
export ZLIB32="$TC_ROOT/zlib32"
export CC1PLUS446="$TC_ROOT/cc1plus446bin"
export PATH="$TC_ROOT/bin:\$PATH"
W

say "冒烟测试"
printf '#include <string>\n#include <cstdio>\nint main(){std::string s="ok";printf("%%s\\n",s.c_str());}\n' > "$RPM_DIR/_smoke.cpp"
"$TC_ROOT/bin/c6-g++" -m32 -O0 "$RPM_DIR/_smoke.cpp" -o "$RPM_DIR/_smoke6" 2>/dev/null && say "c6-g++ -m32 OK" || say "c6-g++ 独立链接冒烟失败（正常：remake 构建只用它编译三方库，不用它链接）"
"$TC_ROOT/bin/c5-g++" -m32 -O0 "$RPM_DIR/_smoke.cpp" -o "$RPM_DIR/_smoke5" 2>/dev/null && say "c5-g++ -m32 OK" || say "c5-g++ 冒烟失败（只影响 remake_verify=ON 路线）"
say "完成。下一步：cmake -S remake/source -B remake/build && cmake --build remake/build -j\$(nproc)"
