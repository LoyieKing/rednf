// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CEventManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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
}

#include "CEventManager/CDelivery.inc"
}

#include "CEventManager/dtor_CDelivery.inc"
}

#include "CEventManager/checkAddress.inc"
}

// b28d-40: BOOST_FOREACH 复现 + 正向 `if (it != end)` 包裹（P220）；ORIG 0x081163fa，AE。
#include "CEventManager/attach.inc"
}

#include "CEventManager/detach.inc"
}

// b28d-solo-CEventManager: [N7 封顶] BOOST_FOREACH 复现 + 正向 `if (it != end)` 包裹
// （P220）；ORIG 0x08116682，144=144 助记符序列全等，唯一残差=间接 call 目标寄存器
// （ORIG %ecx vs 4.4.7 恒 %eax，P230 限定 4.4.7；4.4.6 后端可翻但禁改路由表）。
// 逐条归一化比对确认无其它差异；P282 复核 vtable 槽 +0x30=delivery 与头声明序一致。
// b28d-solo 双后端实测：4.4.7 NEAR / 4.4.6 后端 AE（N7 敏感族）。
// checkAddress 校验 → map.find → 遍历该 type 的 list，对非空 CEventBase 经
// vtable+0x30（= delivery 虚槽 12）调用 delivery(msg)。
#include "CEventManager/notify.inc"
}




#include "CEventManager/dtor_CEventManager.inc"
}

#include "CEventManager/AddEvent.inc"
}

#include "CEventManager/GetRepeatEvent.inc"
}

// ORIG T 0x08116212 _ZN13CEventManager30IsExistOverlappedExpBonusEventEv
// 事件 0x66（经验加成类）进行中，或全局效果管理器效果 2 生效 → 视为存在
// 重叠经验加成。
#include "CEventManager/IsExistOverlappedExpBonusEvent.inc"
}

// b28d-solo-CEventManager: [N7 封顶] ORIG 0x0811630e——虚调用目标 %ecx + 索引
// %edx/%eax vs 4.4.7 目标恒 %eax + 索引 %ecx/%edx（42=42 助记符序列全等，仅寄存器
// 角色轮转）；P282 复核 +0x34=IsEventing、+0x24=dailyresetData 槽与头声明序一致。
// b28d-solo 双后端实测：4.4.7 NEAR / 4.4.6 后端 AE（N7 敏感族）。
#include "CEventManager/dailyresetData.inc"
}

// b28d-solo-CEventManager: [N 封顶] P292 档① 已修——ORIG 两处调用点均
// `call; test %al,%al; je`（无 setne）⇒ 返回类型改 bool（TriggerEventStart/End
// 同步归位）。本体 ORIG 0x08115bf4：78 vs 79（round5：82→79，残 1 条）。P222
// 正向包裹 `if (channel <= 0x10) { … } return 0;` 已复现 ORIG 的 `ja <共享尾 mov$0>`
// 方向（旧 `if (channel >= 0x11) return 0;` 生反极 `jbe` + 独立 mov$0 块）。
// [round5] 本轮试形与实测（每形均 check_tu_routed 复算）：
//  ①纯 return 版（4 个独立 `mov $0` 块）= 82；②全 goto 尾标签 = 85（4.4 给每个
//    case 组各生 `nop;jmp` 岛：段1 4 岛 vs ORIG 1 岛）；③全 break+goto = 75
//    （case 直接跳、0 岛）；④**混合**（段1 break→1 岛，段2/段3 goto→3 岛/1 岛）
//    = **79**，岛数与 ORIG 的 1+3+1 逐段全等 ⇒ 取此形。
// 残余 1 条 = channel 寄存器角色（N7）：ORIG 的 register 值留 %eax
// （`cmp $0x10,%eax` / `mov %eax,%ecx`），4.4 把 register 变量落 %ebx ⇒ 多
// `mov %eax,%ebx`+`mov %ebx,%eax` 两条，而掩码区段1 少 `mov %eax,%edx` 复制、
// 段3 多 `mov %ebx,%eax` 恢复（净 +1）。已试并回退：去 register（槽化）、
// 三档类型旋钮（P248）、`==`/`!` 极性、goto/break 形。块体物理序 A/C/B（ORIG）
// vs A/B/C（我方）同属该族。
// [round5 C05 范式复检] 按 Main 转发 C05② 适用边界（常量比较 / 形参 / 跨调用返回值
// 三者均不适用）逐条核：本件两侧皆为**立即数寄存器比较**（`cmp $0x10,%eax` vs
// `cmp $0x10,%ebx`），无内存直比可换，既不属 C05② 命中形（需两个具名局部比较），
// 也非 C05①/③/④（无尾 fail 块、我方岛数已对齐、无 setne 段）⇒ 归 N7 寄存器角色族。
// P56 直分支形 + P133 独立守卫已应用。
// b28d-solo 双后端实测：4.4.7 DIFF / 4.4.6 后端 DIFF（后端不敏感）。
#include "CEventManager/filterEvent.inc"
}
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
}

