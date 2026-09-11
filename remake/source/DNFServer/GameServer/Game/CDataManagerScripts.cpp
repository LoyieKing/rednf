// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CDataManagerScripts/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// CDataManager 脚本 POD 的构造/析构实现，独立于通用桩 TU.
#include "CBoosterGage.h"  // BoosterGageData 权威布局（须先于 CDataManager.h）
#include "CDataManager.h"
#include "cMyTraceNoop.h"  // add_lottery_needMoney 命中 trace（ORIG 0x83650f5）
#include "GameWorld.h"
#include "CItemGeneratorMgr.h"
#include "CStackableItem.h"
#include "CItem.h"
#include "GlobalData.h"
#include "CWorldMap.h"
#include "NPCScript.h"   // CNPCScript 完整类型（_destroy 中 delete）
#include "STExpertJobScript.h"  // 权威 STExpertJobScript（唯一声明点）
#include "DNFLexWrapper.h"   // ScanType/ScanInt/ScanStr/ScanFloat/loadRDARScriptFile/
                             // unloadRDARScriptFile（RDAR 脚本扫描接口）
#include "Statuepredicate.h"  // pieceQuestRewardStatus 合并谓词（getRewardStatus）
#include "CDungeon.h"        // advancealtar::{StageDifficulty,RewardItemType}
                             // （importAchievementRewardParameter 解析用）
#include <cstring>

#include "CDataManagerScripts/loadRDARScriptListFile.inc"
                            const char* listPath, std::map<int, int>* unused);
#include "CDataManagerScripts/IS_PVP_CHANNEL.inc"
#include <algorithm>     // std::find（CWorldMap::hasDungeon）

// [b28d-solo-CDataManagerScripts-6 Main 盲区排查] 本 TU **无任何 goto 语句**（grep 实证，
// 命中全为判词注释）⇒「`goto scan_exit` 落进 `if(!ScanType){...}` 体内导致退出循环」
// 的模式结构上不适用；全部 continue/break 均位于循环体内（C++ 语义保证回边/退出正确）。
// jmp/jcc 目标方向（回边 B / 前跳 F）逐位核对（objdump 提 (addr,op,target) 分类）：
// createStackableLotteryInfo(AE) 6/6、get_break_away_section_data(AE) 4/4、
// getRewardStatus(NEAR) 12/12 全 0 mismatch；两 import 大件（DIFF，非 AE 假匹配）
// 方向错位全部由「ORIG 每 continue 独立 `nop;jmp 循环头` 岛 vs 4.4 tail-merge 共享回边」
// 解释，回边语义一致。
// [b28d-solo-CDataManagerScripts-3] dual_rank gap 榜首位（gap 15）复核结论：
// `_ZNSt12_Vector_baseIN12advancealtar9FieldType1TESaIS2_EED1Ev` 为 libstdc++ 头
// 内模板弱符号，本 TU 副本尺寸 0x67 与 ORIG 0x67 **逐条全等**（含 4.1 的
// `_M_deallocate` 后 EH landing pad）；链接后二进制取到的是
// advancealtar_misc.cpp.o 的 0x43 副本（该 TU 走 446 路由头集，dtor 无 landing），
// 属弱符号多副本链接选择差异（P93/N89 类），非本 TU 源码可修。

