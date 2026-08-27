#include "W8LT_DBTask.h"
#include "DBDispatcher.h"
#include "Stream.h"
#include "StreamPool.h"
#include "GlobalData.h"        // s_msgq_mgr/s_stream_pool/s_big_stream_pool/
                               // s_small_stream_pool/s_db_mgr（GlobalData.h:159-164）
#include "SigTypes.h"        // SIG_SAVE_PC_ROOM_PLAY_TIME
#include "GameTypes.h"         // cMyTrace
#include "LogManager.h"
#include "DBMgr.h"             // DBMgr::setUserIdx（GlobalData::s_db_mgr 完整类型）

#include <sys/select.h>
#include <unistd.h>

// stPvPTempTable_t —— ORIG 仅存 W 析构 _ZN16stPvPTempTable_tD1Ev/D2Ev @0x08458e92
// （push/mov/pop/ret 空体，5 字节）及 vector<stPvPTempTable_t> 的 allocator 实例化；
// 二进制中无其它字段访问消费点。POD 最小还原。
struct stPvPTempTable_t
{
    ~stPvPTempTable_t();   // ORIG D1/D2 0x08458e92（空体）
};

namespace OS_API
{
void uSleep(unsigned int usec);   // ORIG 0x0858c4fa sz 0x40（select 微秒等待）
}

// ---------------------------------------------------------------------------
// DB_Q 消息 dispatcher 类型集（329 个派生自 DBDispatcher 的空类型）
// ---------------------------------------------------------------------------

