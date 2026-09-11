// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CEventManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ CEventManager 实现（独立 TU，从 GameStubs.cpp 拆出）。
#include "GameWorld.h"
#include "DBMgr.h"
#include "MySQL.h"
#include "GlobalData.h"
#include "CGlobalEffectManager.h"
#include "CEnvironment.h"
#include "InterfacePacketBuf.h"
#include "CGuildServerProxy.h"

#include <cstring>
#include <cstdio>

#include <new>
#include <algorithm>
#include <map>
#include <vector>
#include <boost/foreach.hpp>           // ORIG attach/notify 用 BOOST_FOREACH 遍历 list
#include "RDARScriptStringManager.h"   // g_scriptStringManager_/findString
#include "CEventTable.h"              // CEventTable 权威声明（唯一）
#include "PacketGuard.h"
// InitEventManager 事件类声明（ORIG InitEventManager 0x08114dcc 槽位映射实证）。
#include "CUnlimitFatigueEvent.h"
#include "CMaxFatigueFactorEvent.h"
#include "CExpDoubleEvent.h"
#include "CCoinEventPerDay.h"
#include "ARAD_Database.h"           // CCoinEventOnCharCreate
#include "CLeadingChannelEvent.h"
#include "CItemDropRatioEvent.h"
#include "CPCRoomBurningEvent.h"
#include "CGuildWarEvent.h"
#include "CPCRoomFatigueEvent.h"
#include "CReformingDanjinEvent.h"
#include "CCoinRefillEvent.h"
#include "CBurningFatigueEvent.h"
#include "CClearRewardCardEvent.h"
#include "CCeraShopBonusItemEvent.h"
#include "CTournamentPvPEvent.h"
#include "CGoldCardBlankItemEvent.h"
#include "CPcRoomCardBlankItemEvent.h"
#include "CPowerWarEvent.h"
#include "CCollectArchieveEventLog.h"
#include "CPCRoomWorldDropEvent.h"
#include "CPartyExpBonusEvent.h"
#include "CStabToDeathEvent.h"
#include "CSchoolMatchEvent.h"
#include "W9_EventSmall.h"           // CAutoMarketConditionsControlEvent / CWeekendBonusEvent
#include "CVendingMachineBonusEvent.h"
#include "CBurnigGoldMonsterEvent.h"
#include "CNoNeedGoldOnGuildCreateEvent.h"
#include "CDeathTowerWinPointEvent.h"
#include "CNotApplyBalkeunEvent.h"
#include "CCharacterDayEvent.h"
#include "CRestrictCharacCreationEvent.h"
#include "CReduceUpgradeItemPay.h"
#include "COnTimeEvent.h"
#include "CBreakAwayPreventEvent.h"
#include "CPowerWarVictoriousEvent.h"
#include "CSecretShopEvent.h"
#include "CFatigueAttendance.h"
#include "CConditionEvent.h"
#include "CUXGameLogEvent.h"
#include "BlueMarbleDungeonEvent.h"
#include "CStopOverlabExpEvent.h"
#include "CEventAdvanceAltarOpen.h"
#include "CAradRyosikaEvent.h"
#include "Arad_MomijiEvent.h"
#include "LevelupSupportEvent.h"
#include "CEventStayTime.h"
#include "CEventCreateDnf.h"
#include "EventGiveMeBox.h"
// 全局 events[] 静态初始化（ORIG `D events @0x0936fa60`，79 项 .data+sid_ 实证）专用：
#include "CAssaultOnOffEvent.h"
#include "CBloodDungeonRewardFirstEvent.h"
#include "CBloodDungeonRewardSecondEvent.h"
#include "CPCRoomWorldDropEvent2nd.h"
#include "CPvPExpPenaltyEvent.h"
#include "CPvPLiveEvent.h"
#include "CIntegratedPvPServerMatchEvent.h"
#include "COnlinePreliminaryEvent.h"
#include "CDnFLeaguePromoteFirstEvent.h"
#include "CDnFLeaguePromoteSecondEvent.h"
#include "CDoubleGoldCardEvent.h"
#include "CPremiumGoldCard.h"
#include "CGmRegistEvent.h"
#include "CPCRoomPlayTimeEvent.h"
#include "LevelUpBefore70LvEvent.h"
#include "CDimensionActivationEvent.h"
#include "AttendanceEvent.h"
#include "GrowthEquipEvent.h"
#include "CFatigueGiveItemEvent.h"
#include "NewAccountLevelUpEventToJob.h"
#include "BingoEvent.h"
#include "W8LT_EventTail.h"          // GiveGrowCreatureEvent / SeriaRoomAniDecoEvent / OneADayItemShopEvent
#include "CEventCeraShopRewardPoint.h"
#include "EventNewCharacterReward.h"
#include "HeroMissionEvent.h"        // ARAD::Singleton<HeroMissionEvent>::GetPtr（events[76]）
#include "LevelupSupport2ndEvent.h"

