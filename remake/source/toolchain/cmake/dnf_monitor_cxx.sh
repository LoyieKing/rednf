#!/bin/sh
export GCC_EXEC_PREFIX="${DNF_TC_ROOT:-/home/loyieking/dnf_workspace/dnf_decompile/toolchains}/c6root/usr/libexec/gcc/"
export LD_LIBRARY_PATH="${DNF_TC_ROOT:-/home/loyieking/dnf_workspace/dnf_decompile/toolchains}/c6root/usr/lib64:${DNF_TC_ROOT:-/home/loyieking/dnf_workspace/dnf_decompile/toolchains}/c6root/usr/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
src=""
prev=""
for a in "$@"; do
    if [ "$prev" = "-c" ]; then
        src="$a"
        break
    fi
    prev="$a"
done
case "$(basename "$src" .cpp)" in
    DNFApplication|DNFSignalTranslator)
        exec /home/loyieking/dnf_workspace/dnf_decompile/toolchains/c6root/usr/bin/g++ -B /tmp/cc1plus446bin/ "$@" ;;
    *)
        exec /home/loyieking/dnf_workspace/dnf_decompile/toolchains/c6root/usr/bin/g++ "$@" ;;
esac