// [A16 round5 复核] 本 TU 9 件非 identical 全数复核（fresh check_tu_routed 19:17 +
//   p167 调用点差集）：**9 件全部 CALLSITE CLEAR**（差集仅 C1/C2·D1/D2 同址别名
//   P93 伪差）⇒ 语义缺口 0，残差全为跨版本 codegen 形态。逐件判词（均附 ORIG 地址）：
//   · importAchievementRewardParameter 0x0889afb0（gap -12）：N82 帧/槽分区（§6.2 三件套）。
//   · importAdvanceAltarShopParameter 0x088a10cc（gap -6）：**本波已修 P196 符号性**
//     —— 3 处下标循环 `for (size_t fi = 0; fi < v.size(); ++fi)`（`size_t` ⇒ 无符号
//     比较 ⇒ `seta`）改为 `for (int fi = 0; fi < (int)v.size(); ++fi)`，复现 ORIG
//     三处 `cmp -0x48/-0x18/-0xc(%ebp),%eax; setg %al`（**有符号**；ORIG 0x088a156f 段
//     等）⇒ 实测 OUR setg ×3 = ORIG，`seta` 归零（P196：signed/unsigned 语义分叉）。
//     残余 = 我方多 `sete`×3 / `setne`×1（ORIG 直跳形）+ N82 帧/槽分区（gap 不变）。
//   · AddRewardItemInfo 0x0890cfda（gap -2）：N107 —— ORIG 0x0890d04f
//     `lea (%ecx,%eax,1),%eax; 0x0890d052 lea 0xc(%eax),%edx; mov val,(%edx)`，
//     我方 `lea (%ecx,%eax,1),%edx; mov val,0xc(%edx)`（4.4 恒折 disp；下标形与
//     指针算术形两写法实测同形，判词原文见函数头）。
//   · GetRewardItem 0x0890d326（gap +14）：N107/N7 —— ORIG 单条 scaled
//     `mov 0xc(%eax,%ecx,1),%eax`，我方 `lea(%ecx,%eax,1); add $0xc` ×2 + 多
//     push %esi 与 3-pop epilogue（寄存器角色族，P333 边界外的地址形成差）。
//   · reseal_table_t::get_reseal_cost 0x08900b02（gap +3）：P209 附（unsigned 比较
//     恒载寄存器；S1 一次性旋钮已用 —— idx 改 unsigned 实测劣化并回滚）+ N18a
//     （`mov %eax,%ecx` 单段 regmove 不可造）。
//   · _getIntegratedPvPItemAttr 0x084fc541（gap +3）：P9 —— callee GetReSealCount
//     返回 uchar ⇒ 4.4 调用点恒插 `movzbl %al,%eax` ×2（ORIG 无）；改共享头返回
//     类型须连动 Inven_Item.cpp body 与全调用点，保留。
//   · isBoundaryExpValue 0x0849f420（NEAR，gap +1）：N22/N18b —— ORIG
//     `cmp %eax,%ebx`（dst=局部 e）vs 4.4 `cmp %ebx,%eax`（dst=字段/call 侧）；
//     交换源码操作数实测同形、P248 三档试尽；尾 nop 属 §6.1 对齐豁免。
//   · getRewardStatus 0x08a6be66（NEAR，gap +1）：N126/N95 —— 全帧槽号恒浅 0xc
//     （版本首槽基址常数，P332「永封部分」）+ 1 nop 豁免。
//   · getPvPSkillPointNext 0x08a5df0e（gap -1）：N93/N112 —— 槽号整体平移 4
//     （jcc 30/30 无极性反写）。
//   [A16 round5 · C05 四范式复查留痕（fresh 19:38-19:39）] 本 TU 9 件机械扫描：
//   ① F2（ORIG `cmp -N(%ebp),%eax` + setcc）命中 **0** 处 ⇒ 无 P209 反向可用形；
//   ② 尾 fail 块（尾 `mov $0/$1,%eax`）命中 **0** 处；
//   ③ 岛差：importAchievementRewardParameter / importAdvanceAltarShopParameter 的
//      ORIG 岛目标重复（0x889b05f×9；0x88a12ab×4 等 8 目标各 2~3 次）⇒ C05③ 负例
//      （4.4 合并/跳板不可控），封 N；其余 7 件 ORIG/我方岛数相等（0~1，已对上）；
//   ④ 数据区解码假象本 TU 未命中。⇒ 无 C05 可落地件，维持 N 判词。
//   [补·本波新增两轴复核（Main 配方：cv 限定符 / 循环符号性）]
//   轴A `const` 常量传播：本 TU 9 件 ORIG/OUR 的 `cmpl $imm,-N(%ebp)` 计数**全等**
//      （0/0 … 2/2），ORIG 段 `cmp -N(%ebp),%e` 槽读形 0 处 ⇒ 无「我方多折叠」件；
//   轴B 循环符号性：**已命中并修**（importAdvanceAltarShopParameter 3 处，见上），
//      其余 8 件 setcc 符号序与 ORIG 一致（扫描数据见本块上位判词）。
//   [补·Main 细化口径] `test` 零比较子型亦复核：本 TU 9 件 ORIG 段
//   `mov -N(%ebp),%reg; test %reg,%reg` 对**全 0 处**；我方 `cmpl $0x0,-N(%ebp)`
//   2 处（_getIntegratedPvPItemAttr / importAdvanceAltarShopParameter）两侧同形
//   ⇒ 无 `unsigned`→`int` 可翻转点，勿重试。

