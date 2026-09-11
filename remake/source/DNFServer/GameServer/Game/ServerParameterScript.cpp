// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 ServerParameterScript/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "ServerParameterScript.h"

#include <string.h>
#include <algorithm>

#include "DNFLexWrapper.h"

#include <stdio.h>

#include "LogManager.h"
#include "CItemAmplifier.h"   // ENUM_RARITY（[drop upgrade rarity] 段 ORIG
                              // make_pairIR11ENUM_RARITYR15ItemFarmingData 实证）

#include "ServerParameterScript/SetLightServerFlag.inc"

#include "ServerParameterScript/stPremiumGoldCardEvent.inc"


#include "ServerParameterScript/ServerParameterScript.inc"


#include "ServerParameterScript/dtor_ServerParameterScript.inc"


#include "ServerParameterScript/clear.inc"


#include "ServerParameterScript/GetExpBonusWeight.inc"


#include "ServerParameterScript/CheckCollectDungeonStatisticServer.inc"


#include "ServerParameterScript/stFatigueEventParam_t.inc"


#include "ServerParameterScript/stWeekendBouns.inc"


#include "ServerParameterScript/stLevelCond4PremiumGoldCard.inc"


#include "ServerParameterScript/CheckLightServer.inc"


#include "ServerParameterScript/GetEscaladeTutorialReward.inc"


#include "ServerParameterScript/checkAssaultServer.inc"


#include "ServerParameterScript/getAssaultGhostTime.inc"


#include "ServerParameterScript/getAssaultPriceChaospointWeight.inc"


#include "ServerParameterScript/getLuckPoint.inc"


#include "ServerParameterScript/getLuckPointDeductionRate.inc"


// ===================== 简单 getter =====================

#include "ServerParameterScript/GetLottePoint.inc"


#include "ServerParameterScript/GetPriceAverage.inc"


#include "ServerParameterScript/getAssaultPrice.inc"


#include "ServerParameterScript/getChaosPointWeight.inc"


#include "ServerParameterScript/GetAdmissionIncrease.inc"


#include "ServerParameterScript/GetPvPPenaltyRevision.inc"


#include "ServerParameterScript/getRecoverStaminaPrice.inc"


#include "ServerParameterScript/GetPvPChannelGradePenalty.inc"


#include "ServerParameterScript/CheckUnusableEquipmentInfo.inc"


#include "ServerParameterScript/CheckUnusableItem.inc"


#include "ServerParameterScript/get_pvp_item_give.inc"


#include "ServerParameterScript/GetCleanChattingCount.inc"


#include "ServerParameterScript/GetTraningFirstGive.inc"


#include "ServerParameterScript/GetTraningSealGive.inc"


#include "ServerParameterScript/CheckTraningFirstGive.inc"


#include "ServerParameterScript/isDungeonOpen.inc"


#include "ServerParameterScript/setDungeonOpen.inc"


#include "ServerParameterScript/getAssaultUserRunawayNotice.inc"
{
    return m_str594.c_str();
}

// 内联 getter 取址锚定（ORIG W：0x085bed36/0x085bed44/0x08568bb0/0x086d18ba/
//   0x082694fa/0x086d18ac/0x08269508）。文件作用域常量数组使取址成为 odr-use，
// 强制编译器发出各 W 符号；数组仅存于 .rodata，无动态初始化。
typedef int (ServerParameterScript::*SpsIntGetter)();
static const SpsIntGetter g_spsIntGetters[] = {
    &ServerParameterScript::GetLottoCost,
    &ServerParameterScript::GetJackpot,
    &ServerParameterScript::GetDailyMatchCount,
    &ServerParameterScript::GetPaneltyDupMessage,
    &ServerParameterScript::GetMaxCleanChattingCount,
    &ServerParameterScript::GetPvPLiveEventPlayCount,   // ORIG W 0x08269508
};
typedef std::vector<std::pair<int, int> >& (ServerParameterScript::*SpsVecGetter)();
static const SpsVecGetter g_spsVecGetter =
    &ServerParameterScript::GetPvPLiveEventReward;

// [b28d-Lane SymSrvParam] anchorServerParameterScriptWMembers（T 符号，ORIG 无）
// 与 ServerParameterScript::LoadScript（T 符号，ORIG 无同名，真实入口为
// importServerParameterScript）均已删除。anchor 原补发的 stLightServerNotice_t/
// stLuckPointDeduction_t/stBurningFatigueParam_t C1/C1ERKS/aSERKS 与其余弱符号
// 共 92 个：ORIG 均在，且 nm 实证本仓 InitScriptProducers_6/7、STAssignMonster、
// STMapScript、OnlyOrigBatch14 等多 TU 以同名 W 自然发射（链接期 COMDAT 合并），
// 本 TU 删除 anchor 不产生 only_orig；ORIG 无 ServerParameterScript/上述 st* 型
// 全局对象（nm -S ' [BbDd] ' 无 0x49f4/对应 size 命中），故无需数据锚补发。
// ============================================================================
// ============================================================================
// importServerParameterScript —— ORIG T 0x08a870b7（0x51d4 字节，4616 insn）。
// 按 docs/class_func_reports/_lt_global_gt_/importServerParameterScript.md
// （Ghidra 伪 C + 汇编）逐节 [xxx] 分派完整转写。分支名与写入偏移以 ORIG
// 汇编为准（Ghidra 伪 C 在 [training seal give]/[pvp channel grade penalty]/
// [training first give] 三节错位，已按汇编 0x8a8af11/0x8a8b004/0x8a8b115 修正：
// seal give→m_map4994 2-int 循环、grade penalty→m_map497c 2-int 循环、
// first give→PvPPlayerKey 复杂结构 + m_map49c4 计数）。
// 具名布局字段依据 ORIG 汇编的绝对成员偏移；所有普通成员访问均经脚本字段。
// 仍保留的裸偏移仅存在于局部 stUpgradeItemTicketParam_t 字节视图。
// ============================================================================
unsigned int g_new_scriptVersion;   // ORIG B 0x0949b298（仅本函数写入）

#include "ServerParameterScript/GetEquipmentTypeStringToEnum.inc"

#include "ServerParameterScript/importServerParameterScript.inc"