// ---- 全局作用域 ----
class DB_Login : public DBDispatcher { public: DB_Login() {} };   // ORIG C1 0x840f48e (sz 0x4: base ctor+vptr only)
class DB_CreateCharac : public DBDispatcher { public: DB_CreateCharac() {} };   // ORIG C1 0x8400cc2 (sz 0x4: base ctor+vptr only)
class DB_DeleteCharac : public DBDispatcher { public: DB_DeleteCharac() {} };   // ORIG C1 0x8402bce (sz 0x4: base ctor+vptr only)
class DB_UpdateCharacView : public DBDispatcher { public: DB_UpdateCharacView() {} };   // ORIG C1 0x844d85c (sz 0x4: base ctor+vptr only)
class DB_LoadEtc : public DBDispatcher { public: DB_LoadEtc() {} };   // ORIG C1 0x8403b84 (sz 0x4: base ctor+vptr only)
class DB_SaveInven : public DBDispatcher { public: DB_SaveInven() {} };   // ORIG C1 0x8416a42 (sz 0x4: base ctor+vptr only)
class DB_SaveSkill : public DBDispatcher { public: DB_SaveSkill() {} };   // ORIG C1 0x8417770 (sz 0x4: base ctor+vptr only)
class DB_SavePvp : public DBDispatcher { public: DB_SavePvp() {} };   // ORIG C1 0x841720e (sz 0x4: base ctor+vptr only)
class DB_UpdateQuest : public DBDispatcher { public: DB_UpdateQuest() {} };   // ORIG C1 0x8418b74 (sz 0x4: base ctor+vptr only)
class DB_UpdateGrowType : public DBDispatcher { public: DB_UpdateGrowType() {} };   // ORIG C1 0x8418130 (sz 0x4: base ctor+vptr only)
class DB_UpdateLogin : public DBDispatcher { public: DB_UpdateLogin() {} };   // ORIG C1 0x844da04 (sz 0x4: base ctor+vptr only)
class DB_UpdateLogout : public DBDispatcher { public: DB_UpdateLogout() {} };   // ORIG C1 0x84183d8 (sz 0x4: base ctor+vptr only)
class DB_UpdateChannel : public DBDispatcher { public: DB_UpdateChannel() {} };   // ORIG C1 0x8417f4a (sz 0x4: base ctor+vptr only)
class DB_DeleteChannel : public DBDispatcher { public: DB_DeleteChannel() {} };   // ORIG C1 0x8402892 (sz 0x4: base ctor+vptr only)
class DB_InsertChannel : public DBDispatcher { public: DB_InsertChannel() {} };   // ORIG C1 0x840364c (sz 0x4: base ctor+vptr only)
class DB_AvatarItemInsert : public DBDispatcher { public: DB_AvatarItemInsert() {} };   // ORIG C1 0x83ffd50 (sz 0x4: base ctor+vptr only)
class DB_AvatarItemDel : public DBDispatcher { public: DB_AvatarItemDel() {} };   // ORIG C1 0x83ffc42 (sz 0x4: base ctor+vptr only)
class DB_AvatarItemMove : public DBDispatcher { public: DB_AvatarItemMove() {} };   // ORIG C1 0x840031e (sz 0x4: base ctor+vptr only)
class DB_AvatarExtendPeriod : public DBDispatcher { public: DB_AvatarExtendPeriod() {} };   // ORIG C1 0x83ffab8 (sz 0x4: base ctor+vptr only)
class DB_AvatarChangeOwner : public DBDispatcher { public: DB_AvatarChangeOwner() {} };   // ORIG C1 0x844d7f2 (sz 0x4: base ctor+vptr only)
class DB_AvatarChangeStat : public DBDispatcher { public: DB_AvatarChangeStat() {} };   // ORIG C1 0x844e0aa (sz 0x4: base ctor+vptr only)
class DB_SaveRank : public DBDispatcher { public: DB_SaveRank() {} };   // ORIG C1 0x844d99a (sz 0x4: base ctor+vptr only)
class DB_InsertTicket : public DBDispatcher { public: DB_InsertTicket() {} };   // ORIG C1 0x8403a4e (sz 0x4: base ctor+vptr only)
class DB_LoadTicket : public DBDispatcher { public: DB_LoadTicket() {} };   // ORIG C1 0x8419572 (sz 0x4: base ctor+vptr only)
class DB_LoadDungeonClear : public DBDispatcher { public: DB_LoadDungeonClear() {} };   // ORIG C1 0x8419852 (sz 0x4: base ctor+vptr only)
class DB_SaveDungeonClear : public DBDispatcher { public: DB_SaveDungeonClear() {} };   // ORIG C1 0x8419fec (sz 0x4: base ctor+vptr only)
class DB_TerminateServer : public DBDispatcher { public: DB_TerminateServer() {} };   // ORIG C1 0x841a284 (sz 0x4: base ctor+vptr only)
class DB_StatBreakJar : public DBDispatcher { public: DB_StatBreakJar() {} };   // ORIG C1 0x8417a12 (sz 0x4: base ctor+vptr only)
class DB_StatComposition : public DBDispatcher { public: DB_StatComposition() {} };   // ORIG C1 0x8417b82 (sz 0x4: base ctor+vptr only)
class DB_StatCoinUse : public DBDispatcher { public: DB_StatCoinUse() {} };   // ORIG C1 0x8417b04 (sz 0x4: base ctor+vptr only)
class DB_StatPvpPlay : public DBDispatcher { public: DB_StatPvpPlay() {} };   // ORIG C1 0x8417e58 (sz 0x4: base ctor+vptr only)
class DB_StatGameRank : public DBDispatcher { public: DB_StatGameRank() {} };   // ORIG C1 0x8417d66 (sz 0x4: base ctor+vptr only)
class DB_StatDungeonClear : public DBDispatcher { public: DB_StatDungeonClear() {} };   // ORIG C1 0x8417c74 (sz 0x4: base ctor+vptr only)
class DB_CrashDownReport : public DBDispatcher { public: DB_CrashDownReport() {} };   // ORIG C1 0x844e0c6 (sz 0x4: base ctor+vptr only)
class DB_SaveFeaturedGoodsIdx : public DBDispatcher { public: DB_SaveFeaturedGoodsIdx() {} };   // ORIG C1 0x844e0e2 (sz 0x4: base ctor+vptr only)
class DB_RemoveFeaturedGoodsIdx : public DBDispatcher { public: DB_RemoveFeaturedGoodsIdx() {} };   // ORIG C1 0x844e0fe (sz 0x4: base ctor+vptr only)
class DB_GenCeraTicket : public DBDispatcher { public: DB_GenCeraTicket() {} };   // ORIG C1 0x844da6e (sz 0x4: base ctor+vptr only)
class DB_SaveEventItem : public DBDispatcher { public: DB_SaveEventItem() {} };   // ORIG C1 0x844dad8 (sz 0x4: base ctor+vptr only)
class DB_RemoveEventItem : public DBDispatcher { public: DB_RemoveEventItem() {} };   // ORIG C1 0x844db42 (sz 0x4: base ctor+vptr only)
class DB_FatigueResetTimer : public DBDispatcher { public: DB_FatigueResetTimer() {} };   // ORIG C1 0x844dbac (sz 0x4: base ctor+vptr only)
class DB_SuicideWhenQueueIsEmpty : public DBDispatcher { public: DB_SuicideWhenQueueIsEmpty() {} };   // ORIG C1 0x844e11a (sz 0x4: base ctor+vptr only)
class DB_SavePremiumInfo : public DBDispatcher { public: DB_SavePremiumInfo() {} };   // ORIG C1 0x844e136 (sz 0x4: base ctor+vptr only)
class DB_SaveCharacInfo4Guild : public DBDispatcher { public: DB_SaveCharacInfo4Guild() {} };   // ORIG C1 0x844e152 (sz 0x4: base ctor+vptr only)
class DB_StatDungeonFail : public DBDispatcher { public: DB_StatDungeonFail() {} };   // ORIG C1 0x844e16e (sz 0x4: base ctor+vptr only)
class DB_StatPVPPlay2 : public DBDispatcher { public: DB_StatPVPPlay2() {} };   // ORIG C1 0x844e18a (sz 0x4: base ctor+vptr only)
class DB_StatUseShusiaService : public DBDispatcher { public: DB_StatUseShusiaService() {} };   // ORIG C1 0x844e1a6 (sz 0x4: base ctor+vptr only)
class DB_UpdateDetailChannel : public DBDispatcher { public: DB_UpdateDetailChannel() {} };   // ORIG C1 0x844e1c2 (sz 0x4: base ctor+vptr only)
class DB_SaveChannelUserLevel : public DBDispatcher { public: DB_SaveChannelUserLevel() {} };   // ORIG C1 0x844e1de (sz 0x4: base ctor+vptr only)
class DB_SavePrivateStore : public DBDispatcher { public: DB_SavePrivateStore() {} };   // ORIG C1 0x844e1fa (sz 0x4: base ctor+vptr only)
class DB_LoadPrivateStore : public DBDispatcher { public: DB_LoadPrivateStore() {} };   // ORIG C1 0x844e216 (sz 0x4: base ctor+vptr only)
class DB_SaveGamechannel : public DBDispatcher { public: DB_SaveGamechannel() {} };   // ORIG C1 0x844e232 (sz 0x4: base ctor+vptr only)
class DB_UpdateQueryCountTimerLogdb : public DBDispatcher { public: DB_UpdateQueryCountTimerLogdb() {} };   // ORIG C1 0x844e24e (sz 0x4: base ctor+vptr only)
class DB_UpdateQueryCountTimerDb : public DBDispatcher { public: DB_UpdateQueryCountTimerDb() {} };   // ORIG C1 0x844e26a (sz 0x4: base ctor+vptr only)
class DB_UpdateQueryCountDbLogdb : public DBDispatcher { public: DB_UpdateQueryCountDbLogdb() {} };   // ORIG C1 0x844e286 (sz 0x4: base ctor+vptr only)
class DB_UpdateNatType : public DBDispatcher { public: DB_UpdateNatType() {} };   // ORIG C1 0x844e2a2 (sz 0x4: base ctor+vptr only)
class DB_UpdateNatTypeUser : public DBDispatcher { public: DB_UpdateNatTypeUser() {} };   // ORIG C1 0x844e2be (sz 0x4: base ctor+vptr only)
class DB_MailBox_Req_CheckReceiver : public DBDispatcher { public: DB_MailBox_Req_CheckReceiver() {} };   // ORIG C1 0x844e2da (sz 0x4: base ctor+vptr only)
class DB_MailBox_Req_List : public DBDispatcher { public: DB_MailBox_Req_List() {} };   // ORIG C1 0x844e2f6 (sz 0x4: base ctor+vptr only)
class DB_MailBox_Req_Delete : public DBDispatcher { public: DB_MailBox_Req_Delete() {} };   // ORIG C1 0x844e312 (sz 0x4: base ctor+vptr only)
class DB_MailBox_Req_Send : public DBDispatcher { public: DB_MailBox_Req_Send() {} };   // ORIG C1 0x844e32e (sz 0x4: base ctor+vptr only)
class DB_InsertCreatureItem : public DBDispatcher { public: DB_InsertCreatureItem() {} };   // ORIG C1 0x844e34a (sz 0x4: base ctor+vptr only)
class DB_UpdateCreatureItemList : public DBDispatcher { public: DB_UpdateCreatureItemList() {} };   // ORIG C1 0x844e366 (sz 0x4: base ctor+vptr only)
class DB_UpdateCreatureItem : public DBDispatcher { public: DB_UpdateCreatureItem() {} };   // ORIG C1 0x844e382 (sz 0x4: base ctor+vptr only)
class DB_SwapCreatureItem : public DBDispatcher { public: DB_SwapCreatureItem() {} };   // ORIG C1 0x844e39e (sz 0x4: base ctor+vptr only)
class DB_GatheringParyStatus : public DBDispatcher { public: DB_GatheringParyStatus() {} };   // ORIG C1 0x844e3ba (sz 0x4: base ctor+vptr only)
class DB_ForbidUserToPlay : public DBDispatcher { public: DB_ForbidUserToPlay() {} };   // ORIG C1 0x844e3d6 (sz 0x4: base ctor+vptr only)
class DB_LastMsgDb : public DBDispatcher { public: DB_LastMsgDb() {} };   // ORIG C1 0x844e3f2 (sz 0x4: base ctor+vptr only)
class DB_LastMsgLogDb : public DBDispatcher { public: DB_LastMsgLogDb() {} };   // ORIG C1 0x844e40e (sz 0x4: base ctor+vptr only)
class DB_SaveClientHackLog : public DBDispatcher { public: DB_SaveClientHackLog() {} };   // ORIG C1 0x844e42a (sz 0x4: base ctor+vptr only)
class DB_InsertQueueSizeLog : public DBDispatcher { public: DB_InsertQueueSizeLog() {} };   // ORIG C1 0x844e446 (sz 0x4: base ctor+vptr only)
class DB_SaveSameMIDOtherCharLoginHack : public DBDispatcher { public: DB_SaveSameMIDOtherCharLoginHack() {} };   // ORIG C1 0x844e462 (sz 0x4: base ctor+vptr only)
class DB_SaveProxyLog : public DBDispatcher { public: DB_SaveProxyLog() {} };   // ORIG C1 0x844e47e (sz 0x4: base ctor+vptr only)
class DB_SaveGoldZeroTradeLog : public DBDispatcher { public: DB_SaveGoldZeroTradeLog() {} };   // ORIG C1 0x844e49a (sz 0x4: base ctor+vptr only)
class DB_MouseRegister : public DBDispatcher { public: DB_MouseRegister() {} };   // ORIG C1 0x844e4b6 (sz 0x4: base ctor+vptr only)
class DB_PassPadUpdateFailCnt : public DBDispatcher { public: DB_PassPadUpdateFailCnt() {} };   // ORIG C1 0x844e4d2 (sz 0x4: base ctor+vptr only)
class DB_PassPadUpdateCancelCnt : public DBDispatcher { public: DB_PassPadUpdateCancelCnt() {} };   // ORIG C1 0x844e4ee (sz 0x4: base ctor+vptr only)
class DB_UpdateDungeonDataStatisticLogdb : public DBDispatcher { public: DB_UpdateDungeonDataStatisticLogdb() {} };   // ORIG C1 0x844e50a (sz 0x4: base ctor+vptr only)
class DB_BillingRecord : public DBDispatcher { public: DB_BillingRecord() {} };   // ORIG C1 0x844e526 (sz 0x4: base ctor+vptr only)
class DB_ReqBlackCount : public DBDispatcher { public: DB_ReqBlackCount() {} };   // ORIG C1 0x844e542 (sz 0x4: base ctor+vptr only)
class DB_ReqSaveCrazyLevelupEvent : public DBDispatcher { public: DB_ReqSaveCrazyLevelupEvent() {} };   // ORIG C1 0x844e55e (sz 0x4: base ctor+vptr only)
class DB_ReqSaveHackUserPunish : public DBDispatcher { public: DB_ReqSaveHackUserPunish() {} };   // ORIG C1 0x844e57a (sz 0x4: base ctor+vptr only)
class DB_InsertUserPlayTime : public DBDispatcher { public: DB_InsertUserPlayTime() {} };   // ORIG C1 0x844e596 (sz 0x4: base ctor+vptr only)
class DB_UpdateHappyBeanInfo : public DBDispatcher { public: DB_UpdateHappyBeanInfo() {} };   // ORIG C1 0x844e5b2 (sz 0x4: base ctor+vptr only)
class DB_InsertQueryCounterDb : public DBDispatcher { public: DB_InsertQueryCounterDb() {} };   // ORIG C1 0x844e5ce (sz 0x4: base ctor+vptr only)
class DB_InsertQueryCounterLogdb : public DBDispatcher { public: DB_InsertQueryCounterLogdb() {} };   // ORIG C1 0x844e5ea (sz 0x4: base ctor+vptr only)
class DB_ReqItemUpgradeLog : public DBDispatcher { public: DB_ReqItemUpgradeLog() {} };   // ORIG C1 0x844e606 (sz 0x4: base ctor+vptr only)
class DB_ChangeLetterStat : public DBDispatcher { public: DB_ChangeLetterStat() {} };   // ORIG C1 0x844e622 (sz 0x4: base ctor+vptr only)
class DB_ReqChangeCharacName : public DBDispatcher { public: DB_ReqChangeCharacName() {} };   // ORIG C1 0x844e63e (sz 0x4: base ctor+vptr only)
class DB_ReqQueryCharacInfo : public DBDispatcher { public: DB_ReqQueryCharacInfo() {} };   // ORIG C1 0x844e65a (sz 0x4: base ctor+vptr only)
class DB_MailBoxSendNewMailWithDBWork : public DBDispatcher { public: DB_MailBoxSendNewMailWithDBWork() {} };   // ORIG C1 0x844e676 (sz 0x4: base ctor+vptr only)
class DB_ReportMannerlessUser_Upgrade : public DBDispatcher { public: DB_ReportMannerlessUser_Upgrade() {} };   // ORIG C1 0x844e692 (sz 0x4: base ctor+vptr only)
class DB_ReportMannerlessUser : public DBDispatcher { public: DB_ReportMannerlessUser() {} };   // ORIG C1 0x844e6ae (sz 0x4: base ctor+vptr only)
class DB_UpdateClientCrashDownLog : public DBDispatcher { public: DB_UpdateClientCrashDownLog() {} };   // ORIG C1 0x844e6ca (sz 0x4: base ctor+vptr only)
class DB_TimerStatisticsLog : public DBDispatcher { public: DB_TimerStatisticsLog() {} };   // ORIG C1 0x844e6e6 (sz 0x4: base ctor+vptr only)
class DB_SaveUserHandicap : public DBDispatcher { public: DB_SaveUserHandicap() {} };   // ORIG C1 0x844e702 (sz 0x4: base ctor+vptr only)
class DB_ForceSaveSimpleSSO : public DBDispatcher { public: DB_ForceSaveSimpleSSO() {} };   // ORIG C1 0x844e71e (sz 0x4: base ctor+vptr only)
class DB_CountOfHumanCertify : public DBDispatcher { public: DB_CountOfHumanCertify() {} };   // ORIG C1 0x844e73a (sz 0x4: base ctor+vptr only)
class DB_UpdateGiftCount : public DBDispatcher { public: DB_UpdateGiftCount() {} };   // ORIG C1 0x844e756 (sz 0x4: base ctor+vptr only)
class DB_UpdateMemberDoubtTrade : public DBDispatcher { public: DB_UpdateMemberDoubtTrade() {} };   // ORIG C1 0x844e772 (sz 0x4: base ctor+vptr only)
class DB_UpdateItemDropStatistics : public DBDispatcher { public: DB_UpdateItemDropStatistics() {} };   // ORIG C1 0x844e78e (sz 0x4: base ctor+vptr only)
class DB_UpdatePvPGrade : public DBDispatcher { public: DB_UpdatePvPGrade() {} };   // ORIG C1 0x844e7aa (sz 0x4: base ctor+vptr only)
class DB_InsertValueStatistic : public DBDispatcher { public: DB_InsertValueStatistic() {} };   // ORIG C1 0x844e7c6 (sz 0x4: base ctor+vptr only)
class DB_InsertDungeonEntrance : public DBDispatcher { public: DB_InsertDungeonEntrance() {} };   // ORIG C1 0x844e7e2 (sz 0x4: base ctor+vptr only)
class DB_InsertDungeonEntrance_hour : public DBDispatcher { public: DB_InsertDungeonEntrance_hour() {} };   // ORIG C1 0x844e7fe (sz 0x4: base ctor+vptr only)
class DB_GetDeathTowerBestRecord : public DBDispatcher { public: DB_GetDeathTowerBestRecord() {} };   // ORIG C1 0x844e81a (sz 0x4: base ctor+vptr only)
class DB_UpdateDeathTowerBestRecord : public DBDispatcher { public: DB_UpdateDeathTowerBestRecord() {} };   // ORIG C1 0x844e836 (sz 0x4: base ctor+vptr only)
class DB_DeathTowerLoadTopRanker : public DBDispatcher { public: DB_DeathTowerLoadTopRanker() {} };   // ORIG C1 0x844e852 (sz 0x4: base ctor+vptr only)
class DB_CheckDoubleGuildName : public DBDispatcher { public: DB_CheckDoubleGuildName() {} };   // ORIG C1 0x844e86e (sz 0x4: base ctor+vptr only)
class DB_CheckDoubleGuildAddress : public DBDispatcher { public: DB_CheckDoubleGuildAddress() {} };   // ORIG C1 0x844e88a (sz 0x4: base ctor+vptr only)
class DB_ArrangeUserLoginInfo : public DBDispatcher { public: DB_ArrangeUserLoginInfo() {} };   // ORIG C1 0x844e8a6 (sz 0x4: base ctor+vptr only)
class DB_SecurityCardIssue : public DBDispatcher { public: DB_SecurityCardIssue() {} };   // ORIG C1 0x844e8c2 (sz 0x4: base ctor+vptr only)
class DB_SecurityCardDisuse : public DBDispatcher { public: DB_SecurityCardDisuse() {} };   // ORIG C1 0x844e8de (sz 0x4: base ctor+vptr only)
class DB_SecurityCardActivate : public DBDispatcher { public: DB_SecurityCardActivate() {} };   // ORIG C1 0x844e8fa (sz 0x4: base ctor+vptr only)
class DB_SecurityCardUpdateValidityTime : public DBDispatcher { public: DB_SecurityCardUpdateValidityTime() {} };   // ORIG C1 0x844e916 (sz 0x4: base ctor+vptr only)
class DB_SecurityCardUpdateFailCnt : public DBDispatcher { public: DB_SecurityCardUpdateFailCnt() {} };   // ORIG C1 0x844e932 (sz 0x4: base ctor+vptr only)
class DB_SecurityCardUpdateCancelCnt : public DBDispatcher { public: DB_SecurityCardUpdateCancelCnt() {} };   // ORIG C1 0x844e94e (sz 0x4: base ctor+vptr only)
class DB_SecurityCardRetransfer : public DBDispatcher { public: DB_SecurityCardRetransfer() {} };   // ORIG C1 0x844e96a (sz 0x4: base ctor+vptr only)
class DB_InsertPartyType : public DBDispatcher { public: DB_InsertPartyType() {} };   // ORIG C1 0x844e986 (sz 0x4: base ctor+vptr only)
class DB_InsertPVPPlayerRate : public DBDispatcher { public: DB_InsertPVPPlayerRate() {} };   // ORIG C1 0x844e9a2 (sz 0x4: base ctor+vptr only)
class DB_InsertPVPMap : public DBDispatcher { public: DB_InsertPVPMap() {} };   // ORIG C1 0x844e9be (sz 0x4: base ctor+vptr only)
class DB_LoadDnfHackLog : public DBDispatcher { public: DB_LoadDnfHackLog() {} };   // ORIG C1 0x844e9da (sz 0x4: base ctor+vptr only)
class DB_InsertLevelDungeonPlay : public DBDispatcher { public: DB_InsertLevelDungeonPlay() {} };   // ORIG C1 0x844e9f6 (sz 0x4: base ctor+vptr only)
class DB_ReloadAutoPunishRuleBackIP : public DBDispatcher { public: DB_ReloadAutoPunishRuleBackIP() {} };   // ORIG C1 0x844ea12 (sz 0x4: base ctor+vptr only)
class DB_ReloadAutoPunishRuleHackType : public DBDispatcher { public: DB_ReloadAutoPunishRuleHackType() {} };   // ORIG C1 0x844ea2e (sz 0x4: base ctor+vptr only)
class DB_InsertAutoPunishFirstUser : public DBDispatcher { public: DB_InsertAutoPunishFirstUser() {} };   // ORIG C1 0x844ea4a (sz 0x4: base ctor+vptr only)
class DB_InsertAutoPunishSecondUser : public DBDispatcher { public: DB_InsertAutoPunishSecondUser() {} };   // ORIG C1 0x844ea66 (sz 0x4: base ctor+vptr only)
class DB_BlackIPMonitor : public DBDispatcher { public: DB_BlackIPMonitor() {} };   // ORIG C1 0x844ea82 (sz 0x4: base ctor+vptr only)
class DB_SaveCleanpadPoint : public DBDispatcher { public: DB_SaveCleanpadPoint() {} };   // ORIG C1 0x844ea9e (sz 0x4: base ctor+vptr only)
class DB_BlackIPMonitorPartLoad : public DBDispatcher { public: DB_BlackIPMonitorPartLoad() {} };   // ORIG C1 0x844eaba (sz 0x4: base ctor+vptr only)
class DB_Insert0712WinterEventCoupon : public DBDispatcher { public: DB_Insert0712WinterEventCoupon() {} };   // ORIG C1 0x844ead6 (sz 0x4: base ctor+vptr only)
class DB_InsertArchieveEventLog : public DBDispatcher { public: DB_InsertArchieveEventLog() {} };   // ORIG C1 0x844eaf2 (sz 0x4: base ctor+vptr only)
class DB_InsertDungeonExp : public DBDispatcher { public: DB_InsertDungeonExp() {} };   // ORIG C1 0x844eb0e (sz 0x4: base ctor+vptr only)
class DB_WriteGeoReject : public DBDispatcher { public: DB_WriteGeoReject() {} };   // ORIG C1 0x844eb2a (sz 0x4: base ctor+vptr only)
class DB_InsertUnlimitSupportLog : public DBDispatcher { public: DB_InsertUnlimitSupportLog() {} };   // ORIG C1 0x844eb46 (sz 0x4: base ctor+vptr only)
class DB_LoadCleanPadPoint : public DBDispatcher { public: DB_LoadCleanPadPoint() {} };   // ORIG C1 0x844eb62 (sz 0x4: base ctor+vptr only)
class DB_UpdateSchoolPoint : public DBDispatcher { public: DB_UpdateSchoolPoint() {} };   // ORIG C1 0x844eb7e (sz 0x4: base ctor+vptr only)
class DB_SaveAccountInfo : public DBDispatcher { public: DB_SaveAccountInfo() {} };   // ORIG C1 0x844eb9a (sz 0x4: base ctor+vptr only)
class DB_GoblinPadUpdateReward : public DBDispatcher { public: DB_GoblinPadUpdateReward() {} };   // ORIG C1 0x844ebb6 (sz 0x4: base ctor+vptr only)
class DB_GoblinPadUpdateValidityTime : public DBDispatcher { public: DB_GoblinPadUpdateValidityTime() {} };   // ORIG C1 0x844ebd2 (sz 0x4: base ctor+vptr only)
class DB_LoadGameOption : public DBDispatcher { public: DB_LoadGameOption() {} };   // ORIG C1 0x844ebee (sz 0x4: base ctor+vptr only)
class DB_SaveGameOption_1 : public DBDispatcher { public: DB_SaveGameOption_1() {} };   // ORIG C1 0x844ec0a (sz 0x4: base ctor+vptr only)
class DB_SaveGameOption_2 : public DBDispatcher { public: DB_SaveGameOption_2() {} };   // ORIG C1 0x844ec26 (sz 0x4: base ctor+vptr only)
class DB_SaveCharacterOption : public DBDispatcher { public: DB_SaveCharacterOption() {} };   // ORIG C1 0x844ec42 (sz 0x4: base ctor+vptr only)
class DB_UpdateCoolTimeItem : public DBDispatcher { public: DB_UpdateCoolTimeItem() {} };   // ORIG C1 0x844ec5e (sz 0x4: base ctor+vptr only)
class DB_UpdateEffectItem : public DBDispatcher { public: DB_UpdateEffectItem() {} };   // ORIG C1 0x844ec7a (sz 0x4: base ctor+vptr only)
class DB_UpdateAvatarJewelSlot : public DBDispatcher { public: DB_UpdateAvatarJewelSlot() {} };   // ORIG C1 0x844ec96 (sz 0x4: base ctor+vptr only)
class DB_UpdatePvPResult : public DBDispatcher { public: DB_UpdatePvPResult() {} };   // ORIG C1 0x844ecb2 (sz 0x4: base ctor+vptr only)
class DB_UpdatePvPLastPlayTime : public DBDispatcher { public: DB_UpdatePvPLastPlayTime() {} };   // ORIG C1 0x844ecce (sz 0x4: base ctor+vptr only)
class DB_LoadBloodBestRecord : public DBDispatcher { public: DB_LoadBloodBestRecord() {} };   // ORIG C1 0x844ecea (sz 0x4: base ctor+vptr only)
class DB_UpdateBloodBestRecord : public DBDispatcher { public: DB_UpdateBloodBestRecord() {} };   // ORIG C1 0x844ed06 (sz 0x4: base ctor+vptr only)
class DB_LoadBreakAwayPreventData : public DBDispatcher { public: DB_LoadBreakAwayPreventData() {} };   // ORIG C1 0x844ed22 (sz 0x4: base ctor+vptr only)
class DB_LoadBreakAwayRewardData : public DBDispatcher { public: DB_LoadBreakAwayRewardData() {} };   // ORIG C1 0x844ed3e (sz 0x4: base ctor+vptr only)
class DB_UpdateBreakAwayAccruedCera : public DBDispatcher { public: DB_UpdateBreakAwayAccruedCera() {} };   // ORIG C1 0x844ed5a (sz 0x4: base ctor+vptr only)
class DB_UpdateBreakAwayUvList : public DBDispatcher { public: DB_UpdateBreakAwayUvList() {} };   // ORIG C1 0x844ed76 (sz 0x4: base ctor+vptr only)
class DB_UpdateBreakAwayRuckPoint : public DBDispatcher { public: DB_UpdateBreakAwayRuckPoint() {} };   // ORIG C1 0x844ed92 (sz 0x4: base ctor+vptr only)
class DB_UpdateBreakAwayClearAddInfo : public DBDispatcher { public: DB_UpdateBreakAwayClearAddInfo() {} };   // ORIG C1 0x844edae (sz 0x4: base ctor+vptr only)
class DB_UpdateBreakAwayDungeonClearCnt : public DBDispatcher { public: DB_UpdateBreakAwayDungeonClearCnt() {} };   // ORIG C1 0x844edca (sz 0x4: base ctor+vptr only)
class DB_UpdateGMData : public DBDispatcher { public: DB_UpdateGMData() {} };   // ORIG C1 0x844ede6 (sz 0x4: base ctor+vptr only)
class DB_LoadDimension : public DBDispatcher { public: DB_LoadDimension() {} };   // ORIG C1 0x844ee02 (sz 0x4: base ctor+vptr only)
class DB_UpdateDimension : public DBDispatcher { public: DB_UpdateDimension() {} };   // ORIG C1 0x844ee1e (sz 0x4: base ctor+vptr only)
class DB_InsertCirculationStatistic : public DBDispatcher { public: DB_InsertCirculationStatistic() {} };   // ORIG C1 0x844ee3a (sz 0x4: base ctor+vptr only)
class DB_LoadAutoMarketConditionsControl : public DBDispatcher { public: DB_LoadAutoMarketConditionsControl() {} };   // ORIG C1 0x844ee56 (sz 0x4: base ctor+vptr only)
class DB_UpdateAutoMarketConditionsControl : public DBDispatcher { public: DB_UpdateAutoMarketConditionsControl() {} };   // ORIG C1 0x844ee72 (sz 0x4: base ctor+vptr only)
class DB_UpdateOneDayLetheSkill : public DBDispatcher { public: DB_UpdateOneDayLetheSkill() {} };   // ORIG C1 0x844ee8e (sz 0x4: base ctor+vptr only)
class DB_UpdateVendingMachineStatistic : public DBDispatcher { public: DB_UpdateVendingMachineStatistic() {} };   // ORIG C1 0x844eeaa (sz 0x4: base ctor+vptr only)
class DB_LoadPowerWarStatueRanker : public DBDispatcher { public: DB_LoadPowerWarStatueRanker() {} };   // ORIG C1 0x844eec6 (sz 0x4: base ctor+vptr only)
class DB_LoadPowerWarStatueInfo : public DBDispatcher { public: DB_LoadPowerWarStatueInfo() {} };   // ORIG C1 0x844eee2 (sz 0x4: base ctor+vptr only)
class DB_BlackipPrivateSecurityValuation : public DBDispatcher { public: DB_BlackipPrivateSecurityValuation() {} };   // ORIG C1 0x844eefe (sz 0x4: base ctor+vptr only)
class DB_UpdateServerMessage : public DBDispatcher { public: DB_UpdateServerMessage() {} };   // ORIG C1 0x844ef1a (sz 0x4: base ctor+vptr only)
class DB_LoadServerMessage : public DBDispatcher { public: DB_LoadServerMessage() {} };   // ORIG C1 0x844ef36 (sz 0x4: base ctor+vptr only)
class DB_UpdateCharacterDay : public DBDispatcher { public: DB_UpdateCharacterDay() {} };   // ORIG C1 0x844ef52 (sz 0x4: base ctor+vptr only)
class DB_UpdateNPCRelationship : public DBDispatcher { public: DB_UpdateNPCRelationship() {} };   // ORIG C1 0x844ef6e (sz 0x4: base ctor+vptr only)
class DB_SecurityCardAddCertCnt : public DBDispatcher { public: DB_SecurityCardAddCertCnt() {} };   // ORIG C1 0x844ef8a (sz 0x4: base ctor+vptr only)
class DB_LoadExpertJob : public DBDispatcher { public: DB_LoadExpertJob() {} };   // ORIG C1 0x844efa6 (sz 0x4: base ctor+vptr only)
class DB_SaveExpertJob : public DBDispatcher { public: DB_SaveExpertJob() {} };   // ORIG C1 0x844efc2 (sz 0x4: base ctor+vptr only)
class DB_UpdateExpertJobStatistic : public DBDispatcher { public: DB_UpdateExpertJobStatistic() {} };   // ORIG C1 0x844efde (sz 0x4: base ctor+vptr only)
class DB_InsertServerLoadStatistic : public DBDispatcher { public: DB_InsertServerLoadStatistic() {} };   // ORIG C1 0x844effa (sz 0x4: base ctor+vptr only)
class DB_FindFactoryHub : public DBDispatcher { public: DB_FindFactoryHub() {} };   // ORIG C1 0x844f016 (sz 0x4: base ctor+vptr only)
class DB_HackIPSave : public DBDispatcher { public: DB_HackIPSave() {} };   // ORIG C1 0x844f032 (sz 0x4: base ctor+vptr only)
class DB_CancelRestingUserRestrict : public DBDispatcher { public: DB_CancelRestingUserRestrict() {} };   // ORIG C1 0x844f04e (sz 0x4: base ctor+vptr only)
class DB_SaveBloodDungeon : public DBDispatcher { public: DB_SaveBloodDungeon() {} };   // ORIG C1 0x844f06a (sz 0x4: base ctor+vptr only)
class DB_LoadPowerWarInfo : public DBDispatcher { public: DB_LoadPowerWarInfo() {} };   // ORIG C1 0x844f086 (sz 0x4: base ctor+vptr only)
class DB_SecuServiceReward : public DBDispatcher { public: DB_SecuServiceReward() {} };   // ORIG C1 0x844f0a2 (sz 0x4: base ctor+vptr only)
class DB_Find_Factory_Hub_User : public DBDispatcher { public: DB_Find_Factory_Hub_User() {} };   // ORIG C1 0x844f0be (sz 0x4: base ctor+vptr only)
class DB_SaveConditionEventInfo : public DBDispatcher { public: DB_SaveConditionEventInfo() {} };   // ORIG C1 0x844f0da (sz 0x4: base ctor+vptr only)
class DB_DeletePowerWarStatueMessage : public DBDispatcher { public: DB_DeletePowerWarStatueMessage() {} };   // ORIG C1 0x844f0f6 (sz 0x4: base ctor+vptr only)
class DB_LoadNoCacheData : public DBDispatcher { public: DB_LoadNoCacheData() {} };   // ORIG C1 0x844f112 (sz 0x4: base ctor+vptr only)
class DB_UpdateItemLock : public DBDispatcher { public: DB_UpdateItemLock() {} };   // ORIG C1 0x844f12e (sz 0x4: base ctor+vptr only)
class DB_UpdateCreatureItemLock : public DBDispatcher { public: DB_UpdateCreatureItemLock() {} };   // ORIG C1 0x844f14a (sz 0x4: base ctor+vptr only)
class DB_UpdateAvatarItemLock : public DBDispatcher { public: DB_UpdateAvatarItemLock() {} };   // ORIG C1 0x844f166 (sz 0x4: base ctor+vptr only)
class DB_UpdateMemberPlayInfoPcRoom : public DBDispatcher { public: DB_UpdateMemberPlayInfoPcRoom() {} };   // ORIG C1 0x844f182 (sz 0x4: base ctor+vptr only)
class DB_UpdateCompoundEmblemStatics : public DBDispatcher { public: DB_UpdateCompoundEmblemStatics() {} };   // ORIG C1 0x844f19e (sz 0x4: base ctor+vptr only)
class DB_AccountHack_CheckMyAccount : public DBDispatcher { public: DB_AccountHack_CheckMyAccount() {} };   // ORIG C1 0x844f1ba (sz 0x4: base ctor+vptr only)
class DB_SecuServiceUpdateValidityTime : public DBDispatcher { public: DB_SecuServiceUpdateValidityTime() {} };   // ORIG C1 0x844f1d6 (sz 0x4: base ctor+vptr only)
class DB_AutoPunishedSecondUserData : public DBDispatcher { public: DB_AutoPunishedSecondUserData() {} };   // ORIG C1 0x844f1f2 (sz 0x4: base ctor+vptr only)
class DB_SaveProperDungeonClearCount : public DBDispatcher { public: DB_SaveProperDungeonClearCount() {} };   // ORIG C1 0x844f20e (sz 0x4: base ctor+vptr only)
class DB_LoadPvPBuddy : public DBDispatcher { public: DB_LoadPvPBuddy() {} };   // ORIG C1 0x844f22a (sz 0x4: base ctor+vptr only)
class DB_InsertPvPBuddy : public DBDispatcher { public: DB_InsertPvPBuddy() {} };   // ORIG C1 0x844f246 (sz 0x4: base ctor+vptr only)
class DB_DeletePvPBuddy : public DBDispatcher { public: DB_DeletePvPBuddy() {} };   // ORIG C1 0x844f262 (sz 0x4: base ctor+vptr only)
class DB_UpdatePvPBuddy : public DBDispatcher { public: DB_UpdatePvPBuddy() {} };   // ORIG C1 0x844f27e (sz 0x4: base ctor+vptr only)
class DB_DeleteGuild : public DBDispatcher { public: DB_DeleteGuild() {} };   // ORIG C1 0x844f29a (sz 0x4: base ctor+vptr only)
class DB_LoadPvPLiveEventData : public DBDispatcher { public: DB_LoadPvPLiveEventData() {} };   // ORIG C1 0x844f2b6 (sz 0x4: base ctor+vptr only)
class DB_UpdatePvPLiveEventData : public DBDispatcher { public: DB_UpdatePvPLiveEventData() {} };   // ORIG C1 0x844f2d2 (sz 0x4: base ctor+vptr only)
class DB_LoadOnlinePreliminaryMember : public DBDispatcher { public: DB_LoadOnlinePreliminaryMember() {} };   // ORIG C1 0x844f2ee (sz 0x4: base ctor+vptr only)
class DB_UpdateOnlinePreliminaryMember : public DBDispatcher { public: DB_UpdateOnlinePreliminaryMember() {} };   // ORIG C1 0x844f30a (sz 0x4: base ctor+vptr only)
class DB_LoadOnlinePreliminaryTeam : public DBDispatcher { public: DB_LoadOnlinePreliminaryTeam() {} };   // ORIG C1 0x844f326 (sz 0x4: base ctor+vptr only)
class DB_UpdateOnlinePreliminaryTeam : public DBDispatcher { public: DB_UpdateOnlinePreliminaryTeam() {} };   // ORIG C1 0x844f342 (sz 0x4: base ctor+vptr only)
class DB_UpdateOnlinePreliminaryMemberMatchList : public DBDispatcher { public: DB_UpdateOnlinePreliminaryMemberMatchList() {} };   // ORIG C1 0x844f35e (sz 0x4: base ctor+vptr only)
class DB_LoadOnlinePreliminaryTeamMatchList : public DBDispatcher { public: DB_LoadOnlinePreliminaryTeamMatchList() {} };   // ORIG C1 0x844f37a (sz 0x4: base ctor+vptr only)
class DB_UpdateOnlinePreliminaryTeamMatchList : public DBDispatcher { public: DB_UpdateOnlinePreliminaryTeamMatchList() {} };   // ORIG C1 0x844f396 (sz 0x4: base ctor+vptr only)
class DB_SaveRedeemItemList : public DBDispatcher { public: DB_SaveRedeemItemList() {} };   // ORIG C1 0x844f3b2 (sz 0x4: base ctor+vptr only)
class DB_CreateAccountCargo : public DBDispatcher { public: DB_CreateAccountCargo() {} };   // ORIG C1 0x844f3ce (sz 0x4: base ctor+vptr only)
class DB_UpgradeAccountCargo : public DBDispatcher { public: DB_UpgradeAccountCargo() {} };   // ORIG C1 0x844f3ea (sz 0x4: base ctor+vptr only)
class DB_DeleteAccountCargo : public DBDispatcher { public: DB_DeleteAccountCargo() {} };   // ORIG C1 0x844f406 (sz 0x4: base ctor+vptr only)
class DB_LoadAccountCargo : public DBDispatcher { public: DB_LoadAccountCargo() {} };   // ORIG C1 0x844f422 (sz 0x4: base ctor+vptr only)
class DB_SaveAccountCargo : public DBDispatcher { public: DB_SaveAccountCargo() {} };   // ORIG C1 0x844f43e (sz 0x4: base ctor+vptr only)
class DB_SaveMileage : public DBDispatcher { public: DB_SaveMileage() {} };   // ORIG C1 0x844f45a (sz 0x4: base ctor+vptr only)
class DB_LoadMileage : public DBDispatcher { public: DB_LoadMileage() {} };   // ORIG C1 0x844f476 (sz 0x4: base ctor+vptr only)
class DB_Multi_MailBox_Req_Send : public DBDispatcher { public: DB_Multi_MailBox_Req_Send() {} };   // ORIG C1 0x844f492 (sz 0x4: base ctor+vptr only)
class DB_ReqQueryCharacInfoMailbox : public DBDispatcher { public: DB_ReqQueryCharacInfoMailbox() {} };   // ORIG C1 0x844f4ae (sz 0x4: base ctor+vptr only)
class DB_GuildExpBookDelete : public DBDispatcher { public: DB_GuildExpBookDelete() {} };   // ORIG C1 0x844f4ca (sz 0x4: base ctor+vptr only)
class DB_InsertPoliceSaveChatting : public DBDispatcher { public: DB_InsertPoliceSaveChatting() {} };   // ORIG C1 0x844f4e6 (sz 0x4: base ctor+vptr only)
class DB_UpdateChuseokPackagePurchase : public DBDispatcher { public: DB_UpdateChuseokPackagePurchase() {} };   // ORIG C1 0x844f502 (sz 0x4: base ctor+vptr only)
class DB_CheckJoinGuild : public DBDispatcher { public: DB_CheckJoinGuild() {} };   // ORIG C1 0x844f51e (sz 0x4: base ctor+vptr only)
class DB_RequestJoinGuild : public DBDispatcher { public: DB_RequestJoinGuild() {} };   // ORIG C1 0x844f53a (sz 0x4: base ctor+vptr only)
class DB_JoinGuildInfo : public DBDispatcher { public: DB_JoinGuildInfo() {} };   // ORIG C1 0x844f556 (sz 0x4: base ctor+vptr only)
class DB_CancelJoinGuild : public DBDispatcher { public: DB_CancelJoinGuild() {} };   // ORIG C1 0x844f572 (sz 0x4: base ctor+vptr only)
class DB_GuildJoinList : public DBDispatcher { public: DB_GuildJoinList() {} };   // ORIG C1 0x844f58e (sz 0x4: base ctor+vptr only)
class DB_DenyJoinGuild : public DBDispatcher { public: DB_DenyJoinGuild() {} };   // ORIG C1 0x844f5aa (sz 0x4: base ctor+vptr only)
class DB_UpdateReturnUserFirstLogin : public DBDispatcher { public: DB_UpdateReturnUserFirstLogin() {} };   // ORIG C1 0x844f5c6 (sz 0x4: base ctor+vptr only)
class DB_InsertAccountFirstLoginFlag : public DBDispatcher { public: DB_InsertAccountFirstLoginFlag() {} };   // ORIG C1 0x844f5e2 (sz 0x4: base ctor+vptr only)
class DB_SaveGameOption_QuickChatting : public DBDispatcher { public: DB_SaveGameOption_QuickChatting() {} };   // ORIG C1 0x844f5fe (sz 0x4: base ctor+vptr only)
class DB_UpdatePurchaseCashItemBonus : public DBDispatcher { public: DB_UpdatePurchaseCashItemBonus() {} };   // ORIG C1 0x844f61a (sz 0x4: base ctor+vptr only)
class DB_UpdateItemCountValue : public DBDispatcher { public: DB_UpdateItemCountValue() {} };   // ORIG C1 0x844f636 (sz 0x4: base ctor+vptr only)
class DB_UpdateLimitItemUsage : public DBDispatcher { public: DB_UpdateLimitItemUsage() {} };   // ORIG C1 0x844f652 (sz 0x4: base ctor+vptr only)
class DB_LogBuyItem : public DBDispatcher { public: DB_LogBuyItem() {} };   // ORIG C1 0x844f66e (sz 0x4: base ctor+vptr only)
class DB_UpdateRevengeData : public DBDispatcher { public: DB_UpdateRevengeData() {} };   // ORIG C1 0x844f68a (sz 0x4: base ctor+vptr only)
class DB_UpdateGoblinPadAdvicePwdRewardTime : public DBDispatcher { public: DB_UpdateGoblinPadAdvicePwdRewardTime() {} };   // ORIG C1 0x844f6a6 (sz 0x4: base ctor+vptr only)
class DB_UpdateCharacPlayTime : public DBDispatcher { public: DB_UpdateCharacPlayTime() {} };   // ORIG C1 0x844f6c2 (sz 0x4: base ctor+vptr only)
class DB_SaveExpandEquipslot : public DBDispatcher { public: DB_SaveExpandEquipslot() {} };   // ORIG C1 0x844f6de (sz 0x4: base ctor+vptr only)
class DB_UpdateFairPvPScore : public DBDispatcher { public: DB_UpdateFairPvPScore() {} };   // ORIG C1 0x844f6fa (sz 0x4: base ctor+vptr only)
class DB_SavePvPMissionList : public DBDispatcher { public: DB_SavePvPMissionList() {} };   // ORIG C1 0x844f716 (sz 0x4: base ctor+vptr only)
class DB_InsertResponseTime : public DBDispatcher { public: DB_InsertResponseTime() {} };   // ORIG C1 0x844f732 (sz 0x4: base ctor+vptr only)
class DB_UpdateItemMakingSkill : public DBDispatcher { public: DB_UpdateItemMakingSkill() {} };   // ORIG C1 0x844f74e (sz 0x4: base ctor+vptr only)
class DB_TOD_Random_Select_UserAPC : public DBDispatcher { public: DB_TOD_Random_Select_UserAPC() {} };   // ORIG C1 0x844f76a (sz 0x4: base ctor+vptr only)
class DB_TOD_Reset_UserAPC : public DBDispatcher { public: DB_TOD_Reset_UserAPC() {} };   // ORIG C1 0x844f786 (sz 0x4: base ctor+vptr only)
class DB_TOD_Update_UserState : public DBDispatcher { public: DB_TOD_Update_UserState() {} };   // ORIG C1 0x844f7a2 (sz 0x4: base ctor+vptr only)
class DB_UpdateQuestShopBuyingStatusList : public DBDispatcher { public: DB_UpdateQuestShopBuyingStatusList() {} };   // ORIG C1 0x844f7be (sz 0x4: base ctor+vptr only)
class DB_UpdateAddItemCountValue : public DBDispatcher { public: DB_UpdateAddItemCountValue() {} };   // ORIG C1 0x844f7da (sz 0x4: base ctor+vptr only)
class DB_SaveKillMonsterInfo : public DBDispatcher { public: DB_SaveKillMonsterInfo() {} };   // ORIG C1 0x844f7f6 (sz 0x4: base ctor+vptr only)
class DB_LoadKillMonsterInfo : public DBDispatcher { public: DB_LoadKillMonsterInfo() {} };   // ORIG C1 0x844f812 (sz 0x4: base ctor+vptr only)
class DB_SaveTitleBook : public DBDispatcher { public: DB_SaveTitleBook() {} };   // ORIG C1 0x844f82e (sz 0x4: base ctor+vptr only)
class DB_SaveAchievement : public DBDispatcher { public: DB_SaveAchievement() {} };   // ORIG C1 0x844f84a (sz 0x4: base ctor+vptr only)
class DB_UpdateUserFatigueQuantity : public DBDispatcher { public: DB_UpdateUserFatigueQuantity() {} };   // ORIG C1 0x844f866 (sz 0x4: base ctor+vptr only)
class DB_UpdateUserGiftFatigueQuantity : public DBDispatcher { public: DB_UpdateUserGiftFatigueQuantity() {} };   // ORIG C1 0x844f882 (sz 0x4: base ctor+vptr only)
class DB_SaveCharacSlotExtend : public DBDispatcher { public: DB_SaveCharacSlotExtend() {} };   // ORIG C1 0x844f89e (sz 0x4: base ctor+vptr only)
class DB_SaveGiveAvengerTitleFlag : public DBDispatcher { public: DB_SaveGiveAvengerTitleFlag() {} };   // ORIG C1 0x844f8ba (sz 0x4: base ctor+vptr only)
class DB_SelectLevelUpReward : public DBDispatcher { public: DB_SelectLevelUpReward() {} };   // ORIG C1 0x844f8d6 (sz 0x4: base ctor+vptr only)
class DB_InsertRandomOptionItemInform : public DBDispatcher { public: DB_InsertRandomOptionItemInform() {} };   // ORIG C1 0x844f8f2 (sz 0x4: base ctor+vptr only)
class DB_StatisticCeraShopGiftLog : public DBDispatcher { public: DB_StatisticCeraShopGiftLog() {} };   // ORIG C1 0x844f90e (sz 0x4: base ctor+vptr only)
class DB_GetUserRegdate : public DBDispatcher { public: DB_GetUserRegdate() {} };   // ORIG C1 0x844f92a (sz 0x4: base ctor+vptr only)
class DB_UpdateOntimeLastRecvIdx : public DBDispatcher { public: DB_UpdateOntimeLastRecvIdx() {} };   // ORIG C1 0x844f946 (sz 0x4: base ctor+vptr only)
class DB_InsertOnTimeRewardUserLog : public DBDispatcher { public: DB_InsertOnTimeRewardUserLog() {} };   // ORIG C1 0x844f962 (sz 0x4: base ctor+vptr only)
class DB_CheckCharacName : public DBDispatcher { public: DB_CheckCharacName() {} };   // ORIG C1 0x844f97e (sz 0x4: base ctor+vptr only)
class DB_MailBox_Req_System_Multi_Mail : public DBDispatcher { public: DB_MailBox_Req_System_Multi_Mail() {} };   // ORIG C1 0x844f99a (sz 0x4: base ctor+vptr only)
class DB_CheckSecuReward : public DBDispatcher { public: DB_CheckSecuReward() {} };   // ORIG C1 0x844f9b6 (sz 0x4: base ctor+vptr only)
class DB_UpdateFatigueAttendance : public DBDispatcher { public: DB_UpdateFatigueAttendance() {} };   // ORIG C1 0x844f9d2 (sz 0x4: base ctor+vptr only)
class DB_UpdateInGameEventHistory : public DBDispatcher { public: DB_UpdateInGameEventHistory() {} };   // ORIG C1 0x844f9ee (sz 0x4: base ctor+vptr only)
class DB_UpdateInventoryCapacity : public DBDispatcher { public: DB_UpdateInventoryCapacity() {} };   // ORIG C1 0x844fa0a (sz 0x4: base ctor+vptr only)
class DB_UpdateAuraAvatarOption : public DBDispatcher { public: DB_UpdateAuraAvatarOption() {} };   // ORIG C1 0x844fa26 (sz 0x4: base ctor+vptr only)
class DB_InsertCeraItemHistory : public DBDispatcher { public: DB_InsertCeraItemHistory() {} };   // ORIG C1 0x844fa42 (sz 0x4: base ctor+vptr only)
class DB_UpdateAvatarHistory : public DBDispatcher { public: DB_UpdateAvatarHistory() {} };   // ORIG C1 0x844fa5e (sz 0x4: base ctor+vptr only)
class DB_SaveItemDictionaryFlag : public DBDispatcher { public: DB_SaveItemDictionaryFlag() {} };   // ORIG C1 0x844fa7a (sz 0x4: base ctor+vptr only)
class DB_SaveMercenaryData : public DBDispatcher { public: DB_SaveMercenaryData() {} };   // ORIG C1 0x844fa96 (sz 0x4: base ctor+vptr only)
class DB_HadesPunish : public DBDispatcher { public: DB_HadesPunish() {} };   // ORIG C1 0x844fab2 (sz 0x4: base ctor+vptr only)
class DB_Check3rdPartyConcent : public DBDispatcher { public: DB_Check3rdPartyConcent() {} };   // ORIG C1 0x844face (sz 0x4: base ctor+vptr only)
class DB_MercenaryLogInfo : public DBDispatcher { public: DB_MercenaryLogInfo() {} };   // ORIG C1 0x844faea (sz 0x4: base ctor+vptr only)
class DB_SelectCharacMid : public DBDispatcher { public: DB_SelectCharacMid() {} };   // ORIG C1 0x844fb06 (sz 0x4: base ctor+vptr only)
class DB_AvatarOptionChange : public DBDispatcher { public: DB_AvatarOptionChange() {} };   // ORIG C1 0x844fb22 (sz 0x4: base ctor+vptr only)
class DB_SaveObjectBringUp : public DBDispatcher { public: DB_SaveObjectBringUp() {} };   // ORIG C1 0x844fb3e (sz 0x4: base ctor+vptr only)
class DB_SaveGameOption_ChattingEmoticon : public DBDispatcher { public: DB_SaveGameOption_ChattingEmoticon() {} };   // ORIG C1 0x844fb5a (sz 0x4: base ctor+vptr only)
class DB_UpdateBoosterGageData : public DBDispatcher { public: DB_UpdateBoosterGageData() {} };   // ORIG C1 0x844fb76 (sz 0x4: base ctor+vptr only)
class CLocalChina_DB_BreakAwayReset : public DBDispatcher { public: CLocalChina_DB_BreakAwayReset() {} };   // ORIG C1 0x844fb92 (sz 0x4: base ctor+vptr only)
class CLocalChina_DB_LastDay_BreakAway : public DBDispatcher { public: CLocalChina_DB_LastDay_BreakAway() {} };   // ORIG C1 0x844fbae (sz 0x4: base ctor+vptr only)
class CLocalGlobal_DB_STATICS_BOSSTOWER : public DBDispatcher { public: CLocalGlobal_DB_STATICS_BOSSTOWER() {} };   // ORIG C1 0x844fbca (sz 0x4: base ctor+vptr only)
class DB_UpdateCeraShopPurchaseCount : public DBDispatcher { public: DB_UpdateCeraShopPurchaseCount() {} };   // ORIG C1 0x844fbe6 (sz 0x4: base ctor+vptr only)
class DB_LoadPuUserCheck : public DBDispatcher { public: DB_LoadPuUserCheck() {} };   // ORIG C1 0x844fc02 (sz 0x4: base ctor+vptr only)
class DB_SavePuUserCheck : public DBDispatcher { public: DB_SavePuUserCheck() {} };   // ORIG C1 0x844fc1e (sz 0x4: base ctor+vptr only)
class DB_CheckAddFriend : public DBDispatcher { public: DB_CheckAddFriend() {} };   // ORIG C1 0x844fc56 (sz 0x4: base ctor+vptr only)
class DB_SaveInformNotice : public DBDispatcher { public: DB_SaveInformNotice() {} };   // ORIG C1 0x844fc72 (sz 0x4: base ctor+vptr only)
class DB_NoticeIngameAdvertisement : public DBDispatcher { public: DB_NoticeIngameAdvertisement() {} };   // ORIG C1 0x844fc8e (sz 0x4: base ctor+vptr only)
class DB_RequestCharacSkillInfo : public DBDispatcher { public: DB_RequestCharacSkillInfo() {} };   // ORIG C1 0x844fcaa (sz 0x4: base ctor+vptr only)
class DB_RequestTagCharacInfo : public DBDispatcher { public: DB_RequestTagCharacInfo() {} };   // ORIG C1 0x844fcc6 (sz 0x4: base ctor+vptr only)
class DB_SavePcRoomPlayTimeEvent : public DBDispatcher
{
public:
    DB_SavePcRoomPlayTimeEvent() {}
    // ORIG T 0x08447bd0 _ZN26DB_SavePcRoomPlayTimeEvent11makeRequestEjjh
    static void makeRequest(unsigned int accId, unsigned int totalTime,
                            unsigned char rewardCnt);
};   // ORIG C1 0x844fce2 (sz 0x4: base ctor+vptr only)

