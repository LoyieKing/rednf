#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 advancealtar_misc/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ advancealtar 杂项批次（Wave5 AltarMisc）。覆盖 ORIG 符号：
//   BuyUpgradeData::{operator= 0x88a5784 W, getFieldData 0x88a2c62, getFieldType
//     0x88a3012, getMaxLevel 0x88a3322, getMinLevel 0x88a33c0,
//     getMinLevelFieldDataPoint 0x88a2f1a, getStatFirstPos 0x88a2f7a,
//     getStatIconIndex 0x88a3056, getStatStringIndex 0x88a3106,
//     isValidLevel 0x88a345e}
//   BuyShopData::reset                                  0x088a1090 T
//   AchievementRewardData::reset                        0x0889ae56 T
//   SurvivalRewardData::getRoundRewardData              0x0889aebe T(const)
//   RewardParameter::getAchievementRewardData           0x0889b544 / 0x0889b5a8
//   AdvanceAltarShopParameter::getMaxStarResetGoldFee   0x088a2bc0 T
//   StageDifficulty::getNextDifficulty                  0x0889900a T
//   isOnEvent                                           0x0812cfa6 T
//   isOpenAdvanceAltar                                  0x0812cfea T
//   putRidableInfoToPacket                              0x08141781 T
//   AdvanceAltarData::{set 0x88992da, getBuyItemMap 0x88999d8,
//                      getSlotMap 0x8899a0c}
//   StageTimeLineParameter::{importPhase 0x889a930,
//                            checkAndSetActionDefine 0x889a032}
// 以及使用点发射的生命周期弱符号：
//   _StarRestFeeByGold C1/C2 0x88a3760（inline ctor，头内定义）
//   _ActionAssign C1/C2      0x889b99c（inline ctor，头内定义）
//   _PhaseSummary C1/C2/D1/D2 0x889b954 / 0x0889ebc0（inline ctor/dtor，头内定义）
//   _BuyItem D1/D2           0x0813b4cc（inline 空析构，随使用点发射）
// 逐函数对照 docs/class_func_reports/ 对应报告；报告缺处以 objdump 转写并标
// [objdump ADDR]。需要 CMake 登记。
#include "CDataManager.h"            // advancealtar 共享类型权威段（含 AdvanceAltarField.h）
#include "advancealtar.h"            // AdvanceAltarData/_AdvanceAltarDbData/_Slot/_Stage 等
#include "advancealtar_StageDifficulty.h"
#include "CDungeon.h"                // SurvivalRewardData/RoundRewardData
#include "GlobalData.h"              // s_event_manager / s_DailyScheduleManager
#include "CGuildServerProxy.h"       // CEventManager::GetRepeatEvent / CEventBase::IsEventing
#include "PacketGuard.h"             // PacketGuard : InterfacePacketBuf
#include "DNFLexWrapper.h"           // ScanType/ScanInt(bool*)/ScanStr
#include "StringResource.h"          // CStringManager（全局域类，LinkFinal 链接收口批）

#include <cstdio>
#include <cstring>

namespace advancealtar
{



// ============================================================================
// BuyUpgradeData 查询组（docs/class_func_reports/advancealtar__BuyUpgradeData/）
// ============================================================================

// ORIG W 0x88a5784（objdump 全文）：+0/+4 直拷、+8 仅低 1 字节（movzbl）、
// +0xc string 赋值、+0x10 vector<FieldType::T> 赋值、+0x1c LevelFieldDataMap
// 赋值；返回 *this。
BuyUpgradeData& BuyUpgradeData::operator=(const BuyUpgradeData& other)
{
    m_nItemId = other.m_nItemId;                                    // [objdump 0x88a578a]
    m_field4 = other.m_field4;                                      // [objdump 0x88a5794]
    m_field8 = (int)(unsigned char)other.m_field8;                  // [objdump 0x88a57a3 movzbl]
    m_strDesc = other.m_strDesc;                                    // [objdump 0x88a57c0 Ss::operator=]
    m_fieldTypes = other.m_fieldTypes;                              // [objdump 0x88a57d8 vector::operator=]
    m_listMap = other.m_listMap;                                    // [objdump 0x88a57f0 map::operator=]
    return *this;
}

#include "advancealtar_misc/getFieldData.inc"


#include "advancealtar_misc/getFieldType.inc"


#include "advancealtar_misc/getMaxLevel.inc"


#include "advancealtar_misc/getMinLevel.inc"


#include "advancealtar_misc/getMinLevelFieldDataPoint.inc"


#include "advancealtar_misc/getStatFirstPos.inc"


#include "advancealtar_misc/getStatIconIndex.inc"


#include "advancealtar_misc/getStatStringIndex.inc"


#include "advancealtar_misc/isValidLevel.inc"


#include "advancealtar_misc/reset.inc"


#include "advancealtar_misc/reset_1.inc"


#include "advancealtar_misc/getRoundRewardData.inc"


// ============================================================================
// RewardParameter::getAchievementRewardData 两个重载
// ============================================================================

// ORIG T 0x0889b544：m_rewardMap.find(type) 命中 → 元素指针，否则 0。
advancealtar::AchievementRewardData*
#include "advancealtar_misc/getAchievementRewardData.inc"


// ORIG T 0x0889b5a8：线性找 m_difficulty==diff 且 m_stageClearCount==count 的
// 条目（命中判断 [objdump 0x889b61c +0x8/+0xc]）；未命中 → 0。
advancealtar::AchievementRewardData*
#include "advancealtar_misc/getAchievementRewardData_1.inc"


#include "advancealtar_misc/getMaxStarResetGoldFee.inc"


#include "advancealtar_misc/getNextDifficulty.inc"


#include "advancealtar_misc/isOnEvent.inc"

#include "advancealtar_misc/isOpenAdvanceAltar.inc"


#include "advancealtar_misc/putRidableInfoToPacket.inc"


// ============================================================================
// AdvanceAltarData::{getBuyItemMap,getSlotMap,set}
// ============================================================================

#include "advancealtar_misc/getBuyItemMap.inc"


#include "advancealtar_misc/getSlotMap.inc"


#include "advancealtar_misc/set.inc"


// ============================================================================
// StageTimeLineParameter::{importPhase,checkAndSetActionDefine}
// ============================================================================

#include "advancealtar_misc/importPhase.inc"


#include "advancealtar_misc/checkAndSetActionDefine.inc"


// ---- 布局断言（本批次相关）-------------------------------------------------
typedef char assert_slotfee_0x4[(sizeof(_StarRestFeeByGold) == 4) ? 1 : -1];

}  // namespace advancealtar