// ============================================================================
// 脚本 POD 生命周期（ORIG 语义，见 docs/class_func_reports 与 ORIG 反汇编）。
// 各 ctor 构造 STL 容器成员（必要时调 clear()），dtor 逆序析构。
// ============================================================================

#define ZERO_POD(T) T::T() { memset(this, 0, sizeof(*this)); } T::~T() {}

#include "CDataManagerScripts/AdvanceAltarShopParameter.inc"


#include "CDataManagerScripts/QuestParameterScript.inc"

#include "CDataManagerScripts/dtor_QuestParameterScript.inc"

#include "CDataManagerScripts/clear.inc"


#include "CDataManagerScripts/getRewardLevel.inc"


#include "CDataManagerScripts/getRewardStatus.inc"


#include "CDataManagerScripts/GuildParameterScript.inc"

#include "CDataManagerScripts/dtor_GuildParameterScript.inc"
#include "CDataManagerScripts/clear_1.inc"


#include "CDataManagerScripts/AccountCargoScript.inc"

#include "CDataManagerScripts/dtor_AccountCargoScript.inc"

#include "CDataManagerScripts/PvPSkillTreeParameterScript.inc"

#include "CDataManagerScripts/dtor_PvPSkillTreeParameterScript.inc"

#include "CDataManagerScripts/clear_2.inc"

#include "CDataManagerScripts/getPvPSkillPointNext.inc"


#include "CDataManagerScripts/IndependentDropParameterScript.inc"

#include "CDataManagerScripts/dtor_IndependentDropParameterScript.inc"
#include "CDataManagerScripts/clear_3.inc"


#include "CDataManagerScripts/stMercenarySystemInfo.inc"

#include "CDataManagerScripts/dtor_stMercenarySystemInfo.inc"

#include "CDataManagerScripts/channel_script_t.inc"
#include "CDataManagerScripts/dtor_channel_script_t.inc"

#include "CDataManagerScripts/stBroadCastItemScript.inc"

#include "CDataManagerScripts/dtor_stBroadCastItemScript.inc"

#include "CDataManagerScripts/isUnCommonItem.inc"


#include "CDataManagerScripts/checkCommonItem.inc"


#include "CDataManagerScripts/checkUnCommonNestedItem.inc"


#include "CDataManagerScripts/eventReward.inc"

#include "CDataManagerScripts/dtor_eventReward.inc"


#include "CDataManagerScripts/ConditionLevelChkDungeon.inc"

#include "CDataManagerScripts/dtor_ConditionLevelChkDungeon.inc"
#include "CDataManagerScripts/FairPvPStatScript.inc"

#include "CDataManagerScripts/dtor_FairPvPStatScript.inc"
#include "CDataManagerScripts/clear_4.inc"
#include "CDataManagerScripts/RandomOptionItem_Regenerator.inc"


#include "CDataManagerScripts/RegenerationROI.inc"

#include "CDataManagerScripts/dtor_RegenerationROI.inc"

#include "CDataManagerScripts/stSeriaBlessingScript.inc"
#include "CDataManagerScripts/dtor_stSeriaBlessingScript.inc"

#include "CDataManagerScripts/is_possible_lv.inc"


#include "CDataManagerScripts/FindCharacStat.inc"


#include "CDataManagerScripts/get_break_away_section_data.inc"


#include "CDataManagerScripts/get_break_away_section_low_level.inc"