// makeRequest —— ORIG 0x08447bd0 逐指令转写：
// StreamPool::Acquire("DBThread.cpp", 0xb270) → CStreamGuard(stream,true)
//   → guard << 0x2ff → guard << -1
//   → sig = guard.GetInBuffer<SIG_SAVE_PC_ROOM_PLAY_TIME>()
//   → sig +0/+4/+8 = accId/totalTime/rewardCnt
//   → MsgQueueMgr::put((QUEUE_IDX)2, guard)（析构时入队）。
void DB_SavePcRoomPlayTimeEvent::makeRequest(
    unsigned int accId, unsigned int totalTime, unsigned char rewardCnt)
{
    Stream* stream =
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xb270);
    CStreamGuard guard(stream, true);
    guard << (int)0x2ff;
    guard << (int)-1;
    SIG_SAVE_PC_ROOM_PLAY_TIME* sig =
        guard.GetInBuffer<SIG_SAVE_PC_ROOM_PLAY_TIME>();
    sig->m_accId = accId;
    sig->m_totalTime = totalTime;
    sig->m_rewardCnt = rewardCnt;
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
}
class DB_SaveBestClearTime : public DBDispatcher { public: DB_SaveBestClearTime() {} };   // ORIG C1 0x844fcfe (sz 0x4: base ctor+vptr only)
class DB_AuctionSubMoney : public DBDispatcher { public: DB_AuctionSubMoney() {} };   // ORIG C1 0x844fd1a (sz 0x4: base ctor+vptr only)
class DB_UpdateBlueMarbleEnterCount : public DBDispatcher { public: DB_UpdateBlueMarbleEnterCount() {} };   // ORIG C1 0x844fd36 (sz 0x4: base ctor+vptr only)
class DB_InsertAttendanceEvent : public DBDispatcher { public: DB_InsertAttendanceEvent() {} };   // ORIG C1 0x844fd52 (sz 0x4: base ctor+vptr only)
class DB_UpdateBingoEvent : public DBDispatcher { public: DB_UpdateBingoEvent() {} };   // ORIG C1 0x844fd6e (sz 0x4: base ctor+vptr only)
class DB_EventSaveGrowthWeapon : public DBDispatcher { public: DB_EventSaveGrowthWeapon() {} };   // ORIG C1 0x844fd8a (sz 0x4: base ctor+vptr only)
class DB_EventUpdateGrowthWeaponCharac : public DBDispatcher { public: DB_EventUpdateGrowthWeaponCharac() {} };   // ORIG C1 0x844fda6 (sz 0x4: base ctor+vptr only)
class DB_EventUpdateGrowthWeaponTimepiece : public DBDispatcher { public: DB_EventUpdateGrowthWeaponTimepiece() {} };   // ORIG C1 0x844fdc2 (sz 0x4: base ctor+vptr only)
class DB_EventGrowthWeaponReset : public DBDispatcher { public: DB_EventGrowthWeaponReset() {} };   // ORIG C1 0x844fdde (sz 0x4: base ctor+vptr only)
class DB_EventSaveGiveGrowCreature : public DBDispatcher { public: DB_EventSaveGiveGrowCreature() {} };   // ORIG C1 0x844fdfa (sz 0x4: base ctor+vptr only)
class DB_EventUpdateGiveGrowCreature : public DBDispatcher { public: DB_EventUpdateGiveGrowCreature() {} };   // ORIG C1 0x844fe16 (sz 0x4: base ctor+vptr only)
class DB_EventAccountCharacterLevelUpOnceGift : public DBDispatcher { public: DB_EventAccountCharacterLevelUpOnceGift() {} };   // ORIG C1 0x844fe32 (sz 0x4: base ctor+vptr only)
class DB_AvatarColorChange : public DBDispatcher { public: DB_AvatarColorChange() {} };   // ORIG C1 0x844fe4e (sz 0x4: base ctor+vptr only)
class DB_TestTradeRestrict : public DBDispatcher { public: DB_TestTradeRestrict() {} };   // ORIG C1 0x844fe6a (sz 0x4: base ctor+vptr only)
class DB_UpdateComboSkill : public DBDispatcher { public: DB_UpdateComboSkill() {} };   // ORIG C1 0x844fe86 (sz 0x4: base ctor+vptr only)
class DB_SaveCharacInformNotice : public DBDispatcher { public: DB_SaveCharacInformNotice() {} };   // ORIG C1 0x844fea2 (sz 0x4: base ctor+vptr only)
class DB_UpdateSeriaRoomAnimationDeco : public DBDispatcher { public: DB_UpdateSeriaRoomAnimationDeco() {} };   // ORIG C1 0x844febe (sz 0x4: base ctor+vptr only)
class DB_LoadCharacHotKeyOption : public DBDispatcher { public: DB_LoadCharacHotKeyOption() {} };   // ORIG C1 0x844feda (sz 0x4: base ctor+vptr only)
class DB_LogIngameAdvertisement : public DBDispatcher { public: DB_LogIngameAdvertisement() {} };   // ORIG C1 0x844fef6 (sz 0x4: base ctor+vptr only)
class DB_LogCeraShopError : public DBDispatcher { public: DB_LogCeraShopError() {} };   // ORIG C1 0x844ff12 (sz 0x4: base ctor+vptr only)
class DB_GetPremiumSelectedCubeInfo : public DBDispatcher { public: DB_GetPremiumSelectedCubeInfo() {} };   // ORIG C1 0x844ff2e (sz 0x4: base ctor+vptr only)
class DB_InsertEventStageLog : public DBDispatcher { public: DB_InsertEventStageLog() {} };   // ORIG C1 0x844ff4a (sz 0x4: base ctor+vptr only)
class DB_SaveCerashopAddRestrictType : public DBDispatcher { public: DB_SaveCerashopAddRestrictType() {} };   // ORIG C1 0x844ff66 (sz 0x4: base ctor+vptr only)
class DB_LoadCerashopAddRestrictType : public DBDispatcher { public: DB_LoadCerashopAddRestrictType() {} };   // ORIG C1 0x844ff82 (sz 0x4: base ctor+vptr only)
class DB_InsertAccountFirstLoginDailyFlag : public DBDispatcher { public: DB_InsertAccountFirstLoginDailyFlag() {} };   // ORIG C1 0x844ff9e (sz 0x4: base ctor+vptr only)
class DB_SaveEventLevelupSupport : public DBDispatcher { public: DB_SaveEventLevelupSupport() {} };   // ORIG C1 0x844ffba (sz 0x4: base ctor+vptr only)
class DB_LoadEventLevelupSupport : public DBDispatcher { public: DB_LoadEventLevelupSupport() {} };   // ORIG C1 0x844ffd6 (sz 0x4: base ctor+vptr only)
class DB_SelectAllowProxyUser : public DBDispatcher { public: DB_SelectAllowProxyUser() {} };   // ORIG C1 0x844fff2 (sz 0x4: base ctor+vptr only)


