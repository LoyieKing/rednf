// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 advancealtar_Manager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "advancealtar.h"     // advancealtar::Manager / StageDifficulty / ENUM_ERROR
#include "CGameManager.h"     // G_CGameManager / getAdvanceAltar / StageControl 完整声明
#include "PacketGuard.h"      // PacketGuard : InterfacePacketBuf（put_header/put_byte/put_int/finalize）
#include "GameTypes.h"        // cMyTrace
#include "CUser.h"            // CUser 完整类型（m_advanceAltarIdx 字段化写）
#include "advancealtar_ProcStage.h"  // ProcStage / Timer_StageControl / HistoryLog（本批新增叶子头）
#include "GameWorld.h"             // G_GameWorld / out_from_dungeon / send_all（leaveUser）
#include "CGuildServerProxy.h"     // CEventManager / CEventBase 完整类型（isOnEvent）
#include "GlobalData.h"            // GlobalData::s_event_manager（isOnEvent）
#include "CMDPacketStruct.h"         // _SetSlotData 完整定义（setSlot/exchangeSlot/resetSlot 按值访问）

namespace advancealtar

{

// ORIG namespace 内 static 常量（_ZN12advancealtarL<len><name>E，r 段；值实测 ORIG
// .rodata 0x08af2e40..0x08af2e90 / 0x08b6d3cc..）：gcc 4.4 对未使用也发射。
// ----------------------------------------------------------------------------
static const int kMaxSlotCount = 12;            // 0x08af2e70
static const int kPlayMinLevel = 50;            // 0x08af2e64
static const int kMaxStageCount = 50;           // 0x08af2e6c
static const int kUnitSlotCount = 6;            // 0x08af2e50
static const int kSkillSlotCount = 6;           // 0x08af2e54
static const int kMaxBuyItemCount = 50;         // 0x08af2e74
static const int kMaxRewardCount = 30;          // 0x08af2e78
static const int kDefaultTicketFree = 100;      // 0x08af2e4c
static const int kInvalidRewardType = 0;        // 0x08af2e68
static const int kMaxAdvanceAltarDbData = 1;    // 0x08af2e7c
static const int kMaxStrBuffer = 2048;          // 0x08b6d3d0
static const int kMaxZipBuffer = 2048;          // 0x08b6d3cc
static const char* const kDelimterItem = "|";   // 0x08b6d3d8 指针 → '|'(0x7c)
static const char* const kDelimterField = ",";  // 0x08b6d3dc 指针 → ','(0x2c)
static const int KGaugeWidth[4] = { 60, 90, 125, 170 };       // 0x08af2e80
// [b28d-solo-advancealtar_Manager] 元素类型 bool：ORIG setDefaultSlot 对
// m_byte0(bool) 直接 `mov %al` 无 test/setne 归一化（P300 源同型直存）。
static const bool kUnitSlotNoLock[6] = { false, false, false, false, false, false };   // 0x08af2e58
static const bool kSkillSlotNoLock[6] = { false, false, false, false, true, true };  // 0x08af2e5e（实测）

#include "advancealtar_Manager/selectStage.inc"


#include "advancealtar_Manager/procErrorSelectStage.inc"


#include "advancealtar_Manager/dieCharac.inc"


#include "advancealtar_Manager/finishLoad.inc"


#include "advancealtar_Manager/giveUpGame.inc"



// ============================ OnlyOrig_Altar.AltarMgr 批（2026-08-27）=========================
// 新增符号（ORIG 报告全文转写；证据路径见各函数注记）：
//   Manager::{C1,C2,D1,D2,eplp,pause,dieMob,leaveUser}、isOnEvent、
//   CharacAdvanceAltarManager::{gmSetTicket,decTicket,addRewardData,
//     getClearStageSummary,getCurrentAdvanceAltarData,setUnlockBuyItem,
//     setDefaultBuyItem,buyItem}、
//   AdvanceAltarData::{getSlotMap,getBuyItemMap,set}。
// ============================================================================

#include "advancealtar_Manager/Manager.inc"
#include "advancealtar_Manager/dtor_Manager.inc"

#include "advancealtar_Manager/eplp.inc"


#include "advancealtar_Manager/pause.inc"


#include "advancealtar_Manager/dieMob.inc"


#include "advancealtar_Manager/leaveUser.inc"



#include "advancealtar_Manager/gmSetTicket.inc"


#include "advancealtar_Manager/decTicket.inc"


#include "advancealtar_Manager/addRewardData.inc"


#include "advancealtar_Manager/getClearStageSummary.inc"






#include "advancealtar_Manager/setUnlockBuyItem.inc"


#include "advancealtar_Manager/setDefaultBuyItem.inc"


// ---- ORIG T 0x08132d20 后续：本函数自上一提交块起沿用同一文件命名空间；
// 以下为第二收口批次（slot 族全真转写）。迭代器偏移按协调者裁定采用
// pair<const u8,_Slot>* 视图：it+0=键(槽号)、it+1=&second(m_byte0)、
// it+2=&m_int1、it+6=&m_byte5、it+7=&m_int6 —— 与现头 sizeof(_Slot)==0xa 自洽。----

#include "advancealtar_Manager/setSlot.inc"


#include "advancealtar_Manager/exchangeSlot.inc"


#include "advancealtar_Manager/resetSlot.inc"


#include "advancealtar_Manager/setUnlockItemToEmptySlot.inc"


#include "advancealtar_Manager/setDefaultSlot.inc"


#include "advancealtar_Manager/resetStar.inc"


#include "advancealtar_Manager/gmSetStar.inc"


#include "advancealtar_Manager/checkAndCreateDefaultRidable.inc"


#include "advancealtar_Manager/buyItem.inc"


#include "advancealtar_Manager/sendAchievementReward.inc"


#include "advancealtar_Manager/getAdvanceAltarDataByIndex.inc"


#include "advancealtar_Manager/setCurrentAdvanceAltarData.inc"


#include "advancealtar_Manager/setDefaultStage.inc"


#include "advancealtar_Manager/set.inc"


#include "advancealtar_Manager/getSlotItem.inc"


#include "advancealtar_Manager/getCurrentAdvanceAltarData.inc"


// ---- ORIG 内部只读表 _ZN12advancealtarL11kRewardStarE（rodata 0xc，
//      nm 多副本同值）：按难度 0/1/2 → 通关奖励星 {3,2,1}。ORIG 每个使用 TU
//      各持一份 internal 副本，此处以 static 复刻（无 ODR 冲突）。----
static const int kRewardStar[3] = {3, 2, 1};

#include "advancealtar_Manager/clearStage.inc"


#include "advancealtar_Manager/setNextStage.inc"

}  // namespace advancealtar
// ============================================================================
// StageMapList::{isExist,getNextStage,getFirstStageIndex} —— LfAltarStage 批
// 定义归本 TU；声明在 CDataManager.h::StageMapList。
// ============================================================================

#include "advancealtar_Manager/isExist.inc"


#include "advancealtar_Manager/getFirstStageIndex.inc"


#include "advancealtar_Manager/getNextStage.inc"