#include "CDataManagerScripts/stLevelUpRewardItemScript.inc"

#include "CDataManagerScripts/dtor_stLevelUpRewardItemScript.inc"
#include "CDataManagerScripts/stStepRewardStackableItemScript.inc"

#include "CDataManagerScripts/dtor_stStepRewardStackableItemScript.inc"
#include "CDataManagerScripts/stGrowthCapsuleScript.inc"

#include "CDataManagerScripts/CLottery_NeedMoney.inc"
#include "CDataManagerScripts/dtor_CLottery_NeedMoney.inc"

#include "CDataManagerScripts/isNeedMoney.inc"


#include "CDataManagerScripts/add_lottery_needMoney.inc"


#include "CDataManagerScripts/getChannelInfo.inc"


#include "CDataManagerScripts/GetPvPChannelNumber.inc"


#include "CDataManagerScripts/getBonusRate.inc"


#include "CDataManagerScripts/find.inc"


#include "CDataManagerScripts/getFavorLevel.inc"


#include "CDataManagerScripts/getFavorLevelValue.inc"


#include "CDataManagerScripts/getMaxFavorValue.inc"


#include "CDataManagerScripts/_destroy.inc"
{
    __gnu_cxx::hash_map<unsigned int, CNPCScript*>::const_iterator
        it(m_npcMap.begin());
    __gnu_cxx::hash_map<unsigned int, CNPCScript*>::const_iterator
        e(m_npcMap.end());
    while (it != e)
    {
        delete it->second;
        ++it;
    }
    m_npcMap.clear();
}

#include "CDataManagerScripts/dtor_CNPCScriptList.inc"
{
    // ORIG D1 0x83762a8：_destroy() + stNPCCommonData_t D1 + hash_map D1
    _destroy();
}

namespace WongWork
{
#include "CDataManagerScripts/dtor_CItemGeneratorMgr.inc"
#include "CDataManagerScripts/setDropRatio.inc"

// getGenerator 已由 CItemGeneratorMgr.cpp 真实定义，不重复。
}

#include "CDataManagerScripts/getItemGenerator.inc"

// get_event_script_mng 已由 CDataManager.cpp 定义，不重复。
#undef ZERO_POD

#include "CDataManagerScripts/GetCreateInfo.inc"


#include "CDataManagerScripts/GetCurrUpgradeInfo.inc"


#include "CDataManagerScripts/GetNextUpgradeInfo.inc"


#include "CDataManagerScripts/GetNpcIdx.inc"


#include "CDataManagerScripts/GetNpcByDungeonIdx.inc"


#include "CDataManagerScripts/GetNpcByDungeonLev.inc"


// [SplitC 2026-09-11 batch2 · TU 迁移] DeleteInvalidItemScript 已迁出至 global_ScriptDeleteInvalidItem.cpp（ORIG GI 键对齐）。


#include "CDataManagerScripts/dtor_DeleteInvalidItemScript.inc"


#include "CDataManagerScripts/clearScript.inc"


#include "CDataManagerScripts/IsInvalidItem2Delete.inc"


#include "CDataManagerScripts/GetRewardItemInfo.inc"


#include "CDataManagerScripts/GetMailInfo.inc"

#include "CDataManagerScripts/createStackableLotteryInfo.inc"


#include "CDataManagerScripts/createGlobalEffectInfo.inc"


// ============================================================================
// SetLightServerFlag / GetLightServerFlag（ORIG 0x8a8cba5 / 0x822ad3a）。
// ORIG 全局标志位于 0x940b558（.bss）；SetLightServerFlag 经
// GetLightServerFlag() 取指针后写入 bool。
// ============================================================================

// probe 实证（/tmp/wk/probe12.cpp）：调用点若见 GetLightServerFlag 的"定义"，
// 4.4 将 caller 帧收紧为 0x14/spill -0x14；只见原型则 pad[12] 生效得
// ORIG 帧 0x28/spill -0x1c/p -0xc（0x8a8cba8-0x8a8cbbe）⇒ 定义置于调用点之后。
bool* GetLightServerFlag();
#include "CDataManagerScripts/SetLightServerFlag.inc"