namespace advancealtar
{
class DB_Load : public DBDispatcher { public: DB_Load() {} };   // ORIG C1 0x844ddea (sz 0x4: base ctor+vptr only)
class DB_Update : public DBDispatcher { public: DB_Update() {} };   // ORIG C1 0x844de06 (sz 0x4: base ctor+vptr only)
class DB_StatisticAdvanceAltar : public DBDispatcher { public: DB_StatisticAdvanceAltar() {} };   // ORIG C1 0x844fc3a (sz 0x4: base ctor+vptr only)

}  // namespace advancealtar



namespace APSystem
{
class DB_LoadActionPoint : public DBDispatcher { public: DB_LoadActionPoint() {} };   // ORIG C1 0x844dd7a (sz 0x4: base ctor+vptr only)
class DB_UpdateActionPoint : public DBDispatcher { public: DB_UpdateActionPoint() {} };   // ORIG C1 0x844dd96 (sz 0x4: base ctor+vptr only)
class DB_LoadRewardMedal : public DBDispatcher { public: DB_LoadRewardMedal() {} };   // ORIG C1 0x844ddb2 (sz 0x4: base ctor+vptr only)
class DB_UpdateRewardMedal : public DBDispatcher { public: DB_UpdateRewardMedal() {} };   // ORIG C1 0x844ddce (sz 0x4: base ctor+vptr only)

}  // namespace APSystem


