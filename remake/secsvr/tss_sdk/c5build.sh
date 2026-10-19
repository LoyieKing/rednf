#!/bin/bash
# Compile one recon TU with the vendor-matched c5root GCC 4.1.2 toolchain.
# Usage: c5build.sh <relative .cpp> <out .o> [extra flags...]
set -e
C5=/home/loyieking/dnf_workspace/dnf_remake/toolchains/c5root
export PATH="$C5/usr/bin:$PATH"
export LD_LIBRARY_PATH="$C5/usr/lib:$C5/lib:${LD_LIBRARY_PATH:-}"
cd "$(dirname "$0")"
src="$1"; out="$2"; shift 2
exec "$C5/usr/bin/g++" -m32 -O2 -fPIC -fno-implement-inlines -fno-stack-protector \
    -std=gnu++98 -I recon "$@" -c "$src" -o "$out"