#include "CEventManager/BroadcastEventInfo.inc"
}

#include "CEventManager/TriggerEventStart.inc"
}

#include "CEventManager/TriggerEventEnd.inc"
}

// ORIG 0x08114dcc（InitEventManager.md 反汇编逐槽核对）：按槽位映射堆构造 50
// 个事件对象（new + C1 + 写槽），成功后调 LoadEventFromDB() 并透传其返回值；
// 任何构造异常经 catch 直接返回 0（ORIG catch @0x08115965 只 begin/end_catch，
// 不回收已构造对象）。GLOBAL 键控槽（21/24/35/36/39/50/58/86/87/91/102/119/
// 158）在 ORIG 为静态单例 getter 返回指针，析构同样 delete——语义等价堆构造，
// 统一 new。槽位 = 偏移/4，映射如下：
//   1 CUnlimitFatigueEvent 2 CMaxFatigueFactorEvent 3 CExpDoubleEvent
//   4 CCoinEventPerDay 5 CCoinEventOnCharCreate 6 CLeadingChannelEvent
//   7 CItemDropRatioEvent 8 CPCRoomBurningEvent 33 CGuildWarEvent
//   10 CPCRoomFatigueEvent 12 CReformingDanjinEvent 13 CCoinRefillEvent
//   15 CBurningFatigueEvent 16 CClearRewardCardEvent 17 CCeraShopBonusItemEvent
//   18 CTournamentPvPEvent 19 CGoldCardBlankItemEvent 29 CPcRoomCardBlankItemEvent
//   30 CPowerWarEvent 21 CCollectArchieveEventLog 22 CPCRoomWorldDropEvent
//   24 CPartyExpBonusEvent 32 CStabToDeathEvent 9 CSchoolMatchEvent
//   34 CAutoMarketConditionsControlEvent 35 CVendingMachineBonusEvent
//   36 CBurnigGoldMonsterEvent 38 CNoNeedGoldOnGuildCreateEvent
//   39 CDeathTowerWinPointEvent 40 CNotApplyBalkeunEvent 41 CCharacterDayEvent
//   49 CRestrictCharacCreationEvent 50 CReduceUpgradeItemPay 51 COnTimeEvent
//   52 CBreakAwayPreventEvent 53 CPowerWarVictoriousEvent 58 CSecretShopEvent
//   86 CFatigueAttendance 87 CWeekendBonusEvent 113 CConditionEvent
//   91 CUXGameLogEvent 95 BlueMarbleDungeonEvent 102 CStopOverlabExpEvent
//   119 CEventAdvanceAltarOpen 116 CAradRyosikaEvent 155 Arad_MomijiEvent
//   158 LevelupSupportEvent 159 CEventStayTime 160 CEventCreateDnf
//   165 EventGiveMeBox
// §6.2 巨件核销：P167 153/153 [0缺0多]；gap 927→927。
// [round5 归零] 原残余尾部 7 条（缺 `xor $0x1,%eax` 段 + 两臂块序）。真因 =
// 「汇聚变量」：ORIG 的 if 在 **try 体内**、两臂写 4 字节 `mov $0x0/%0x1,%ebx`，
// catch 体同写 `mov $0x0,%ebx` 并 fall-through 到 0x08115977 `mov %ebx,%eax`
// ⇒ 变量为 **register int**（非 bool 局部：不占槽、不物化 movb），函数返回类型
// 为 **int**（尾块无 bool 归一化 setne，P9/P159）。改法：`register int result;`
// 裸声明（零发射）+ `if (!LoadEventFromDB()) result = 0; else result = 1;` 移入
// try 内 + `return result;`；同步 CGuildServerProxy.h 返回类型 bool→int
// （mangled 不含返回类型、全仓无调用点 ⇒ 零 ABI 风险）。实测 927=927（AE 归位，
// TU diff 3→2）。
#include "CEventManager/InitEventManager.inc"
}