namespace OS_API
{

// ORIG 0x0858c4fa：select(0,NULL,NULL,NULL,{sec=0,usec})。
void uSleep(unsigned int usec)
{
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = usec;
    select(0, NULL, NULL, NULL, &tv);
}

}  // namespace OS_API

// ---------------------------------------------------------------------------
// DBTask 生命周期
// ---------------------------------------------------------------------------

// ORIG 0x083fbfc6（报告 docs/class_func_reports/DBTask/DBTask.md）
const int DBTask::DISPATCHER_COUNT;   // C++98 odr-use 保险（ORIG 循环上界常量）
DBTask::DBTask()
{
    RegistDispatcher();
}

// ORIG 0x083fbfda（报告 docs/class_func_reports/DBTask/~DBTask.md）
DBTask::~DBTask()
{
    UnregistDispatcher();
}

// ---------------------------------------------------------------------------
// RegistDispatcher —— ORIG 0x083fbfee（0x3168 字节逐指令转写：
// 先把 852 个槽清零，再按消息 id 顺序 new 各 dispatcher 并写入对应槽；
// 地址 /tmp/df_batches/w8lt/regist_dispatcher.asm 全文核对）
// ---------------------------------------------------------------------------
void DBTask::RegistDispatcher()
{
    for (int i = 0; i <= DISPATCHER_COUNT - 1; ++i)     // ORIG cmpl $0x353/setle
    {
        m_dispatchers[i] = 0;
    }

    m_dispatchers[3] = new DB_Login();                                             // ORIG C1 0x840f48e
    m_dispatchers[4] = new DB_CreateCharac();                                      // ORIG C1 0x8400cc2
    m_dispatchers[5] = new DB_DeleteCharac();                                      // ORIG C1 0x8402bce
    m_dispatchers[6] = new DB_UpdateCharacView();                                  // ORIG C1 0x844d85c
    // ORIG 0x844d8c6: slot[7] = DB_LoadCharac   [批界:归 LT03_DBLoadSig(Batch03)域 实现，不在此建模]
    // ORIG 0x840a604: slot[8] = DB_LoadInventory   [批界:归 LT03_DBLoadSig(Batch03)域 实现，不在此建模]
    // ORIG 0x844d930: slot[10] = DB_LoadQuest   [批界:归 LT03_DBLoadSig(Batch03)域 实现，不在此建模]
    m_dispatchers[17] = new APSystem::DB_LoadActionPoint();                         // ORIG C1 0x844dd7a
    m_dispatchers[19] = new APSystem::DB_UpdateActionPoint();                       // ORIG C1 0x844dd96
    m_dispatchers[20] = new APSystem::DB_LoadRewardMedal();                         // ORIG C1 0x844ddb2
    m_dispatchers[21] = new APSystem::DB_UpdateRewardMedal();                       // ORIG C1 0x844ddce
    m_dispatchers[32] = new DB_LoadEtc();                                           // ORIG C1 0x8403b84
    // ORIG 0x8415c2e: slot[34] = DB_SaveCharac   [批界:归 LT03_DBLoadSig(Batch03)域 实现，不在此建模]
    m_dispatchers[35] = new DB_SaveInven();                                         // ORIG C1 0x8416a42
    m_dispatchers[36] = new DB_SaveSkill();                                         // ORIG C1 0x8417770
    m_dispatchers[37] = new DB_SavePvp();                                           // ORIG C1 0x841720e
    m_dispatchers[41] = new DB_UpdateQuest();                                       // ORIG C1 0x8418b74
    m_dispatchers[42] = new DB_UpdateGrowType();                                    // ORIG C1 0x8418130
    m_dispatchers[43] = new DB_UpdateLogin();                                       // ORIG C1 0x844da04
    m_dispatchers[44] = new DB_UpdateLogout();                                      // ORIG C1 0x84183d8
    m_dispatchers[46] = new DB_UpdateChannel();                                     // ORIG C1 0x8417f4a
    m_dispatchers[47] = new DB_DeleteChannel();                                     // ORIG C1 0x8402892
    m_dispatchers[45] = new DB_InsertChannel();                                     // ORIG C1 0x840364c
    m_dispatchers[49] = new DB_AvatarItemInsert();                                  // ORIG C1 0x83ffd50
    m_dispatchers[51] = new DB_AvatarItemDel();                                     // ORIG C1 0x83ffc42
    m_dispatchers[50] = new DB_AvatarItemMove();                                    // ORIG C1 0x840031e
    m_dispatchers[52] = new DB_AvatarExtendPeriod();                                // ORIG C1 0x83ffab8
    m_dispatchers[53] = new DB_AvatarChangeOwner();                                 // ORIG C1 0x844d7f2
    m_dispatchers[54] = new DB_AvatarChangeStat();                                  // ORIG C1 0x844e0aa
    m_dispatchers[38] = new DB_SaveRank();                                          // ORIG C1 0x844d99a
    m_dispatchers[40] = new DB_InsertTicket();                                      // ORIG C1 0x8403a4e
    m_dispatchers[11] = new DB_LoadTicket();                                        // ORIG C1 0x8419572
    m_dispatchers[12] = new DB_LoadDungeonClear();                                  // ORIG C1 0x8419852
    m_dispatchers[39] = new DB_SaveDungeonClear();                                  // ORIG C1 0x8419fec
    m_dispatchers[63] = new DB_TerminateServer();                                   // ORIG C1 0x841a284
    m_dispatchers[55] = new DB_StatBreakJar();                                      // ORIG C1 0x8417a12
    m_dispatchers[56] = new DB_StatComposition();                                   // ORIG C1 0x8417b82
    m_dispatchers[57] = new DB_StatCoinUse();                                       // ORIG C1 0x8417b04
    m_dispatchers[58] = new DB_StatPvpPlay();                                       // ORIG C1 0x8417e58
    m_dispatchers[59] = new DB_StatGameRank();                                      // ORIG C1 0x8417d66
    m_dispatchers[60] = new DB_StatDungeonClear();                                  // ORIG C1 0x8417c74
    m_dispatchers[80] = new DB_CrashDownReport();                                   // ORIG C1 0x844e0c6
    m_dispatchers[82] = new DB_SaveFeaturedGoodsIdx();                              // ORIG C1 0x844e0e2
    m_dispatchers[83] = new DB_RemoveFeaturedGoodsIdx();                            // ORIG C1 0x844e0fe
    m_dispatchers[73] = new DB_GenCeraTicket();                                     // ORIG C1 0x844da6e
    m_dispatchers[76] = new DB_SaveEventItem();                                     // ORIG C1 0x844dad8
    m_dispatchers[77] = new DB_RemoveEventItem();                                   // ORIG C1 0x844db42
    m_dispatchers[78] = new DB_FatigueResetTimer();                                 // ORIG C1 0x844dbac
    m_dispatchers[81] = new DB_SuicideWhenQueueIsEmpty();                           // ORIG C1 0x844e11a
    m_dispatchers[84] = new DB_SavePremiumInfo();                                   // ORIG C1 0x844e136
    m_dispatchers[99] = new DB_SaveCharacInfo4Guild();                              // ORIG C1 0x844e152
    m_dispatchers[103] = new DB_StatDungeonFail();                                   // ORIG C1 0x844e16e
    m_dispatchers[104] = new DB_StatPVPPlay2();                                      // ORIG C1 0x844e18a
    m_dispatchers[105] = new DB_StatUseShusiaService();                              // ORIG C1 0x844e1a6
    m_dispatchers[106] = new DB_UpdateDetailChannel();                               // ORIG C1 0x844e1c2
    m_dispatchers[107] = new DB_SaveChannelUserLevel();                              // ORIG C1 0x844e1de
    m_dispatchers[119] = new DB_SavePrivateStore();                                  // ORIG C1 0x844e1fa
    m_dispatchers[120] = new DB_LoadPrivateStore();                                  // ORIG C1 0x844e216
    m_dispatchers[121] = new DB_SaveGamechannel();                                   // ORIG C1 0x844e232
    m_dispatchers[123] = new DB_UpdateQueryCountTimerLogdb();                        // ORIG C1 0x844e24e
    m_dispatchers[124] = new DB_UpdateQueryCountTimerDb();                           // ORIG C1 0x844e26a
    m_dispatchers[125] = new DB_UpdateQueryCountDbLogdb();                           // ORIG C1 0x844e286
    m_dispatchers[131] = new DB_UpdateNatType();                                     // ORIG C1 0x844e2a2
    m_dispatchers[132] = new DB_UpdateNatTypeUser();                                 // ORIG C1 0x844e2be
    m_dispatchers[126] = new DB_MailBox_Req_CheckReceiver();                         // ORIG C1 0x844e2da
    m_dispatchers[127] = new DB_MailBox_Req_List();                                  // ORIG C1 0x844e2f6
    m_dispatchers[129] = new DB_MailBox_Req_Delete();                                // ORIG C1 0x844e312
    m_dispatchers[128] = new DB_MailBox_Req_Send();                                  // ORIG C1 0x844e32e
    m_dispatchers[135] = new DB_InsertCreatureItem();                                // ORIG C1 0x844e34a
    m_dispatchers[137] = new DB_UpdateCreatureItemList();                            // ORIG C1 0x844e366
    m_dispatchers[138] = new DB_UpdateCreatureItem();                                // ORIG C1 0x844e382
    m_dispatchers[140] = new DB_SwapCreatureItem();                                  // ORIG C1 0x844e39e
    m_dispatchers[142] = new DB_GatheringParyStatus();                               // ORIG C1 0x844e3ba
    m_dispatchers[147] = new DB_ForbidUserToPlay();                                  // ORIG C1 0x844e3d6
    m_dispatchers[149] = new DB_LastMsgDb();                                         // ORIG C1 0x844e3f2
    m_dispatchers[150] = new DB_LastMsgLogDb();                                      // ORIG C1 0x844e40e
    m_dispatchers[154] = new DB_SaveClientHackLog();                                 // ORIG C1 0x844e42a
    m_dispatchers[155] = new DB_InsertQueueSizeLog();                                // ORIG C1 0x844e446
    m_dispatchers[157] = new DB_SaveSameMIDOtherCharLoginHack();                     // ORIG C1 0x844e462
    m_dispatchers[158] = new DB_SaveProxyLog();                                      // ORIG C1 0x844e47e
    m_dispatchers[161] = new DB_SaveGoldZeroTradeLog();                              // ORIG C1 0x844e49a
    m_dispatchers[163] = new DB_MouseRegister();                                     // ORIG C1 0x844e4b6
    m_dispatchers[164] = new DB_PassPadUpdateFailCnt();                              // ORIG C1 0x844e4d2
    m_dispatchers[165] = new DB_PassPadUpdateCancelCnt();                            // ORIG C1 0x844e4ee
    m_dispatchers[175] = new DB_UpdateDungeonDataStatisticLogdb();                   // ORIG C1 0x844e50a
    m_dispatchers[176] = new DB_BillingRecord();                                     // ORIG C1 0x844e526
    m_dispatchers[179] = new DB_ReqBlackCount();                                     // ORIG C1 0x844e542
    m_dispatchers[180] = new DB_ReqSaveCrazyLevelupEvent();                          // ORIG C1 0x844e55e
    m_dispatchers[181] = new DB_ReqSaveHackUserPunish();                             // ORIG C1 0x844e57a
    m_dispatchers[182] = new DB_InsertUserPlayTime();                                // ORIG C1 0x844e596
    m_dispatchers[187] = new DB_UpdateHappyBeanInfo();                               // ORIG C1 0x844e5b2
    m_dispatchers[190] = new DB_InsertQueryCounterDb();                              // ORIG C1 0x844e5ce
    m_dispatchers[191] = new DB_InsertQueryCounterLogdb();                           // ORIG C1 0x844e5ea
    m_dispatchers[192] = new DB_ReqItemUpgradeLog();                                 // ORIG C1 0x844e606
    m_dispatchers[194] = new DB_ChangeLetterStat();                                  // ORIG C1 0x844e622
    m_dispatchers[195] = new DB_ReqChangeCharacName();                               // ORIG C1 0x844e63e
    m_dispatchers[196] = new DB_ReqQueryCharacInfo();                                // ORIG C1 0x844e65a
    m_dispatchers[197] = new DB_MailBoxSendNewMailWithDBWork();                      // ORIG C1 0x844e676
    m_dispatchers[202] = new DB_ReportMannerlessUser_Upgrade();                      // ORIG C1 0x844e692
    m_dispatchers[203] = new DB_ReportMannerlessUser();                              // ORIG C1 0x844e6ae
    m_dispatchers[204] = new DB_UpdateClientCrashDownLog();                          // ORIG C1 0x844e6ca
    m_dispatchers[205] = new DB_TimerStatisticsLog();                                // ORIG C1 0x844e6e6
    m_dispatchers[206] = new DB_SaveUserHandicap();                                  // ORIG C1 0x844e702
    m_dispatchers[207] = new DB_ForceSaveSimpleSSO();                                // ORIG C1 0x844e71e
    m_dispatchers[273] = new DB_CountOfHumanCertify();                               // ORIG C1 0x844e73a
    m_dispatchers[209] = new DB_UpdateGiftCount();                                   // ORIG C1 0x844e756
    m_dispatchers[208] = new DB_UpdateMemberDoubtTrade();                            // ORIG C1 0x844e772
    m_dispatchers[210] = new DB_UpdateItemDropStatistics();                          // ORIG C1 0x844e78e
    m_dispatchers[212] = new DB_UpdatePvPGrade();                                    // ORIG C1 0x844e7aa
    m_dispatchers[213] = new DB_InsertValueStatistic();                              // ORIG C1 0x844e7c6
    m_dispatchers[224] = new DB_InsertDungeonEntrance();                             // ORIG C1 0x844e7e2
    m_dispatchers[225] = new DB_InsertDungeonEntrance_hour();                        // ORIG C1 0x844e7fe
    m_dispatchers[226] = new DB_GetDeathTowerBestRecord();                           // ORIG C1 0x844e81a
    m_dispatchers[227] = new DB_UpdateDeathTowerBestRecord();                        // ORIG C1 0x844e836
    m_dispatchers[228] = new DB_DeathTowerLoadTopRanker();                           // ORIG C1 0x844e852
    m_dispatchers[235] = new DB_CheckDoubleGuildName();                              // ORIG C1 0x844e86e
    m_dispatchers[236] = new DB_CheckDoubleGuildAddress();                           // ORIG C1 0x844e88a
    m_dispatchers[238] = new DB_ArrangeUserLoginInfo();                              // ORIG C1 0x844e8a6
    m_dispatchers[253] = new DB_SecurityCardIssue();                                 // ORIG C1 0x844e8c2
    m_dispatchers[254] = new DB_SecurityCardDisuse();                                // ORIG C1 0x844e8de
    m_dispatchers[255] = new DB_SecurityCardActivate();                              // ORIG C1 0x844e8fa
    m_dispatchers[256] = new DB_SecurityCardUpdateValidityTime();                    // ORIG C1 0x844e916
    m_dispatchers[257] = new DB_SecurityCardUpdateFailCnt();                         // ORIG C1 0x844e932
    m_dispatchers[258] = new DB_SecurityCardUpdateCancelCnt();                       // ORIG C1 0x844e94e
    m_dispatchers[312] = new DB_SecurityCardRetransfer();                            // ORIG C1 0x844e96a
    m_dispatchers[259] = new DB_InsertPartyType();                                   // ORIG C1 0x844e986
    m_dispatchers[260] = new DB_InsertPVPPlayerRate();                               // ORIG C1 0x844e9a2
    m_dispatchers[261] = new DB_InsertPVPMap();                                      // ORIG C1 0x844e9be
    m_dispatchers[262] = new DB_LoadDnfHackLog();                                    // ORIG C1 0x844e9da
    m_dispatchers[263] = new DB_InsertLevelDungeonPlay();                            // ORIG C1 0x844e9f6
    m_dispatchers[264] = new DB_ReloadAutoPunishRuleBackIP();                        // ORIG C1 0x844ea12
    m_dispatchers[265] = new DB_ReloadAutoPunishRuleHackType();                      // ORIG C1 0x844ea2e
    m_dispatchers[266] = new DB_InsertAutoPunishFirstUser();                         // ORIG C1 0x844ea4a
    m_dispatchers[267] = new DB_InsertAutoPunishSecondUser();                        // ORIG C1 0x844ea66
    m_dispatchers[338] = new DB_BlackIPMonitor();                                    // ORIG C1 0x844ea82
    m_dispatchers[340] = new DB_SaveCleanpadPoint();                                 // ORIG C1 0x844ea9e
    m_dispatchers[341] = new DB_BlackIPMonitorPartLoad();                            // ORIG C1 0x844eaba
    m_dispatchers[274] = new DB_Insert0712WinterEventCoupon();                       // ORIG C1 0x844ead6
    m_dispatchers[275] = new DB_Insert0712WinterEventCoupon();                       // ORIG C1 0x844ead6
    m_dispatchers[279] = new DB_InsertArchieveEventLog();                            // ORIG C1 0x844eaf2
    m_dispatchers[278] = new DB_InsertDungeonExp();                                  // ORIG C1 0x844eb0e
    m_dispatchers[283] = new DB_WriteGeoReject();                                    // ORIG C1 0x844eb2a
    m_dispatchers[290] = new DB_InsertUnlimitSupportLog();                           // ORIG C1 0x844eb46
    m_dispatchers[295] = new DB_LoadCleanPadPoint();                                 // ORIG C1 0x844eb62
    m_dispatchers[297] = new DB_UpdateSchoolPoint();                                 // ORIG C1 0x844eb7e
    m_dispatchers[336] = new DB_SaveAccountInfo();                                   // ORIG C1 0x844eb9a
    m_dispatchers[299] = new DB_GoblinPadUpdateReward();                             // ORIG C1 0x844ebb6
    m_dispatchers[300] = new DB_GoblinPadUpdateValidityTime();                       // ORIG C1 0x844ebd2
    m_dispatchers[302] = new DB_LoadGameOption();                                    // ORIG C1 0x844ebee
    m_dispatchers[304] = new DB_SaveGameOption_1();                                  // ORIG C1 0x844ec0a
    m_dispatchers[305] = new DB_SaveGameOption_2();                                  // ORIG C1 0x844ec26
    m_dispatchers[693] = new DB_SaveCharacterOption();                               // ORIG C1 0x844ec42
    m_dispatchers[306] = new DB_UpdateCoolTimeItem();                                // ORIG C1 0x844ec5e
    m_dispatchers[311] = new DB_UpdateEffectItem();                                  // ORIG C1 0x844ec7a
    m_dispatchers[313] = new DB_UpdateAvatarJewelSlot();                             // ORIG C1 0x844ec96
    m_dispatchers[309] = new DB_UpdatePvPResult();                                   // ORIG C1 0x844ecb2
    m_dispatchers[310] = new DB_UpdatePvPLastPlayTime();                             // ORIG C1 0x844ecce
    m_dispatchers[319] = new DB_LoadBloodBestRecord();                               // ORIG C1 0x844ecea
    m_dispatchers[320] = new DB_UpdateBloodBestRecord();                             // ORIG C1 0x844ed06
    m_dispatchers[321] = new DB_LoadBreakAwayPreventData();                          // ORIG C1 0x844ed22
    m_dispatchers[322] = new DB_LoadBreakAwayRewardData();                           // ORIG C1 0x844ed3e
    m_dispatchers[326] = new DB_UpdateBreakAwayAccruedCera();                        // ORIG C1 0x844ed5a
    m_dispatchers[327] = new DB_UpdateBreakAwayUvList();                             // ORIG C1 0x844ed76
    m_dispatchers[329] = new DB_UpdateBreakAwayRuckPoint();                          // ORIG C1 0x844ed92
    m_dispatchers[324] = new DB_UpdateBreakAwayClearAddInfo();                       // ORIG C1 0x844edae
    m_dispatchers[325] = new DB_UpdateBreakAwayDungeonClearCnt();                    // ORIG C1 0x844edca
    m_dispatchers[330] = new DB_UpdateGMData();                                      // ORIG C1 0x844ede6
    m_dispatchers[334] = new DB_LoadDimension();                                     // ORIG C1 0x844ee02
    m_dispatchers[335] = new DB_UpdateDimension();                                   // ORIG C1 0x844ee1e
    m_dispatchers[337] = new DB_InsertCirculationStatistic();                        // ORIG C1 0x844ee3a
    m_dispatchers[346] = new DB_LoadAutoMarketConditionsControl();                   // ORIG C1 0x844ee56
    m_dispatchers[347] = new DB_UpdateAutoMarketConditionsControl();                 // ORIG C1 0x844ee72
    m_dispatchers[348] = new DB_UpdateOneDayLetheSkill();                            // ORIG C1 0x844ee8e
    m_dispatchers[355] = new DB_UpdateVendingMachineStatistic();                     // ORIG C1 0x844eeaa
    m_dispatchers[357] = new DB_LoadPowerWarStatueRanker();                          // ORIG C1 0x844eec6
    m_dispatchers[358] = new DB_LoadPowerWarStatueInfo();                            // ORIG C1 0x844eee2
    m_dispatchers[356] = new DB_BlackipPrivateSecurityValuation();                   // ORIG C1 0x844eefe
    m_dispatchers[364] = new DB_UpdateServerMessage();                               // ORIG C1 0x844ef1a
    m_dispatchers[365] = new DB_LoadServerMessage();                                 // ORIG C1 0x844ef36
    m_dispatchers[366] = new DB_UpdateCharacterDay();                                // ORIG C1 0x844ef52
    m_dispatchers[367] = new DB_UpdateNPCRelationship();                             // ORIG C1 0x844ef6e
    m_dispatchers[380] = new DB_SecurityCardAddCertCnt();                            // ORIG C1 0x844ef8a
    m_dispatchers[384] = new DB_LoadExpertJob();                                     // ORIG C1 0x844efa6
    m_dispatchers[385] = new DB_SaveExpertJob();                                     // ORIG C1 0x844efc2
    m_dispatchers[386] = new DB_UpdateExpertJobStatistic();                          // ORIG C1 0x844efde
    m_dispatchers[387] = new DB_InsertServerLoadStatistic();                         // ORIG C1 0x844effa
    m_dispatchers[394] = new DB_FindFactoryHub();                                    // ORIG C1 0x844f016
    m_dispatchers[400] = new DB_HackIPSave();                                        // ORIG C1 0x844f032
    m_dispatchers[405] = new DB_CancelRestingUserRestrict();                         // ORIG C1 0x844f04e
    m_dispatchers[407] = new DB_SaveBloodDungeon();                                  // ORIG C1 0x844f06a
    m_dispatchers[411] = new DB_LoadPowerWarInfo();                                  // ORIG C1 0x844f086
    m_dispatchers[413] = new DB_SecuServiceReward();                                 // ORIG C1 0x844f0a2
    m_dispatchers[416] = new DB_Find_Factory_Hub_User();                             // ORIG C1 0x844f0be
    m_dispatchers[415] = new DB_SaveConditionEventInfo();                            // ORIG C1 0x844f0da
    m_dispatchers[421] = new DB_DeletePowerWarStatueMessage();                       // ORIG C1 0x844f0f6
    m_dispatchers[423] = new DB_LoadNoCacheData();                                   // ORIG C1 0x844f112
    m_dispatchers[440] = new DB_UpdateItemLock();                                    // ORIG C1 0x844f12e
    m_dispatchers[449] = new DB_UpdateCreatureItemLock();                            // ORIG C1 0x844f14a
    m_dispatchers[450] = new DB_UpdateAvatarItemLock();                              // ORIG C1 0x844f166
    m_dispatchers[451] = new DB_UpdateMemberPlayInfoPcRoom();                        // ORIG C1 0x844f182
    m_dispatchers[452] = new DB_UpdateCompoundEmblemStatics();                       // ORIG C1 0x844f19e
    m_dispatchers[395] = new DB_AccountHack_CheckMyAccount();                        // ORIG C1 0x844f1ba
    m_dispatchers[457] = new DB_SecuServiceUpdateValidityTime();                     // ORIG C1 0x844f1d6
    m_dispatchers[458] = new DB_AutoPunishedSecondUserData();                        // ORIG C1 0x844f1f2
    m_dispatchers[468] = new DB_SaveProperDungeonClearCount();                       // ORIG C1 0x844f20e
    m_dispatchers[469] = new DB_LoadPvPBuddy();                                      // ORIG C1 0x844f22a
    m_dispatchers[475] = new DB_InsertPvPBuddy();                                    // ORIG C1 0x844f246
    m_dispatchers[476] = new DB_DeletePvPBuddy();                                    // ORIG C1 0x844f262
    m_dispatchers[477] = new DB_UpdatePvPBuddy();                                    // ORIG C1 0x844f27e
    m_dispatchers[479] = new DB_DeleteGuild();                                       // ORIG C1 0x844f29a
    m_dispatchers[480] = new DB_LoadPvPLiveEventData();                              // ORIG C1 0x844f2b6
    m_dispatchers[481] = new DB_UpdatePvPLiveEventData();                            // ORIG C1 0x844f2d2
    m_dispatchers[486] = new DB_LoadOnlinePreliminaryMember();                       // ORIG C1 0x844f2ee
    m_dispatchers[487] = new DB_UpdateOnlinePreliminaryMember();                     // ORIG C1 0x844f30a
    m_dispatchers[488] = new DB_LoadOnlinePreliminaryTeam();                         // ORIG C1 0x844f326
    m_dispatchers[489] = new DB_UpdateOnlinePreliminaryTeam();                       // ORIG C1 0x844f342
    m_dispatchers[491] = new DB_UpdateOnlinePreliminaryMemberMatchList();            // ORIG C1 0x844f35e
    m_dispatchers[492] = new DB_LoadOnlinePreliminaryTeamMatchList();                // ORIG C1 0x844f37a
    m_dispatchers[493] = new DB_UpdateOnlinePreliminaryTeamMatchList();              // ORIG C1 0x844f396
    m_dispatchers[500] = new DB_SaveRedeemItemList();                                // ORIG C1 0x844f3b2
    m_dispatchers[494] = new DB_CreateAccountCargo();                                // ORIG C1 0x844f3ce
    m_dispatchers[495] = new DB_UpgradeAccountCargo();                               // ORIG C1 0x844f3ea
    m_dispatchers[496] = new DB_DeleteAccountCargo();                                // ORIG C1 0x844f406
    m_dispatchers[498] = new DB_LoadAccountCargo();                                  // ORIG C1 0x844f422
    m_dispatchers[497] = new DB_SaveAccountCargo();                                  // ORIG C1 0x844f43e
    m_dispatchers[508] = new DB_SaveMileage();                                       // ORIG C1 0x844f45a
    m_dispatchers[509] = new DB_LoadMileage();                                       // ORIG C1 0x844f476
    m_dispatchers[25] = new advancealtar::DB_Load();                                // ORIG C1 0x844ddea
    m_dispatchers[26] = new advancealtar::DB_Update();                              // ORIG C1 0x844de06
    m_dispatchers[501] = new DB_Multi_MailBox_Req_Send();                            // ORIG C1 0x844f492
    m_dispatchers[502] = new DB_ReqQueryCharacInfoMailbox();                         // ORIG C1 0x844f4ae
    m_dispatchers[521] = new DB_GuildExpBookDelete();                                // ORIG C1 0x844f4ca
    m_dispatchers[524] = new DB_InsertPoliceSaveChatting();                          // ORIG C1 0x844f4e6
    m_dispatchers[531] = new DB_UpdateChuseokPackagePurchase();                      // ORIG C1 0x844f502
    m_dispatchers[534] = new DB_CheckJoinGuild();                                    // ORIG C1 0x844f51e
    m_dispatchers[535] = new DB_RequestJoinGuild();                                  // ORIG C1 0x844f53a
    m_dispatchers[537] = new DB_JoinGuildInfo();                                     // ORIG C1 0x844f556
    m_dispatchers[536] = new DB_CancelJoinGuild();                                   // ORIG C1 0x844f572
    m_dispatchers[538] = new DB_GuildJoinList();                                     // ORIG C1 0x844f58e
    m_dispatchers[540] = new DB_DenyJoinGuild();                                     // ORIG C1 0x844f5aa
    m_dispatchers[543] = new DB_UpdateReturnUserFirstLogin();                        // ORIG C1 0x844f5c6
    m_dispatchers[606] = new DB_InsertAccountFirstLoginFlag();                       // ORIG C1 0x844f5e2
    m_dispatchers[569] = new DB_SaveGameOption_QuickChatting();                      // ORIG C1 0x844f5fe
    m_dispatchers[572] = new DB_UpdatePurchaseCashItemBonus();                       // ORIG C1 0x844f61a
    m_dispatchers[576] = new DB_UpdateItemCountValue();                              // ORIG C1 0x844f636
    m_dispatchers[577] = new DB_UpdateLimitItemUsage();                              // ORIG C1 0x844f652
    m_dispatchers[593] = new DB_LogBuyItem();                                        // ORIG C1 0x844f66e
    m_dispatchers[580] = new DB_UpdateRevengeData();                                 // ORIG C1 0x844f68a
    m_dispatchers[583] = new DB_UpdateGoblinPadAdvicePwdRewardTime();                // ORIG C1 0x844f6a6
    m_dispatchers[590] = new DB_UpdateCharacPlayTime();                              // ORIG C1 0x844f6c2
    m_dispatchers[621] = new DB_SaveExpandEquipslot();                               // ORIG C1 0x844f6de
    m_dispatchers[609] = new DB_UpdateFairPvPScore();                                // ORIG C1 0x844f6fa
    m_dispatchers[611] = new DB_SavePvPMissionList();                                // ORIG C1 0x844f716
    m_dispatchers[620] = new DB_InsertResponseTime();                                // ORIG C1 0x844f732
    m_dispatchers[622] = new DB_UpdateItemMakingSkill();                             // ORIG C1 0x844f74e
    m_dispatchers[629] = new DB_TOD_Random_Select_UserAPC();                         // ORIG C1 0x844f76a
    m_dispatchers[631] = new DB_TOD_Reset_UserAPC();                                 // ORIG C1 0x844f786
    m_dispatchers[632] = new DB_TOD_Update_UserState();                              // ORIG C1 0x844f7a2
    m_dispatchers[633] = new DB_UpdateQuestShopBuyingStatusList();                   // ORIG C1 0x844f7be
    m_dispatchers[640] = new DB_UpdateAddItemCountValue();                           // ORIG C1 0x844f7da
    m_dispatchers[641] = new DB_SaveKillMonsterInfo();                               // ORIG C1 0x844f7f6
    m_dispatchers[642] = new DB_LoadKillMonsterInfo();                               // ORIG C1 0x844f812
    m_dispatchers[645] = new DB_SaveTitleBook();                                     // ORIG C1 0x844f82e
    m_dispatchers[646] = new DB_SaveAchievement();                                   // ORIG C1 0x844f84a
    m_dispatchers[647] = new DB_UpdateUserFatigueQuantity();                         // ORIG C1 0x844f866
    m_dispatchers[648] = new DB_UpdateUserGiftFatigueQuantity();                     // ORIG C1 0x844f882
    m_dispatchers[650] = new DB_SaveCharacSlotExtend();                              // ORIG C1 0x844f89e
    m_dispatchers[653] = new DB_SaveGiveAvengerTitleFlag();                          // ORIG C1 0x844f8ba
    m_dispatchers[654] = new DB_SelectLevelUpReward();                               // ORIG C1 0x844f8d6
    m_dispatchers[543] = new DB_UpdateReturnUserFirstLogin();                        // ORIG C1 0x844f5c6
    m_dispatchers[665] = new DB_InsertRandomOptionItemInform();                      // ORIG C1 0x844f8f2
    m_dispatchers[668] = new DB_StatisticCeraShopGiftLog();                          // ORIG C1 0x844f90e
    m_dispatchers[673] = new DB_GetUserRegdate();                                    // ORIG C1 0x844f92a
    m_dispatchers[663] = new DB_UpdateOntimeLastRecvIdx();                           // ORIG C1 0x844f946
    m_dispatchers[664] = new DB_InsertOnTimeRewardUserLog();                         // ORIG C1 0x844f962
    m_dispatchers[674] = new DB_CheckCharacName();                                   // ORIG C1 0x844f97e
    m_dispatchers[676] = new DB_MailBox_Req_System_Multi_Mail();                     // ORIG C1 0x844f99a
    m_dispatchers[683] = new DB_CheckSecuReward();                                   // ORIG C1 0x844f9b6
    m_dispatchers[684] = new DB_UpdateFatigueAttendance();                           // ORIG C1 0x844f9d2
    m_dispatchers[685] = new DB_UpdateInGameEventHistory();                          // ORIG C1 0x844f9ee
    m_dispatchers[686] = new DB_UpdateInventoryCapacity();                           // ORIG C1 0x844fa0a
    m_dispatchers[687] = new DB_UpdateAuraAvatarOption();                            // ORIG C1 0x844fa26
    m_dispatchers[689] = new DB_InsertCeraItemHistory();                             // ORIG C1 0x844fa42
    m_dispatchers[688] = new DB_UpdateAvatarHistory();                               // ORIG C1 0x844fa5e
    m_dispatchers[690] = new DB_SaveItemDictionaryFlag();                            // ORIG C1 0x844fa7a
    m_dispatchers[691] = new DB_SaveMercenaryData();                                 // ORIG C1 0x844fa96
    m_dispatchers[694] = new DB_HadesPunish();                                       // ORIG C1 0x844fab2
    m_dispatchers[695] = new DB_Check3rdPartyConcent();                              // ORIG C1 0x844face
    m_dispatchers[696] = new DB_MercenaryLogInfo();                                  // ORIG C1 0x844faea
    m_dispatchers[697] = new DB_SelectCharacMid();                                   // ORIG C1 0x844fb06
    m_dispatchers[707] = new DB_AvatarOptionChange();                                // ORIG C1 0x844fb22
    m_dispatchers[714] = new DB_SaveObjectBringUp();                                 // ORIG C1 0x844fb3e
    m_dispatchers[727] = new DB_SaveGameOption_ChattingEmoticon();                   // ORIG C1 0x844fb5a
    m_dispatchers[728] = new DB_UpdateBoosterGageData();                             // ORIG C1 0x844fb76
    m_dispatchers[733] = new CLocalChina_DB_BreakAwayReset();                        // ORIG C1 0x844fb92
    m_dispatchers[734] = new CLocalChina_DB_LastDay_BreakAway();                     // ORIG C1 0x844fbae
    m_dispatchers[736] = new CLocalGlobal_DB_STATICS_BOSSTOWER();                    // ORIG C1 0x844fbca
    m_dispatchers[729] = new DB_UpdateCeraShopPurchaseCount();                       // ORIG C1 0x844fbe6
    // ORIG 0x844bc38: slot[732] = DB_AradDatabaseDispatcher   [BLOCKED: 需 ARAD::DISPATCHER::Arad_DatabaseDispatcher C1/Init(0x08184c7c/0x08184d40)真实producer，仓库尚无 .cpp]
    m_dispatchers[737] = new DB_LoadPuUserCheck();                                   // ORIG C1 0x844fc02
    m_dispatchers[738] = new DB_SavePuUserCheck();                                   // ORIG C1 0x844fc1e
    m_dispatchers[827] = new advancealtar::DB_StatisticAdvanceAltar();               // ORIG C1 0x844fc3a
    m_dispatchers[830] = new DB_CheckAddFriend();                                    // ORIG C1 0x844fc56
    m_dispatchers[757] = new DB_SaveInformNotice();                                  // ORIG C1 0x844fc72
    m_dispatchers[760] = new DB_NoticeIngameAdvertisement();                         // ORIG C1 0x844fc8e
    m_dispatchers[765] = new DB_RequestCharacSkillInfo();                            // ORIG C1 0x844fcaa
    m_dispatchers[766] = new DB_RequestTagCharacInfo();                              // ORIG C1 0x844fcc6
    m_dispatchers[767] = new DB_SavePcRoomPlayTimeEvent();                           // ORIG C1 0x844fce2
    m_dispatchers[768] = new DB_SaveBestClearTime();                                 // ORIG C1 0x844fcfe
    m_dispatchers[777] = new DB_AuctionSubMoney();                                   // ORIG C1 0x844fd1a
    m_dispatchers[780] = new DB_UpdateBlueMarbleEnterCount();                        // ORIG C1 0x844fd36
    m_dispatchers[782] = new DB_InsertAttendanceEvent();                             // ORIG C1 0x844fd52
    m_dispatchers[799] = new DB_UpdateBingoEvent();                                  // ORIG C1 0x844fd6e
    m_dispatchers[787] = new DB_EventSaveGrowthWeapon();                             // ORIG C1 0x844fd8a
    m_dispatchers[788] = new DB_EventUpdateGrowthWeaponCharac();                     // ORIG C1 0x844fda6
    m_dispatchers[789] = new DB_EventUpdateGrowthWeaponTimepiece();                  // ORIG C1 0x844fdc2
    m_dispatchers[790] = new DB_EventGrowthWeaponReset();                            // ORIG C1 0x844fdde
    m_dispatchers[791] = new DB_EventSaveGiveGrowCreature();                         // ORIG C1 0x844fdfa
    m_dispatchers[792] = new DB_EventUpdateGiveGrowCreature();                       // ORIG C1 0x844fe16
    m_dispatchers[793] = new DB_EventAccountCharacterLevelUpOnceGift();              // ORIG C1 0x844fe32
    m_dispatchers[794] = new DB_AvatarColorChange();                                 // ORIG C1 0x844fe4e
    m_dispatchers[796] = new DB_TestTradeRestrict();                                 // ORIG C1 0x844fe6a
    m_dispatchers[797] = new DB_UpdateComboSkill();                                  // ORIG C1 0x844fe86
    m_dispatchers[785] = new DB_SaveCharacInformNotice();                            // ORIG C1 0x844fea2
    m_dispatchers[798] = new DB_UpdateSeriaRoomAnimationDeco();                      // ORIG C1 0x844febe
    m_dispatchers[800] = new DB_LoadCharacHotKeyOption();                            // ORIG C1 0x844feda
    m_dispatchers[812] = new DB_LogIngameAdvertisement();                            // ORIG C1 0x844fef6
    m_dispatchers[814] = new DB_LogCeraShopError();                                  // ORIG C1 0x844ff12
    m_dispatchers[818] = new DB_GetPremiumSelectedCubeInfo();                        // ORIG C1 0x844ff2e
    m_dispatchers[819] = new DB_InsertEventStageLog();                               // ORIG C1 0x844ff4a
    // ORIG 0x844c9d4: slot[847] = DB_DispatcherTaiwan   [BLOCKED: 需 TaiwanDBDispatcher 全家(init 0x0815a1f8 ~0x5f8)producer]
    m_dispatchers[832] = new DB_SaveCerashopAddRestrictType();                       // ORIG C1 0x844ff66
    m_dispatchers[833] = new DB_LoadCerashopAddRestrictType();                       // ORIG C1 0x844ff82
    m_dispatchers[834] = new DB_InsertAccountFirstLoginDailyFlag();                  // ORIG C1 0x844ff9e
    m_dispatchers[847] = new DB_SaveEventLevelupSupport();                           // ORIG C1 0x844ffba
    m_dispatchers[848] = new DB_LoadEventLevelupSupport();                           // ORIG C1 0x844ffd6
    m_dispatchers[850] = new DB_SelectAllowProxyUser();                              // ORIG C1 0x844fff2
}

