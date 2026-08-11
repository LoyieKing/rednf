#!/bin/sh
# guild 逐 TU 编译器分发（2026-08-11 第 9 轮集成用）：
# - 默认 c6（c6root 4.4.7 纯驱动），与 ORIG 4.4.7-1 TU 对齐
# - 以下 4 个 TU 保留 c6444r（4.4.7 驱动 + 4.4.4-13 cc1plus）：
#   A/B 全量实测 c6 会使这 4 个 TU 的 13 个已 identical 函数回归为 NEAR
#   （DNFTcpHandler 9、DNFSignalTranslator 2、DNFApplication 1、main 1）
ROOT=/home/loyieking/dnf_workspace/dnf_decompile/source/toolchain/cmake
C6="$ROOT/dnf_c6_gxx.sh"
C6444R="$ROOT/dnf_c6444r_gxx.sh"
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
    DNFTcpHandler|DNFSignalTranslator|DNFApplication|DNFGuildServerMain)
        exec "$C6444R" "$@" ;;
    *)
        exec "$C6" "$@" ;;
esac