#include "CEventManager/LoadEventFromDB.inc"
}

#include "CEventManager/eventDeliveryNotify.inc"
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
EventTableEntry events[0x4f] =
{
    { 1, 0, new CUnlimitFatigueEvent, "CUnlimitFatigueEvent", "event_msg_01" },
    { 2, 1, new CMaxFatigueFactorEvent, "CMaxFatigueFactorEvent", "event_msg_02" },
    { 3, 1, new CExpDoubleEvent, "CExpDoubleEvent", "event_msg_03" },
    { 4, 4, new CCoinEventPerDay, "CCoinEventPerDay", "event_msg_04" },
    { 5, 1, new CCoinEventOnCharCreate, "CCoinEventOnCharCreate", "event_msg_05" },
    { 6, 0, new CLeadingChannelEvent, "CLeadingChannelEvent", "event_msg_06" },
    { 7, 1, new CItemDropRatioEvent, "CItemDropRatioEvent", "event_msg_07" },
    { 8, 1, new CPCRoomBurningEvent, "CPCRoomBurningEvent", "event_msg_08" },
    { 9, 3, new CSchoolMatchEvent, "CSchoolMatchEvent", "event_msg_09" },
    { 10, 0, new CPCRoomFatigueEvent, "CPCRoomFatigueEvent", "event_msg_10" },
    { 12, 0, new CReformingDanjinEvent, "CReformingDanjinEvent", "event_msg_12" },
    { 13, 2, new CCoinRefillEvent, "CCoinRefillEvent", "event_msg_13" },
    { 15, 0, new CBurningFatigueEvent, "CBurningFatigueEvent", "event_msg_15" },
    { 16, 0, new CClearRewardCardEvent, "CClearRewardCardEvent", "event_msg_16" },
    { 17, 1, new CCeraShopBonusItemEvent, "CCeraShopBonusItemEvent", "event_msg_17" },
    { 18, 0, new CTournamentPvPEvent, "CTournamentPvPEvent", "event_msg_18" },
    { 19, 0, new CGoldCardBlankItemEvent, "CGoldCardBlankItemEvent", "event_msg_19" },
    { 21, 0, new CCollectArchieveEventLog, "CCollectArchieveEventLog", "event_msg_21" },
    { 22, 0, new CPCRoomWorldDropEvent, "CPCRoomWorldDropEvent", "event_msg_22" },
    { 24, 1, new CPartyExpBonusEvent, "CPartyExpBonusEvent", "event_msg_24" },
    { 29, 0, new CPcRoomCardBlankItemEvent, "CPcRoomCardBlankItemEvent", "event_msg_29" },
    { 30, 0, new CPowerWarEvent, "CPowerWarEvent", "event_msg_30" },
    { 32, 0, new CStabToDeathEvent, "CStabToDeathEvent", "event_msg_32" },
    { 33, 0, new CGuildWarEvent, "CGuildWarEvent", "event_msg_00" },
    { 34, 0, new CAutoMarketConditionsControlEvent, "CAutoMarketConditionsControlEvent", "event_msg_00" },
    { 35, 0, new CVendingMachineBonusEvent, "CVendingMachineBonusEvent", "event_msg_35" },
    { 36, 0, new CBurnigGoldMonsterEvent, "CBurnigGoldMonsterEvent", "event_msg_36" },
    { 38, 0, new CNoNeedGoldOnGuildCreateEvent, "CNoNeedGoldOnGuildCreateEvent", "event_msg_33" },
    { 39, 1, new CDeathTowerWinPointEvent, "CDeathTowerWinPointEvent", "event_msg_34" },
    { 40, 0, new CNotApplyBalkeunEvent, "CNotApplyBalkeunEvent", "event_msg_37" },
    { 41, 1, new CCharacterDayEvent, "CCharacterDayEvent", "event_msg_38" },
    { 42, 0, new CAssaultOnOffEvent, "CAssaultOnOffEvent", "event_msg_39" },
    { 43, 0, new CFatigueBuffEvent, "CFatigueBuffEvent", "event_msg_40" },
    { 45, 1, new CBloodDungeonRewardFirstEvent, "CBloodDungeonRewardFirstEvent", "event_msg_42" },
    { 46, 1, new CBloodDungeonRewardSecondEvent, "CBloodDungeonRewardSecondEvent", "event_msg_43" },
    { 48, 0, new CPCRoomWorldDropEvent2nd, "CPCRoomWorldDropEvent2nd", "event_msg_45" },
    { 49, 1, new CRestrictCharacCreationEvent, "CRestrictCharacCreationEvent", "event_msg_46" },
    { 50, 1, new CReduceUpgradeItemPay, "CReduceUpgradeItemPay", "event_msg_47" },
    { 51, 2, new COnTimeEvent, "COnTimeEvent", "event_msg_48" },
    { 52, 0, new CBreakAwayPreventEvent, "CBreakAwayPreventEvent", "event_msg_49" },
    { 53, 0, new CPowerWarVictoriousEvent, "CPowerWarVictoriousEvent", "event_msg_50" },
    { 54, 0, new CPvPExpPenaltyEvent, "CPvPExpPenaltyEvent", "event_msg_51" },
    { 55, 0, new CPvPLiveEvent, "CPvPLiveEvent", "event_msg_52" },
    { 56, 0, new CIntegratedPvPServerMatchEvent, "CIntegratedPvPServerMatchEvent", "event_msg_53" },
    { 58, 0, new CSecretShopEvent, "CSecretShopEvent", "event_msg_54" },
    { 57, 0, new COnlinePreliminaryEvent, "COnlinePreliminaryEvent", "event_msg_55" },
    { 61, 0, new CDnFLeaguePromoteFirstEvent, "CDnFLeaguePromoteFirstEvent", "event_msg_57" },
    { 62, 0, new CDnFLeaguePromoteSecondEvent, "CDnFLeaguePromoteSecondEvent", "event_msg_58" },
    { 64, 0, new CDoubleGoldCardEvent, "CDoubleGoldCardEvent", "event_msg_60" },
    { 65, 0, new CPremiumGoldCard, "CPremiumGoldCard", "event_msg_61" },
    { 67, 0, new CGmRegistEvent, "CGmRegistEvent", "event_msg_63" },
    { 86, 0, new CFatigueAttendance, "CFatigueAttendance", "event_msg_86" },
    { 87, 0, new CWeekendBonusEvent, "CWeekendBonusEvent", "event_msg_87" },
    { 91, 0, new CUXGameLogEvent, "CUXGameLogEvent", "event_msg_91" },
    { 92, 0, new CPCRoomPlayTimeEvent, "CPCRoomPlayTimeEvent", "event_msg_92" },
    { 93, 0, new LevelUpBefore70LvEvent, "LevelUpBefore70LvEvent", "event_msg_93" },
    { 94, 0, new CDimensionActivationEvent, "CDimensionActivationEvent", "event_msg_94" },
    { 95, 0, new BlueMarbleDungeonEvent, "BlueMarbleDungeonEvent", "event_msg_95" },
    { 96, 1, new AttendanceEvent, "AttendanceEvent", "event_msg_96" },
    { 100, 0, new GrowthEquipEvent, "GrowthEquipEvent", "event_msg_100" },
    { 101, 0, new CFatigueGiveItemEvent, "CFatigueGiveItemEvent", "event_msg_101" },
    { 102, 0, new CStopOverlabExpEvent, "CStopOverlabExpEvent", "event_msg_102" },
    { 103, 0, new GiveGrowCreatureEvent, "GiveGrowCreatureEvent", "event_msg_103" },
    { 104, 0, new NewAccountLevelUpEventToJob, "NewAccountLevelUpEventToJob", "event_msg_104" },
    { 105, 0, new SeriaRoomAniDecoEvent, "SeriaRoomAniDecoEvent", "event_msg_105" },
    { 106, 0, new BingoEvent, "BingoEvent", "event_msg_106" },
    { 109, 0, new OneADayItemShopEvent, "OneADayItemShopEvent", "event_msg_109" },
    { 113, 0, new CConditionEvent, "CConditionEvent", "event_msg_67" },
    { 116, 2, new CAradRyosikaEvent, "CAradRyosikaEvent", "event_msg_190" },
    { 119, 0, new CEventAdvanceAltarOpen, "CEventAdvanceAltarOpen", "event_msg_103" },
    { 155, 4, new Arad_MomijiEvent, "Arad_MomijiEvent", "event_msg_196" },
    { 158, 0, new LevelupSupportEvent, "LevelupSupportEvent", "event_msg_158" },
    { 159, 2, new CEventStayTime, "CEventStayTime", "event_msg_159" },
    { 160, 4, new CEventCreateDnf, "CEventCreateDnf", "event_msg_160" },
    { 161, 2, new CEventCeraShopRewardPoint, "CEventCeraShopRewardPoint", "event_msg_161" },
    { 162, 2, new EventNewCharacterReward, "EventNewCharacterReward", "event_msg_162" },
    { 164, 0, ARAD::Singleton<HeroMissionEvent>::GetPtr(), "HeroMissionEvent", "event_msg_163" },
    { 163, 0, new LevelupSupport2ndEvent, "LevelupSupport2ndEvent", "event_msg_164" },
    { 165, 0, new EventGiveMeBox, "EventGiveMeBox", "event_msg_165" },
};