#include "CEventManager/CEventManager.inc"


#include "CEventManager/CDelivery.inc"


#include "CEventManager/dtor_CDelivery.inc"


#include "CEventManager/checkAddress.inc"


#include "CEventManager/attach.inc"


#include "CEventManager/detach.inc"


#include "CEventManager/notify.inc"





#include "CEventManager/dtor_CEventManager.inc"


#include "CEventManager/AddEvent.inc"


#include "CEventManager/GetRepeatEvent.inc"


#include "CEventManager/IsExistOverlappedExpBonusEvent.inc"


#include "CEventManager/dailyresetData.inc"


#include "CEventManager/filterEvent.inc"

// ORIG 0x08115de2 实证（push_back 4/2/2 字节 store + 迭代 deEv 读 +0/+4/+6）：
// 权威类型名 stEVENTADDINFO（vector 实例 mangled St6vectorI14stEVENTADDINFO…），
// 旧重建发明的 EventAddInfo 属发明符号（N114/§4.4）。
struct stEVENTADDINFO
{
    int eventId;             // +0x00（push_back 4 字节 store）
    unsigned short info0;    // +0x04
    unsigned short info1;    // +0x06
};

#include "CEventManager/MakeNotiEventInfo.inc"


#include "CEventManager/BroadcastEventInfo.inc"


#include "CEventManager/TriggerEventStart.inc"


#include "CEventManager/TriggerEventEnd.inc"


#include "CEventManager/InitEventManager.inc"


#include "CEventManager/LoadEventFromDB.inc"


#include "CEventManager/eventDeliveryNotify.inc"
{
    // ORIG 0x08116774：无局部槽，条件与调用各重读 m_delivery（+0x298）
    // 两次（P0 表达式直读）；body 尾 `jmp +1; nop` 中继需 P223 goto 尾空标签形。
    if (m_delivery == 0)
        goto done;
    m_delivery->notify(type, msg);
done:;
}


// ORIG 每项 0x14 字节（_FindEventTable 0x811a15d / InsertEventTable 0x811a4d4 证据）：
//   +0x00 event_id / +0x04 apply_type / +0x08 CEventBase* / +0x0c name(char*) /
//   +0x10 explain(char*)。
struct EventTableEntry
{
    int id;              // +0x00
    int apply_type;      // +0x04
    CEventBase* base;    // +0x08
    const char* name;    // +0x0c（event_name）
    const char* explain; // +0x10（event_explain key/string）
};
// ORIG `nm -S $O` = `0936fa60 0000062c D events`：本 TU 文件作用域**全局**对象（裸名
// `events`，非 `_ZL`/匿名 namespace 局部），且位于 .data（**非 BSS**）——79 项的
// id/apply_type/name/explain 是编译期静态初值（objdump -j .data 逐字复核，与
// local://events_table.md 0 差异）；base(+0x08) 在 .data 全 0，由同 TU
// __static_initialization_and_destruction_0 @0x0811a7a4 启动期填充：实证恰 79 次
// `mov %eax,<events+off>`，off 全部 ≡8 (mod 0x14)、下标 0..78 连续无缺；78 项为
// `operator new(sizeof(CxxxEvent))` + `CxxxEvent::CxxxEvent()`（new 尺寸∈{12,16,20,24,
// 32,96}，逐条等于对应类 sizeof），idx76 唯一例外 = ARAD::Singleton<HeroMissionEvent>::
// GetPtr() 写 events[76].base（0x9370058）。ORIG sid_ 全程仅 1 次 __cxa_atexit（宿主为
// std::ios_base::Init），即 78 个 new 出的事件对象**不登记析构**（进程退出不清理）。
// ⇒ 以聚合初始化器内联 new 的形态复现：静态字段落 .data，base 动态初始化按声明序
// （下标 0..78）在 sid_ 逐项存储。explain 与 id **不一一对应**（如 idx23/24 为
// "event_msg_00"、idx27 "event_msg_33"、idx68 "event_msg_190"），必须逐条照抄。
// [SplitC batch10 · 档2] events[] 定义已迁出至 EventTable.cpp（ORIG TU `EventTable.cpp`
//   键 `_GLOBAL__I_events`）。本 TU 保留 struct 定义 + extern 声明供 Insert/Verify 使用。
extern EventTableEntry events[0x4f];

#include "CEventManager/CEventTable.inc"

#include "CEventManager/dtor_CEventTable.inc"

#include "CEventManager/GetEventTableCount.inc"


#include "CEventManager/GetEventBase.inc"


#include "CEventManager/GetEventId.inc"


#include "CEventManager/_FindEventTable.inc"


#include "CEventManager/VerifyEventTable.inc"


#include "CEventManager/InsertEventTable.inc"


#include "CEventManager/changeCharacName.inc"