bool* GetLightServerFlag()
{
    static bool light_server_flag = false;  // ORIG _ZZ18GetLightServerFlagvE17light_server_flag V @0x940b558
	return &light_server_flag;
}
#include "CDataManagerScripts/_checkTimeoutItem.inc"


#include "CDataManagerScripts/_getIntegratedPvPItemAttr.inc"


// ============================================================================
// GetIntegratedPvPItemAttr（ORIG T 0x84fc5ff）
// ORIG 反汇编（84fc5ff）：
//   G_GameWorld()->IsIntegratedPvPBaseChannel() 且
//   G_CDataManager()->GetUpgradeRevisionPvPChannel() == 0 → _getIntegratedPvPItemAttr(item)
//   否则 → item.GetItemAttr()
// ============================================================================
// ORIG 0x84fc5ff：返回类型为 byte（函数体只经最后 call 的 %eax/%al 透传，
//   else 分支 `call GetItemAttr; leave; ret` 无 movzbl 扩宽）。声明 unsigned char
//   ⇒ 两分支均不透传扩展（true 支 int 直落、else 支 byte 无扩），byte 全等 ORIG。
//   调用方（CAccountCargo/CCargo/CTitleBook）均 `(int)Get...(item)&0xff`，低字节一致。
unsigned char GetIntegratedPvPItemAttr(const Inven_Item& item)
{
    if (G_GameWorld()->IsIntegratedPvPBaseChannel() &&
        G_CDataManager()->GetUpgradeRevisionPvPChannel() == 0)
    {
        return _getIntegratedPvPItemAttr(item);
    }
    return item.GetItemAttr();
}

// ============================================================================
// GameStubs.cpp 迁移：纯数据脚本类 ctor/dtor（符号匹配 ORIG，T）
// 权威声明见 CDataManager.h（seria/growth/create/use/rank/death/blood/village/
// questShop/WorldDrop/Slang/Invalid/GrowthPower/Booster/AvatarColor 等）、
// CStackableItem.h（RecipeInfo/MultiBoxLotteryInfo）、CEquipItem.h（RandomItemTable）、
// CItemList.h（BaseItemKey/BaseItemValue）。
// ============================================================================

#include "CDataManagerScripts/RecipeInfo.inc"

#include "CDataManagerScripts/dtor_RecipeInfo.inc"
#include "CDataManagerScripts/clear_5.inc"


#include "CDataManagerScripts/changeOutput.inc"


#include "CDataManagerScripts/RandomItemTable.inc"
#include "CDataManagerScripts/dtor_RandomItemTable.inc"

#include "CDataManagerScripts/BaseItemKey.inc"

// [b28d-Main·检查点21] BaseItemValue ctor 已删（ORIG 无符号，POD）。

#include "CDataManagerScripts/seriaRoomDecoEventScript.inc"

#include "CDataManagerScripts/dtor_seriaRoomDecoEventScript.inc"

#include "CDataManagerScripts/growthEquipmentScript.inc"

#include "CDataManagerScripts/dtor_growthEquipmentScript.inc"

#include "CDataManagerScripts/createChracScript.inc"
#include "CDataManagerScripts/dtor_createChracScript.inc"
#include "CDataManagerScripts/rewardInfoList.inc"

#include "CDataManagerScripts/useCharacterJobFatigue.inc"

#include "CDataManagerScripts/dtor_useCharacterJobFatigue.inc"

#include "CDataManagerScripts/CharacterCreateCreatureGiftScript.inc"

#include "CDataManagerScripts/dtor_CharacterCreateCreatureGiftScript.inc"

#include "CDataManagerScripts/seriaRoomDecoAniEventScript.inc"
#include "CDataManagerScripts/dtor_seriaRoomDecoAniEventScript.inc"

#include "CDataManagerScripts/stDungeonRankSystem_t.inc"

#include "CDataManagerScripts/dtor_stDungeonRankSystem_t.inc"
#include "CDataManagerScripts/clear_6.inc"


