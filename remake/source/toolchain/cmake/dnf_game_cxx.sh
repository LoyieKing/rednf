#!/bin/sh
# game 逐 TU 编译器分发（2026-08-16 起）。
# 初始：全部 c6（c6root 4.4.7 驱动）。
# 后续按 df_game_r 二进制各 TU 的实际 cc1plus 变体逐步切 c6446r/c6444r，
# 判定口径：diff_func.py --bin game 逐函数 IDENTICAL/NEAR/DIFF 统计。
ROOT="${DNF_TC_ROOT:-$(cd "$(dirname "$0")/../../.." && pwd)/toolchains}"
C6="${ROOT}/c6root/usr/bin/g++"
TOOLDIR="$(cd "$(dirname "$0")" && pwd)"
C6446R="$TOOLDIR/dnf_c6446r_gxx.sh"
C6444R="$TOOLDIR/dnf_c6444r_gxx.sh"
# W7_BuildHarmonize：项目共享头使用 C++0x static_assert；下方被指令级对齐
# 强制 -std=gnu++98 的 TU 需 -include 注入 C++03 兼容展开（static_assert_compat.h），
# 否则 make 全量失败（单 TU 检查链统一 gnu++0x 故不复现该差异）。
SA_COMPAT="-include"
SA_HDR="$TOOLDIR/static_assert_compat.h"

export LD_LIBRARY_PATH="${ROOT}/c6root/usr/lib64:${ROOT}/c6root/usr/lib"

src=""
prev=""
for a in "$@"; do
    case "$a" in
        *.cpp) src="$a" ;;
    esac
done
case "$(basename "$src" .cpp)" in
    *)
        case "$src" in
            *Game/CEnvironment.cpp)
                # CEnvironment：ORIG 为 4.4.6（虚调用/寄存器分配验证：4.4.7 下
                # load 的虚调用目标寄存器不同 → near；4.4.6 逐操作数一致）
                exec "$C6446R" "$@" ;;
            *Game/LogManager.cpp)
                # LogManager：ORIG DWARF producer = Red Hat GCC 4.4.6-3
                # （P2 组验证：4.4.6 下 logString 等虚调用寄存器逐操作数一致）
                exec "$C6446R" "$@" ;;
            *Game/BaseServerProxy.cpp)
                # BaseServerProxy：ORIG 为 4.4.6（虚调用目标寄存器 edx 形态 +
                # OnTcpRecv/OnTcpSend/Disconnect 寄存器分配：4.4.7 下 3 个 near；
                # 4.4.6 下 11/12 AE，仅 InitTcp 差 errnoVal/ret 寄存器互换）
                exec "$C6446R" "$@" ;;
            *Game/CHadesServerProxy.cpp)
                # CHadesServerProxy：本 TU 无虚调用，c6/c6446r 均 16/16 AE；
                # 与 BaseServerProxy 同批（E1），统一走 c6446r 保持一致性
                exec "$C6446R" "$@" ;;
            *Game/CItemList.cpp)
                # CItemList：ORIG 为 4.4.6（destroy/create_item 虚调用寄存器形态
                # + FindGoods 迭代器槽位：4.4.6 下 identical=147 ae=675（合计 822）
                # near 3→1，diff 保持 8；4.4.7 下合计 820）
                exec "$C6446R" "$@" ;;
            *Game/CUser.cpp)
                # CUser：ORIG 为 4.4.6（构造/EH 着陆垫链、子对象寄存器形态；
                # CUser agent 验证 c6446r 更接近，建议与 CItemList 同路由）
                exec "$C6446R" "$@" ;;
            *Game/CExpandEquipslot.cpp)
                # CExpandEquipslot：loadData 虚调用寄存器形态需 4.4.6
                # （c6 NEAR / c6446r AE；G1-3 refine agent 验证）
                exec "$C6446R" "$@" ;;
            *Game/FlexLexerEngine.cpp)
                # flex 2.5.35 生成 TU：ORIG 与 4.4.6 虚调用/寄存器分配一致
                exec "$C6446R" "$@" ;;
            *Game/SlangFlexLexerEngine.cpp)
                exec "$C6446R" "$@" ;;
            *Game/SlangFilter.cpp)
                exec "$C6446R" "$@" ;;
            *Game/CSyncSlangFilter.cpp)
                exec "$C6446R" "$@" ;;
            *Game/DNFLexWrapperHelpers.cpp)
                exec "$C6446R" "$@" ;;
            *Game/DNFLexWrapper.cpp)
                # ORIG wrapper getters are -O2 (pop-ebp-before-load, tail-call tok_str).
                exec "$C6446R" "$SA_COMPAT" "$SA_HDR" "$@" -O2 -std=gnu++98 ;;
            *Game/DNFLexWrapperStream.cpp)
                exec "$C6446R" "$SA_COMPAT" "$SA_HDR" "$@" -O2 -std=gnu++98 ;;
            *Game/DNFLexWrapperLoadStream.cpp)
                exec "$C6446R" "$SA_COMPAT" "$SA_HDR" "$@" -O2 -std=gnu++98 ;;
            *Game/CompiledDNFLexCore.cpp)
                exec "$C6446R" "$SA_COMPAT" "$SA_HDR" "$@" -O2 -std=gnu++98 ;;
            *Game/Arad_Script.cpp)
                # ORIG Arad_Script is -O0 (reload this from 0x8(%ebp) each
                # field access). -O2 made every getter DIFF.
                exec "$C6446R" "$SA_COMPAT" "$SA_HDR" "$@" -std=gnu++98 ;;
            *Game/ScriptExtras.cpp)
                exec "$C6446R" "$@" ;;
            *Game/RDARScriptStringManager.cpp)
                exec "$C6446R" "$@" ;;
            *Game/ScriptStringManager.cpp)
                # ORIG ctor inlines empty vector/map (vptr+zeros+header).
                exec "$C6446R" "$SA_COMPAT" "$SA_HDR" "$@" -O2 -std=gnu++98 ;;
            *Game/DNFLexCore.cpp)
                # SRC_OPTS 强制 -O2 -std=gnu++98（见 game/CMakeLists.txt），同样注入兼容头
                exec "$C6446R" "$SA_COMPAT" "$SA_HDR" "$@" ;;
            *ServerLab/ServerLib/*)
                # nsl 基础库：ORIG 为 Red Hat GCC 4.4.6-3（cc1plus446bin）编译
                # （E/D/C 组独立验证：4.4.6 下虚调用/寄存器分配与 point ORIG 逐操作数一致）
                exec "$C6446R" "$@" ;;
            *)
                exec "$C6" "$@" ;;
        esac ;;
esac
