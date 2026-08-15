#!/bin/sh
export GCC_EXEC_PREFIX="${DNF_TC_ROOT:-/home/loyieking/dnf_workspace/dnf_decompile/toolchains}/c6root/usr/libexec/gcc/"
export LD_LIBRARY_PATH="${DNF_TC_ROOT:-/home/loyieking/dnf_workspace/dnf_decompile/toolchains}/c6root/usr/lib64:${DNF_TC_ROOT:-/home/loyieking/dnf_workspace/dnf_decompile/toolchains}/c6root/usr/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
exec /home/loyieking/dnf_workspace/dnf_decompile/toolchains/c6root/usr/bin/g++ -B /tmp/cc1plus446bin/ "$@"