#include "CDataManagerScripts/stRankSystemInfo.inc"

#include "CDataManagerScripts/dtor_stRankSystemInfo.inc"
#include "CDataManagerScripts/clear_7.inc"


#include "CDataManagerScripts/stDungeonClearTimeData.inc"

#include "CDataManagerScripts/clear_8.inc"

// [b28d-Main 代落·S334 P317] 空析构定义删除（声明端同步）。

#include "CDataManagerScripts/stComboIntervalBoundary.inc"

#include "CDataManagerScripts/clear_9.inc"

// [b28d-Main 代落·S334 P317] 空析构定义删除（声明端同步）。

#include "CDataManagerScripts/stBloodDungeon_t.inc"

#include "CDataManagerScripts/dtor_stBloodDungeon_t.inc"
#include "CDataManagerScripts/clear_10.inc"

// stVillageAttacked_t 现为纯 vector 包装（CDataManager.h），ctor/dtor 隐式生成
// （ORIG W 0x837cf1e/0x837cf32 即隐式符号），无显式定义。

// ---- stDeathTower_t（ORIG C1 0x08370970）----

#include "CDataManagerScripts/stDeathTower_t.inc"


#include "CDataManagerScripts/live_server_info_t.inc"
#include "CDataManagerScripts/dtor_live_server_info_t.inc"

#include "CDataManagerScripts/stQuestShop.inc"

#include "CDataManagerScripts/dtor_stQuestShop.inc"
#include "CDataManagerScripts/InvalidCharData.inc"

#include "CDataManagerScripts/dtor_InvalidCharData.inc"

#include "CDataManagerScripts/SlangNameData.inc"

#include "CDataManagerScripts/dtor_SlangNameData.inc"

#include "CDataManagerScripts/STGrowthPowerData.inc"

#include "CDataManagerScripts/dtor_STGrowthPowerData.inc"

#include "CDataManagerScripts/AvatarColorInfo.inc"

#include "CDataManagerScripts/dtor_AvatarColorInfo.inc"
#include "CDataManagerScripts/BoosterGageData.inc"

#include "CDataManagerScripts/dtor_BoosterGageData.inc"

// ============================================================================
// GameStubs.cpp 迁移（第二批）：其余纯数据脚本类 ctor/dtor（符号匹配 ORIG，T）
// ============================================================================

#include "CDataManagerScripts/reseal_table_t.inc"

#include "CDataManagerScripts/dtor_reseal_table_t.inc"

#include "CDataManagerScripts/get_reseal_cost.inc"


#include "CDataManagerScripts/get_oneshot_reseal_cost.inc"


#include "CDataManagerScripts/InGameAdvertisementScript.inc"


#include "CDataManagerScripts/clear_11.inc"


#include "CDataManagerScripts/PcBangItemRentarData.inc"

#include "CDataManagerScripts/dtor_PcBangItemRentarData.inc"

#include "CDataManagerScripts/DimensionActivationData.inc"

#include "CDataManagerScripts/dtor_DimensionActivationData.inc"

#include "CDataManagerScripts/stUnlimitChallengeInfo.inc"

#include "CDataManagerScripts/dtor_stUnlimitChallengeInfo.inc"

#include "CDataManagerScripts/stLimitItemUsageInfoEx.inc"

#include "CDataManagerScripts/dtor_stLimitItemUsageInfoEx.inc"

#include "CDataManagerScripts/CNPCRelationEventManager.inc"

#include "CDataManagerScripts/dtor_CNPCRelationEventManager.inc"


#include "CDataManagerScripts/AddNPCInfo.inc"


#include "CDataManagerScripts/AddFavorItemInfo.inc"


#include "CDataManagerScripts/AddRewardItemInfo.inc"


#include "CDataManagerScripts/AddDislikeItemInfo.inc"


#include "CDataManagerScripts/GetFavorIndex.inc"


#include "CDataManagerScripts/IsDislikeItem.inc"


#include "CDataManagerScripts/GetItemKind.inc"


