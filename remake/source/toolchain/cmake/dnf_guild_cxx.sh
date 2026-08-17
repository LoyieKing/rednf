#!/bin/sh
# guild 逐 TU 编译器分发（2026-08-11 起调优，2026-08-15 修正）：
# - 默认 c6（c6root 4.4.7 纯驱动），与 ORIG 4.4.7-1 TU 对齐
# - 2026-08-15 实测修正：catch 块 e.what() 虚调用寄存器形态（fn->%edx）
#   由 **c6446r（4.4.6-3 cc1plus）** 产出，c6444r/c6 均产出 fn->%eax。
#   把含 catch 的 TU（DNFPacketTranslater/DNFGuild/GuildCargo/GuildBoard/
#   PowerManager/DNFPacketDecoder/DNFApplication/DNFGuildServerMain/
#   DNFNetworkThread/DNFSignalTranslator/DNFTcpAcceptThread/
#   DNFTcpNetworkThread/Peer）切到 c6446r 后，非 identical 198 → 169
#   （44 NEAR → 15，29 个翻为 IDENTICAL/IDENTICAL_AE，零回归）。
#   全量 c6446r 为 170（略差 1）；DNFAppStopInit/DNFAppStartInit 保持 c6444r。
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
ROOT="$(cd "$(dirname "$0")" && pwd)"
C6="$ROOT/dnf_c6_gxx.sh"
C6444R="$ROOT/dnf_c6444r_gxx.sh"
C6446R="$ROOT/dnf_c6446r_gxx.sh"
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
        exec "$C6446R" $filtered ;;
    DNFTcpHandler|DNFSignalTranslator|DNFApplication|DNFGuildServerMain|DNFThreadInterface|DNFTcpAcceptThread|DNFServerHandler|PowerWar|TcpNetSystem)
        exec "$C6446R" "$@" ;;
    DNFPacketTranslater|GuildCargo|GuildBoard|PowerManager|DNFPacketDecoder|DNFGuild|DNFApplication|DNFGuildServerMain|DNFNetworkThread|DNFSignalTranslator|DNFTcpAcceptThread|DNFTcpNetworkThread|Peer)
        exec "$C6446R" "$@" ;;
    DNFAppStopInit|DNFAppStartInit)
        exec "$C6444R" "$@" ;;
    *)
        exec "$C6" "$@" ;;
esac