// ---------------------------------------------------------------------------
// UnregistDispatcher —— ORIG 0x083ff156 sz 0x72
// 遍历 852 槽：槽非空则调用其 vtable +0x08 槽（deleting destructor，ORIG
// `mov (%eax),%ecx` 其中 eax=vptr+0x8），随后清槽；等价于 delete p。
// ORIG 存在重复判空分支（0x83ff173 与 0x83ff17d 连续两次 test），忠实保留。
// 返回恒 1。
// ---------------------------------------------------------------------------
bool DBTask::UnregistDispatcher()
{
    for (int i = 0; i <= DISPATCHER_COUNT - 1; ++i)
    {
        DBDispatcher* p = m_dispatchers[i];
        if (p != 0)
        {
            if (p != 0)
            {
                delete p;                       // ORIG call *(vptr+0x08)
            }
            m_dispatchers[i] = 0;
        }
    }
    return true;
}

// ---------------------------------------------------------------------------
// HandleEvents —— ORIG 0x083ff1c8 sz 0x22b
// 取队列流：空队列 uSleep(100) 后返回 true；否则记录当前队列号并交给
// DBProcess。若处理结果是信号 0x3f（关服信号）打 Terminate trace 并返回
// false；否则按 stream 的池类型归还对应流池（size 超限时各打越界 trace）
// 并返回 true。
// 字符串证据：'bool DBTask::HandleEvents(...)' @0x8c5daa0、
// 'Terminate Server db queue: %s(%d)\n' @0x8c3672c、
// 'Small Stream Size over code(%d), size(%u)' @0x8c36750、
// 'Default Stream Size over code(%d), size(%u)' @0x8c3677c、
// 'Big Stream Size over code(%d), size(%u)' @0x8c367a8。
// ---------------------------------------------------------------------------
bool DBTask::HandleEvents(MsgQueueMgr::QUEUE_IDX queueIdx, int& signal)
{
    Stream* stream = GlobalData::s_msgq_mgr->get(queueIdx);
    if (stream == 0)
    {
        OS_API::uSleep(100u);
        return true;
    }

    m_curQueueIdx = static_cast<int>(queueIdx);
    const int length = stream->length();
    const int poolType = stream->m_poolType;      // Stream +0x20（Stream.h:73）

    DBProcess(stream, signal);

    if (signal == 0x3f)
    {
        cMyTrace trace("bool DBTask::HandleEvents(MsgQueueMgr::QUEUE_IDX, int&)",
                       0x8e6, 0);
        trace("Terminate Server db queue: %s(%d)\n",
              MsgQueueMgr::get_queue_name(queueIdx), static_cast<int>(queueIdx));
        return false;
    }

    if (poolType == 1)                            // SmallStreamPool
    {
        if (length > 0xfff)
        {
            cMyTrace trace("bool DBTask::HandleEvents(MsgQueueMgr::QUEUE_IDX, int&)",
                           0x8f2, 5);
            trace("Small Stream Size over code(%d), size(%u)", length,
                  static_cast<unsigned int>(length));
        }
        GlobalData::s_small_stream_pool->Free(stream);        // ORIG 0x08290004
    }
    else if (poolType == 2)                       // BigStreamPool
    {
        if (length > 0x3ffff)
        {
            cMyTrace trace("bool DBTask::HandleEvents(MsgQueueMgr::QUEUE_IDX, int&)",
                           0x908, 5);
            trace("Big Stream Size over code(%d), size(%u)", length,
                  static_cast<unsigned int>(length));
        }
        GlobalData::s_big_stream_pool->Free(stream);          // ORIG 0x08290304
    }
    else                                          // poolType == 0：StreamPool
    {
        if (length > 0x3fff)
        {
            cMyTrace trace("bool DBTask::HandleEvents(MsgQueueMgr::QUEUE_IDX, int&)",
                           0x8fd, 5);
            trace("Default Stream Size over code(%d), size(%u)", length,
                  static_cast<unsigned int>(length));
        }
        GlobalData::s_stream_pool->Free(stream);              // ORIG 0x0828fb28
    }
    return true;
}

