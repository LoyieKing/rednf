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
# - DNFTableBase：c6444r 编译但去掉 -fno-enforce-eh-specs。R42 验证：
#   Load_Txt_Table_Data 虚调用（fn->%edx 形态）与 fopen 赋值条件
#   （(f=fopen())==0 → sete %al;test %al,%al;je）均与 ORIG 逐条一致；
#   Parse_Table 全族改为 bool 返回（ORIG 调用点 test %al 直接消费）。
#   原 c6 形态：fn->%eax + jne 折叠 + int→bool setne，无法对齐。
#   ORIG 该 TU 的
#   CDNFException what/D1/D2 带 __cxa_call_unexpected landing pad（throw()
#   异常规范强制代码），带该 flag 会丢失 pad 导致 DIFF；去掉后
#   what/D1/D2 全部 IDENTICAL_AE（第 40 轮 c6 验证；R42 在 c6444r 下复验无回归，
#   同 TU CTableBase 均为 MISSING_ORIG 不统计，无回归）
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
    DNFTableBase)
        filtered=""
        for a in "$@"; do
            if [ "$a" = "-fno-enforce-eh-specs" ]; then
                continue
            fi
            filtered="$filtered $a"
        done
        # shellcheck disable=SC2086
        exec "$C6444R" $filtered ;;
    DNFTcpHandler|DNFSignalTranslator|DNFApplication|DNFGuildServerMain|DNFPacketTranslater|DNFThreadInterface|DNFTcpAcceptThread|TcpNetSystem|DNFAppStopInit|DNFAppStartInit|DNFServerHandler|PowerWar)
        exec "$C6444R" "$@" ;;
    *)
        exec "$C6" "$@" ;;
esac
