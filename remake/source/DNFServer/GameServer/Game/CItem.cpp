// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CItem/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r CItem 还原（I1 批次，2026-08-16）。
// 函数形态以 docs/class_func_reports/CItem.md + ORIG weak 访问器反汇编为准
// （AE 口径：调用目标归一；字段偏移/常量逐条一致）。
// NeedMaterialDataManager 走权威头（完整定义 + 实现落 NeedMaterialDataManager.cpp），
// 本 TU 不再重复声明（§4.4 单一声明点；两处调用 mangled 一致，零行为变化）。

#include "CItem.h"
#include "NeedMaterialDataManager.h"

// [A31-GI · P338/P327] 补 <iostream> 恢复 __ioinit；GetNeedMaterial() 前移为本 TU
// 首个强外部符号 ⇒ GI 键名 `_GLOBAL__I__ZNK5CItem15GetNeedMaterialEv`（ORIG 0x85139cb）。
#include <iostream>

// ===================== 报告函数（CItem.md） =====================

#include "CItem/GetNeedMaterial.inc"


#include "CItem/CItem.inc"

#include "CItem/dtor_CItem.inc"

#include "CItem/_isPackageItem.inc"


#include "CItem/checkItemUpgradePossible.inc"


#include "CItem/check_item_routing.inc"


#include "CItem/check_job_type.inc"


#include "CItem/isAmplifyPollutionItem.inc"


// ===================== 虚函数（ORIG vtable 0x8c96da8 槽 12..23） =====================

#include "CItem/IsImpossibleUpgrade.inc"


#include "CItem/IsImpossibleAmplify.inc"


#include "CItem/IsImpossibleDisjoint.inc"


#include "CItem/IsImpossibleGift.inc"


#include "CItem/IsImpossibleCharacCargo.inc"


#include "CItem/IsPossibleKiriProtection.inc"


#include "CItem/PeriodAfterUnsealing.inc"


#include "CItem/IsRandomOption.inc"


#include "CItem/GetRandomOptionGrade.inc"


#include "CItem/IsRandomOptionUnseal.inc"


// ===================== 非虚访问器（ORIG weak） =====================

#include "CItem/GetItemName.inc"


#include "CItem/GetAttachType.inc"


#include "CItem/is_stackable.inc"


#include "CItem/get_index.inc"


#include "CItem/get_grade.inc"

#include "CItem/GetSellPrice.inc"


#include "CItem/get_price.inc"


#include "CItem/getItemGroupName.inc"


#include "CItem/getUsableLevel.inc"


#include "CItem/get_rarity.inc"


#include "CItem/getUsablePeriod.inc"


#include "CItem/getExpirationDate.inc"


#include "CItem/GetQuestItemDropBouns.inc"


#include "CItem/GetItemIconName.inc"


#include "CItem/IsEnableWorld.inc"


#include "CItem/IsEnableWorld_1.inc"


// ===================== 缺失弱访问器（ORIG weak） =====================

#include "CItem/isEpicRoutingItem.inc"


#include "CItem/IsRoutingItem.inc"


#include "CItem/check_low_level.inc"


#include "CItem/IsHiddenOption.inc"


#include "CItem/isPackagable.inc"


#include "CItem/isExpertJobUsable.inc"


#include "CItem/GetExpertJobCompoundMaterialVariation.inc"


#include "CItem/GetExpertJobCompoundRateVariation.inc"


#include "CItem/GetExpertJobCompoundResultVariation.inc"


#include "CItem/GetExpertJobSelfDisjointBigWinRate.inc"


#include "CItem/GetExpertJobSelfDisjointResultVariation.inc"


#include "CItem/GetExpertJobAdditionalExp.inc"


#include "CItem/GetFinishPointPrice.inc"


#include "CItem/get_need_skill.inc"


#include "CItem/getFootControlRateLimit.inc"


#include "CItem/GetUsablePvPRank.inc"


#include "CItem/GetIncreaseStatusType.inc"


#include "CItem/GetIncreaseStatusIntData.inc"


#include "CItem/GetNeedLevel.inc"


#include "CItem/GetUpSkillType.inc"


#include "CItem/GetDyeInfo.inc"


#include "CItem/hasCategory.inc"


#include "CItem/IsBanRedeemItem.inc"

