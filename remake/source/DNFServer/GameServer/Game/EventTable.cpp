// ============================================================================
// [SplitC batch10 · routeB_data 档2] 全局 events[] 独立 TU（ORIG `EventTable.cpp`）。
//   ORIG 键 = `_GLOBAL__I_events`；include 集由表内 79 项类型反推 + CEventTable.h。
//   构造序敏感（档2）：relink 后需 .init_array 顺序核对 + 启动冒烟。
// ============================================================================

#include "ARAD_Database.h"
#include "Arad_MomijiEvent.h"
#include "AttendanceEvent.h"
#include "BingoEvent.h"
#include "BlueMarbleDungeonEvent.h"
#include "CAradRyosikaEvent.h"
#include "CAssaultOnOffEvent.h"
#include "CBloodDungeonRewardFirstEvent.h"
#include "CBloodDungeonRewardSecondEvent.h"
#include "CBreakAwayPreventEvent.h"
#include "CBurnigGoldMonsterEvent.h"
#include "CBurningFatigueEvent.h"
#include "CCeraShopBonusItemEvent.h"
#include "CCharacterDayEvent.h"
#include "CClearRewardCardEvent.h"
#include "CCoinEventPerDay.h"
#include "CCoinRefillEvent.h"
#include "CCollectArchieveEventLog.h"
#include "CConditionEvent.h"
#include "CDeathTowerWinPointEvent.h"
#include "CDimensionActivationEvent.h"
#include "CDnFLeaguePromoteFirstEvent.h"
#include "CDnFLeaguePromoteSecondEvent.h"
#include "CDoubleGoldCardEvent.h"
#include "CEventAdvanceAltarOpen.h"
#include "CEventCeraShopRewardPoint.h"
#include "CEventCreateDnf.h"
#include "CEventStayTime.h"
#include "CEventTable.h"
#include "CExpDoubleEvent.h"
#include "CFatigueAttendance.h"
#include "CFatigueGiveItemEvent.h"
#include "CGmRegistEvent.h"
#include "CGoldCardBlankItemEvent.h"
#include "CGuildWarEvent.h"
#include "CIntegratedPvPServerMatchEvent.h"
#include "CItemDropRatioEvent.h"
#include "CLeadingChannelEvent.h"
#include "CMaxFatigueFactorEvent.h"
#include "CNoNeedGoldOnGuildCreateEvent.h"
#include "CNotApplyBalkeunEvent.h"
#include "COnTimeEvent.h"
#include "COnlinePreliminaryEvent.h"
#include "CPCRoomBurningEvent.h"
#include "CPCRoomFatigueEvent.h"
#include "CPCRoomPlayTimeEvent.h"
#include "CPCRoomWorldDropEvent.h"
#include "CPCRoomWorldDropEvent2nd.h"
#include "CPartyExpBonusEvent.h"
#include "CPcRoomCardBlankItemEvent.h"
#include "CPowerWarEvent.h"
#include "CPowerWarVictoriousEvent.h"
#include "CPremiumGoldCard.h"
#include "CPvPExpPenaltyEvent.h"
#include "CPvPLiveEvent.h"
#include "CReduceUpgradeItemPay.h"
#include "CReformingDanjinEvent.h"
#include "CRestrictCharacCreationEvent.h"
#include "CSchoolMatchEvent.h"
#include "CSecretShopEvent.h"
#include "CStabToDeathEvent.h"
#include "CStopOverlabExpEvent.h"
#include "CTournamentPvPEvent.h"
#include "CUXGameLogEvent.h"
#include "CUnlimitFatigueEvent.h"
#include "CVendingMachineBonusEvent.h"
#include "EventGiveMeBox.h"
#include "EventNewCharacterReward.h"
#include "GrowthEquipEvent.h"
#include "HeroMissionEvent.h"
#include "LevelUpBefore70LvEvent.h"
#include "LevelupSupport2ndEvent.h"
#include "LevelupSupportEvent.h"
#include "NewAccountLevelUpEventToJob.h"
#include "W8LT_EventTail.h"
#include "W9_EventSmall.h"

struct EventTableEntry
{
    int id;              // +0x00
    int apply_type;      // +0x04
    CEventBase* base;    // +0x08
    const char* name;    // +0x0c（event_name）
    const char* explain; // +0x10（event_explain key/string）
};

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
