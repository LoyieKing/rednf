// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CEquipItem/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "GameRand.h"
// df_game_r CEquipItem 还原（I1 批次，2026-08-16）。
// 函数形态以 docs/class_func_reports/CEquipItem.md + ORIG weak 反汇编为准
// （AE 口径：调用目标归一；字段偏移/常量逐条一致）。
// STEquipmentScript 内联完整布局为 I1 工作布局（脚本表批次细化），
// 依赖的辅助类（CodePage/CSystemTime/CMTRand 等）由 GameStubs.cpp 提供桩。

#include "CEquipItem.h"
#include "GamePODTypes.h"

#include <set>
#include <math.h>
#include <string.h>

#include "CSystemTime.h"
#include "GameTypes.h"

// ===================== 外部依赖（其它 TU 提供） =====================

#include "CEquipItem/get_rand_int.inc"

#include "CEnvironment.h"  // m_serverEnvir.m_gcNo（+0x198）
#include "GlobalData.h"    // s_systemTime_（+0x941f714；make_item getCurSec 调用）
extern CEnvironment* G_CEnvironment();

class CDataManager
{
public:
    void set_AuctionRegFreeCeraItem(unsigned int idx);
};
extern CDataManager* G_CDataManager();

#include "CEquipItem/getLevelLinearAbility.inc"


#include "ItemGloballyUniqueIdentifierGenerator.h"  // itemGloballyUniqueIdentifier_t 权威定义
#include "../../ServerCommon/DNFFunctionLib.h"

namespace user_creature
{
class CCreatureScriptMgr
{
public:
    void RegisterCreatureIdAndItemId(int creatureId, int itemId);
};
CCreatureScriptMgr* GetInstanceCreatureScriptMgr();
}

namespace ExtreamDungeon
{
class CCompound_ExtreamDun_Item
{
public:
    void ForceResultItemRule_Setting(int idx, bool flag, int value);
};
}

// STEquipmentScript 权威布局见 STEquipmentScript.h（class STEquipmentScript : public STItemScript，
// [b28d-D43] N82/N89 大函数（669/705）（ORIG 0x0850fd84）。
// 基类 0x178 + 派生。CEquipItem::set_item(STEquipmentScript&) 直接使用权威成员名访问，
// 不再保留本文件的 flat 工作布局副本（消除与权威头的重复定义）。
// ===================== 报告函数（CEquipItem.md） =====================

#include "CEquipItem/WpBonusPointData.inc"
#include "CEquipItem/clear.inc"
#include "CEquipItem/ExpBonusPointData.inc"
#include "CEquipItem/clear_1.inc"

#include "CEquipItem/CEquipItem.inc"


#include "CEquipItem/dtor_CEquipItem.inc"

#include "CEquipItem/IsAvatarColorVariation.inc"


#include "CEquipItem/IsAvatarOptionValid.inc"


#include "CEquipItem/checkItemUpgradePossible.inc"


#include "CEquipItem/check_able_To_equip.inc"


#include "CEquipItem/check_eType.inc"

#include "CEquipItem/getExpBonusPoint.inc"


#include "CEquipItem/getGrowthGrade.inc"


#include "CEquipItem/getGrowthRepairCost.inc"


#include "CEquipItem/getUpgradeCostDiscount.inc"


#include "CEquipItem/getWpBonusPoint.inc"

#include "CEquipItem/get_aura_avatar_option_value.inc"


#include "CEquipItem/isAmplifyPollutionItem.inc"


#include "CEquipItem/isExistUpgradableLevelTag.inc"


#include "CEquipItem/isGrowthType.inc"


#include "CEquipItem/isUpgradableLevel.inc"


#include "CEquipItem/is_aura_avatar_option.inc"


#include "CEquipItem/verify.inc"


// ===================== make_item（ORIG 0x851098a） =====================

#include "CEquipItem/make_item.inc"


// ===================== set_item（ORIG 0x850fd84） =====================

#include "CEquipItem/set_item.inc"



// ===================== 简单虚函数 / 访问器（ORIG weak） =====================

#include "CEquipItem/GetItemType.inc"


#include "CEquipItem/setAvatarOptionSize.inc"


#include "CEquipItem/getAvatarOptionSize.inc"


#include "CEquipItem/IsAvatarItem.inc"


#include "CEquipItem/IsCreatureItem.inc"


#include "CEquipItem/IsClearAvatar.inc"


#include "CEquipItem/IsEnchantalbe.inc"


#include "CEquipItem/getUsableItemType.inc"


#include "CEquipItem/isEnableAvatarFunc.inc"


#include "CEquipItem/GetGuildPowerWarPoint.inc"


#include "CEquipItem/GetUserPowerWarPoint.inc"


#include "CEquipItem/IsImpossibleUpgrade.inc"


#include "CEquipItem/IsImpossibleAmplify.inc"


#include "CEquipItem/IsImpossibleDisjoint.inc"


#include "CEquipItem/IsImpossibleGift.inc"


#include "CEquipItem/IsRandomOption.inc"


#include "CEquipItem/GetRandomOptionGrade.inc"


#include "CEquipItem/set_parent_booster_item_id.inc"


#include "CEquipItem/get_parent_booster_item_id.inc"


#include "CEquipItem/get_endurance.inc"


#include "CEquipItem/getAvatarTypeSelect.inc"

#include "CEquipItem/getAvatarPeriod.inc"


#include "CEquipItem/getAvatarSocket.inc"


#include "CEquipItem/GetUsableMaxLevel.inc"


#include "CEquipItem/getEnchantSystemData.inc"


#include "CEquipItem/get_sub_type.inc"


#include "CEquipItem/get_experience_amount.inc"


#include "CEquipItem/get_creature_minimum_level.inc"


#include "CEquipItem/get_creature_species.inc"


#include "CEquipItem/get_hatch_item_id.inc"


#include "CEquipItem/Set.inc"


#include "CEquipItem/GetRandomItemIndex.inc"