#include "CDataManagerScripts/stCharacLinkSystem_t.inc"

#include "CDataManagerScripts/dtor_stCharacLinkSystem_t.inc"
#include "CDataManagerScripts/GetRewardItem.inc"


#include "CDataManagerScripts/StageTimeLineParameter.inc"

#include "CDataManagerScripts/dtor_StageTimeLineParameter.inc"
#include "CDataManagerScripts/dtor_AdvanceAltarShopParameter.inc"
#include "CDataManagerScripts/BuyUpgradeData.inc"

#include "CDataManagerScripts/RewardParameter.inc"

#include "CDataManagerScripts/dtor_RewardParameter.inc"
#include "CDataManagerScripts/BuyShopData.inc"

#include "CDataManagerScripts/ConfigParameter.inc"

#include "CDataManagerScripts/dtor_ConfigParameter.inc"

#include "CDataManagerScripts/reset.inc"


#include "CDataManagerScripts/importRewardTable.inc"


#include "CDataManagerScripts/DeleteInvalidItemData.inc"

#include "CDataManagerScripts/init.inc"


#include "CDataManagerScripts/loadScript.inc"


#include "CDataManagerScripts/importConfigParameter.inc"


#include "CDataManagerScripts/importAdvanceAltarShopParameter.inc"


#include "CDataManagerScripts/getFieldDataPoint.inc"


// ---- advancealtar::BuyShopData::getBuyUpgradeDataVec（ORIG T 0x088a105c）----
// type 0/1/2 → &m_vecT0/T1/T2；其它 → 0。报告 advancealtar__BuyShopData/。
std::vector<advancealtar::BuyUpgradeData>*
#include "CDataManagerScripts/getBuyUpgradeDataVec.inc"


// ---- advancealtar::AdvanceAltarShopParameter::getBuyUpgradeDataVec
// （ORIG T 0x088a0fe8）：m_buyShopMap.find(key) 未命中→0；命中转调
// BuyShopData::getBuyUpgradeDataVec(type)。报告同名 md。
std::vector<advancealtar::BuyUpgradeData>*
#include "CDataManagerScripts/getBuyUpgradeDataVec_1.inc"


#include "CDataManagerScripts/importAchievementRewardParameter.inc"


#include "CDataManagerScripts/PowerParameterScript.inc"

#include "CDataManagerScripts/dtor_PowerParameterScript.inc"


#include "CDataManagerScripts/GetPenalty.inc"


#include "CDataManagerScripts/GetBreakBonus.inc"


#include "CDataManagerScripts/checkPowerWarServer.inc"


// ---- SecretShopScript（成员默认构造）----

#include "CDataManagerScripts/SecretShopScript.inc"

#include "CDataManagerScripts/dtor_SecretShopScript.inc"

#include "CDataManagerScripts/stTitleBookInfo.inc"

#include "CDataManagerScripts/dtor_stTitleBookInfo.inc"

#include "CDataManagerScripts/stItemMakingSkill.inc"

#include "CDataManagerScripts/dtor_stItemMakingSkill.inc"
#include "CDataManagerScripts/clear_12.inc"


#include "CDataManagerScripts/ChoiceItemInfo.inc"


#include "CDataManagerScripts/clear_13.inc"

#include "CDataManagerScripts/ImageCommunicationData.inc"


// ---- ARAD::SCRIPT::AradJumping_Script----
// W7_BuildHarmonize 去重：生命周期唯一 owner 为 Arad_Script.cpp
// （同文件含 getRDARScriptData/importScript 等同类方法）。

#include "CDataManagerScripts/EventEtcScript.inc"

#include "CDataManagerScripts/dtor_EventEtcScript.inc"
#include "CDataManagerScripts/clear_14.inc"


#include "CDataManagerScripts/stReturnUserRewardScript.inc"

#include "CDataManagerScripts/dtor_stReturnUserRewardScript.inc"
#include "CDataManagerScripts/stBingoScript.inc"

#include "CDataManagerScripts/dtor_stBingoScript.inc"
#include "CDataManagerScripts/stNewAccountLevelUpToJobScript.inc"