#include "CEventManager/CEventTable.inc"
#include "CEventManager/dtor_CEventTable.inc"

#include "CEventManager/GetEventTableCount.inc"
}

#include "CEventManager/GetEventBase.inc"
}

#include "CEventManager/GetEventId.inc"
}

#include "CEventManager/_FindEventTable.inc"
}

// ORIG 0x811a1e0（0x2f4 字节）：经 DBMgr::GetDBHandle 查询
// dnf_event_info(event_id, event_name)，逐行 _FindEventTable 校验；把校验通过
// 的事件表索引记入集合，最后将「未在 DB 中」的事件表索引追加到 out，返回是否成功。
#include "CEventManager/VerifyEventTable.inc"
}

// ORIG 0x811a4d4（0x2d0 字节）：为空则直接返回 1；否则遍历 ids 中各事件表索引，
// 用事件表 name/explain 字段构造 "(%d,%d,'%s','%s')" 插入串（explain 经
// g_scriptStringManager_.findString(4, ...) 解析），一次性 INSERT 到
// dnf_event_info，exec 成功返回 1，出错码 0x426（诱骗重复）或失败返回 0。
#include "CEventManager/InsertEventTable.inc"
}

// ---------------------------------------------------------------------------
// LinkClosureB-2 追加：changeCharacName（ORIG T 0x08116276 sz 0x98，
// 报告 docs/class_func_reports/CEventManager/changeCharacName.md 全文已读）：
// 遍历事件数组 0..0xa5：槽非空且 IsEventing(0)（ORIG vptr+0x34 槽）为真 →
// 调该事件 vptr+0x20 槽（ORIG changeCharacName 槽，头内声明序与 ORIG vtable
// 对位：_ZTV10CEventBase @0x08b4bae0 实测 +0x20=changeCharacName、
// +0x34=IsEventing），实参 (user, key, name, flag)。
// b28d-solo-CEventManager: [N7 封顶] 同 dailyresetData——50=50 助记符序列全等，
// 残余=虚调用目标 %ecx vs 4.4.7 %eax + 索引寄存器轮转；P282 复核 +0x34=IsEventing、
// +0x20=changeCharacName 槽与头声明序一致。ORIG 0x08116276。
// b28d-solo 双后端实测：4.4.7 NEAR / 4.4.6 后端 IDENTICAL（N7 敏感族）。
#include "CEventManager/changeCharacName.inc"
}