// ---------------------------------------------------------------------------
// DBProcess —— ORIG 0x083ff3f4 sz 0x1fd
// 从流中顺序读 [signal][userIdx]，按 signal 索引 dispatcher 表执行虚
// dispatch；m_curQueueIdx==DB_Q(2) 时前后设置/复位 DBMgr 的用户分库索引。
// 非法 signal/userIdx 读取失败或表空均走 LogManager logFormat 错误路径。
// 字符串证据：函数名 'bool DBTask::DBProcess(Stream*, int&)' @0x8c5da60、
// '[%s][%d]' @0x8c36721、文件名 'DBThread.cpp' @0x8c36714、
// 'Invalid signal received' @0x8c367d0(line 0x967)、
// "Can't find the db dispatcher : signal(%d)" @0x8c367e8(line 0x970)。
// ---------------------------------------------------------------------------
bool DBTask::DBProcess(Stream* stream, int& signal)
{
    int msgId = 0;
    bool okMsgId = (*stream >> msgId);            // ORIG Stream::rsERi
    signal = msgId;
    if (!okMsgId)
    {
        cMyTrace trace("bool DBTask::DBProcess(Stream*, int&)", 0x94d, 5);
        trace("[%s][%d]", "bool DBTask::DBProcess(Stream*, int&)", 0x94d);
        return false;
    }

    int userIdx = 0;
    bool okUserIdx = (*stream >> userIdx);
    if (!okUserIdx)
    {
        cMyTrace trace("bool DBTask::DBProcess(Stream*, int&)", 0x95f, 5);
        trace("[%s][%d]", "bool DBTask::DBProcess(Stream*, int&)", 0x95f);
        return false;
    }

    if (msgId < 0 || msgId > 851)                 // ORIG cmp $0x353 / jle
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              "bool DBTask::DBProcess(Stream*, int&)", 0x967,
                              "Invalid signal received");
        return false;
    }

    DBDispatcher* dispatcher = m_dispatchers[msgId];
    if (dispatcher == 0)
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              "bool DBTask::DBProcess(Stream*, int&)", 0x970,
                              "Can't find the db dispatcher : signal(%d)", msgId);
        return false;
    }

    if (m_curQueueIdx == MsgQueueMgr::DB_Q)
    {
        GlobalData::s_db_mgr->setUserIdx(userIdx);
    }
    dispatcher->dispatch(msgId, userIdx, stream); // ORIG vtable slot0 (this,msgId,userIdx,stream)
    if (m_curQueueIdx == MsgQueueMgr::DB_Q)
    {
        GlobalData::s_db_mgr->setUserIdx(-1);
    }
    return true;
}

// stPvPTempTable_t 析构强定义（类外显式声明使 D1/D2 由本 TU 发出；
// ORIG 体为 5 字节空 ret @0x08458e92）
stPvPTempTable_t::~stPvPTempTable_t()
{}

static_assert(sizeof(DBTask) == 4 + 852 * 4,     // = 0xd54
              "DBTask layout drift (m_curQueueIdx@+0x00 + dispatchers[852]@+0x04)");
