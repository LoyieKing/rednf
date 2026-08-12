#!/bin/sh
# guild 逐 TU 编译器分发（2026-08-11 第 9 轮集成用）：
# - 默认 c6（c6root 4.4.7 纯驱动），与 ORIG 4.4.7-1 TU 对齐
# - 以下 5 个 TU 保留 c6444r（4.4.7 驱动 + 4.4.4-13 cc1plus）：
#   A/B 全量实测 c6 会使这些 TU 的已 identical 函数回归为 NEAR
#   （DNFTcpHandler 9、DNFSignalTranslator 2、DNFApplication 1、main 1）
#   DNFPacketTranslater：ORIG .comment 以 4.4.4-13 为主（8 项），该 TU 的
#   e.what() 虚调用寄存器形态（fn->%edx）与 c6444r 逐条一致（第 14 轮验证）
#   DNFTcpAcceptThread：CTcpAcceptThread::dispatch 的 e.what() 虚调用寄存器
#   形态同样与 c6444r 逐条一致（第 23 轮验证，NEAR→IDENTICAL_AE，同 TU 其余
#   6 函数 c6/c6444r 均 identical）
#   TcpNetSystem：CTcpNetSystem D1/D2 的虚调用（fn->%edx 形态）与 c6444r
#   逐条一致（第 30 轮验证，NEAR→IDENTICAL；同 TU 其余函数 c6/c6444r 均
#   identical，含 Init 在 begin() 返回 bool 后 IDENTICAL）
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
    DNFTcpHandler|DNFSignalTranslator|DNFApplication|DNFGuildServerMain|DNFPacketTranslater|DNFThreadInterface|DNFTcpAcceptThread|TcpNetSystem|DNFAppStopInit|DNFAppStartInit|DNFServerHandler|PowerWar)
        exec "$C6444R" "$@" ;;
    *)
        exec "$C6" "$@" ;;
esac