#include "CDataManagerScripts/dtor_stNewAccountLevelUpToJobScript.inc"
#include "CDataManagerScripts/CWorldMapList.inc"
#include "CDataManagerScripts/dtor_CWorldMapList.inc"


#include "CDataManagerScripts/destroy.inc"


#include "CDataManagerScripts/find_world_map.inc"


#include "CDataManagerScripts/IsInHellDungeon.inc"


#include "CDataManagerScripts/hasDeathTower.inc"


#include "CDataManagerScripts/getDungeonMinLevel.inc"


#include "CDataManagerScripts/hasDungeon.inc"


#include "CDataManagerScripts/StageMapList.inc"
#include "CDataManagerScripts/dtor_StageMapList.inc"
#include "CDataManagerScripts/WarAreaCounter.inc"


#include "CDataManagerScripts/clear_15.inc"

#include "CDataManagerScripts/dtor_WarAreaCounter.inc"

#include "CDataManagerScripts/stEquipmentGradeSample.inc"

#include "CDataManagerScripts/dtor_stEquipmentGradeSample.inc"

#include "CDataManagerScripts/stOneEmotionData.inc"

// ORIG 0x891b640（stOneEmotionData::operator=）：逐成员复制
// 0x00/0x04 int、0x08/0x0c/0x14 string、0x10/0x11/0x12 char、0x18 vector。
// 注意：+0x13 为 padding（m_pad13），ORIG 不复制；镜像布局须显式定义以
// 覆盖编译器隐式逐成员赋值（隐式版会复制 m_pad13）。
stOneEmotionData& stOneEmotionData::operator=(const stOneEmotionData& rhs)
{
    m_0 = rhs.m_0;
    m_4 = rhs.m_4;
    m_8 = rhs.m_8;
    m_c = rhs.m_c;
    m_10 = rhs.m_10;
    m_11 = rhs.m_11;
    m_12 = rhs.m_12;
    m_14 = rhs.m_14;
    m_18 = rhs.m_18;
    return *this;
}
#include "CDataManagerScripts/dtor_stOneEmotionData.inc"
#include "CDataManagerScripts/clear_16.inc"

#include "CDataManagerScripts/stEmotionTypeTagsInfo.inc"

#include "CDataManagerScripts/dtor_stEmotionTypeTagsInfo.inc"
#include "CDataManagerScripts/clear_17.inc"


#include "CDataManagerScripts/stConditionEventInfo.inc"

#include "CDataManagerScripts/dtor_stConditionEventInfo.inc"

#include "CDataManagerScripts/pvp_channel_info_t.inc"


// ============================================================================
// STExpertJobScript::isBoundaryExpValue（TSV 修复，ORIG 0x0849f420 T）。
// ORIG：遍历 vector<ExpertJobExp>（元素尺寸 12，见 _ZNSt6vectorI12ExpertJobExpSaIS0_EEixEj
// 乘法 i*12），任一元素首整数字段 == exp 返回 true，否则 false。
// 权威声明见 STExpertJobScript.h（CUser::GetCurExpertJobLevel 真实调用）。
// ============================================================================

#include "CDataManagerScripts/isBoundaryExpValue.inc"


#include "CDataManagerScripts/GetLevel.inc"


#include "CDataManagerScripts/GetCompoundRate.inc"


#include "CDataManagerScripts/GetRecipeInfoIndex.inc"


#include "CDataManagerScripts/GetRecipeInfo.inc"

#include "CDataManagerScripts/getGuildExpWeight.inc"


#include "CDataManagerScripts/getGuildAgitUpgradeInfo.inc"


// ---- GuildParameterScript::getExpWeight（ORIG T 0x08979864）----

#include "CDataManagerScripts/getExpWeight.inc"


#include "CDataManagerScripts/getGuildLevelUpParam.inc"


#include "CDataManagerScripts/getLevelupPrice.inc"


#include "CDataManagerScripts/getGuildExpBook.inc"

#include "CDataManagerScripts/IS_PVP_CHANNEL_1.inc"

