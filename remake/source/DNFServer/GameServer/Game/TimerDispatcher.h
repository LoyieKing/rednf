#ifndef DNF_GAME_TIMER_DISPATCHER_H_
#define DNF_GAME_TIMER_DISPATCHER_H_

// df_game_r 还原 —— TimerDispatcher / TimerSig / 全部 Timer_* 定时器分发器
// 本头为这些类型的唯一声明点。全部类名/槽位/注册顺序取自 ORIG 构造函数
// （0x863120a）objdump 逐条转写；每对象 operator new(4)，对象尺寸=4（仅 vptr）。
// TimerDispatcher 自身无虚表（ORIG nm 无对应 _ZTV），为普通类。
// 具体类的 dispatch_sig 等函数体属后续还原批次：当前为带 ORIG 地址注释的 stub
// （与 Dispatchers.cpp 既有 stub 约定一致），符号名/签名与 ORIG 一致。

#include "TimerQueue.h"   // TimerEntry / TIMER_MESSAGE 权威定义复用

class CUser;

class TimerSig
{
public:
    TimerSig();
    virtual ~TimerSig();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3) = 0;
};

class TimerCheckConn : public TimerSig
{
public:
    TimerCheckConn();
    virtual ~TimerCheckConn();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerDBUpdate : public TimerSig
{
public:
    TimerDBUpdate();
    virtual ~TimerDBUpdate();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerStamina : public TimerSig
{
public:
    TimerStamina();
    virtual ~TimerStamina();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerMapLoading : public TimerSig
{
public:
    TimerMapLoading();
    virtual ~TimerMapLoading();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerSetResult : public TimerSig
{
public:
    TimerSetResult();
    virtual ~TimerSetResult();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerDungeonClear : public TimerSig
{
public:
    TimerDungeonClear();
    virtual ~TimerDungeonClear();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerDungeonFail : public TimerSig
{
public:
    TimerDungeonFail();
    virtual ~TimerDungeonFail();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerPartyWalkOutLackUser : public TimerSig
{
public:
    TimerPartyWalkOutLackUser();
    virtual ~TimerPartyWalkOutLackUser();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerPVPWalkOutLackUser : public TimerSig
{
public:
    TimerPVPWalkOutLackUser();
    virtual ~TimerPVPWalkOutLackUser();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerFatigueReset : public TimerSig
{
public:
    TimerFatigueReset();
    virtual ~TimerFatigueReset();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerAvatarExpire : public TimerSig
{
public:
    TimerAvatarExpire();
    virtual ~TimerAvatarExpire();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerStatisticsLog : public TimerSig
{
public:
    TimerStatisticsLog();
    virtual ~TimerStatisticsLog();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerCardSelect : public TimerSig
{
public:
    TimerCardSelect();
    virtual ~TimerCardSelect();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerClearDungeonReward : public TimerSig
{
public:
    TimerClearDungeonReward();
    virtual ~TimerClearDungeonReward();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerClearDungeonScoreScoll : public TimerSig
{
public:
    TimerClearDungeonScoreScoll();
    virtual ~TimerClearDungeonScoreScoll();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerClearDungeonCardSelectRight : public TimerSig
{
public:
    TimerClearDungeonCardSelectRight();
    virtual ~TimerClearDungeonCardSelectRight();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerPremiumServiceOver : public TimerSig
{
public:
    TimerPremiumServiceOver();
    virtual ~TimerPremiumServiceOver();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerPremiumServiceUpdate : public TimerSig
{
public:
    TimerPremiumServiceUpdate();
    virtual ~TimerPremiumServiceUpdate();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerPremiumServiceStart : public TimerSig
{
public:
    TimerPremiumServiceStart();
    virtual ~TimerPremiumServiceStart();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerCheckPaidPCRoomTerm : public TimerSig
{
public:
    TimerCheckPaidPCRoomTerm();
    virtual ~TimerCheckPaidPCRoomTerm();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerEPLPReturnVillage : public TimerSig
{
public:
    TimerEPLPReturnVillage();
    virtual ~TimerEPLPReturnVillage();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerEPLPCommandWait : public TimerSig
{
public:
    TimerEPLPCommandWait();
    virtual ~TimerEPLPCommandWait();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerWarRoomWaitCount : public TimerSig
{
public:
    TimerWarRoomWaitCount();
    virtual ~TimerWarRoomWaitCount();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerWarRoomReadyLockCount : public TimerSig
{
public:
    TimerWarRoomReadyLockCount();
    virtual ~TimerWarRoomReadyLockCount();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerWarRoomBattlePauseCount : public TimerSig
{
public:
    TimerWarRoomBattlePauseCount();
    virtual ~TimerWarRoomBattlePauseCount();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerWarRoomResultCount : public TimerSig
{
public:
    TimerWarRoomResultCount();
    virtual ~TimerWarRoomResultCount();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TImerWarRoomTimeBomb : public TimerSig
{
public:
    TImerWarRoomTimeBomb();
    virtual ~TImerWarRoomTimeBomb();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerWarRoomCheckFail : public TimerSig
{
public:
    TimerWarRoomCheckFail();
    virtual ~TimerWarRoomCheckFail();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerWarRoomChangeHost : public TimerSig
{
public:
    TimerWarRoomChangeHost();
    virtual ~TimerWarRoomChangeHost();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerBloodSmallRoundRegister : public TimerSig
{
public:
    TimerBloodSmallRoundRegister();
    virtual ~TimerBloodSmallRoundRegister();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerBloodPhaseRegister0 : public TimerSig
{
public:
    TimerBloodPhaseRegister0();
    virtual ~TimerBloodPhaseRegister0();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerBloodPhaseRegister1 : public TimerSig
{
public:
    TimerBloodPhaseRegister1();
    virtual ~TimerBloodPhaseRegister1();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerBloodPhaseRegister2 : public TimerSig
{
public:
    TimerBloodPhaseRegister2();
    virtual ~TimerBloodPhaseRegister2();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerBloodPhaseRegister3 : public TimerSig
{
public:
    TimerBloodPhaseRegister3();
    virtual ~TimerBloodPhaseRegister3();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerBloodPhaseRegister4 : public TimerSig
{
public:
    TimerBloodPhaseRegister4();
    virtual ~TimerBloodPhaseRegister4();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerBloodPhaseRegister5 : public TimerSig
{
public:
    TimerBloodPhaseRegister5();
    virtual ~TimerBloodPhaseRegister5();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerBloodPhaseRegister6 : public TimerSig
{
public:
    TimerBloodPhaseRegister6();
    virtual ~TimerBloodPhaseRegister6();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerBloodPhaseRegister7 : public TimerSig
{
public:
    TimerBloodPhaseRegister7();
    virtual ~TimerBloodPhaseRegister7();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerBloodPhaseRegister8 : public TimerSig
{
public:
    TimerBloodPhaseRegister8();
    virtual ~TimerBloodPhaseRegister8();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerBloodPhaseRegister9 : public TimerSig
{
public:
    TimerBloodPhaseRegister9();
    virtual ~TimerBloodPhaseRegister9();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerBloodCrazyMonster : public TimerSig
{
public:
    TimerBloodCrazyMonster();
    virtual ~TimerBloodCrazyMonster();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerBloodDungeonClearReward : public TimerSig
{
public:
    TimerBloodDungeonClearReward();
    virtual ~TimerBloodDungeonClearReward();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerUltimateNextRoundStart : public TimerSig
{
public:
    TimerUltimateNextRoundStart();
    virtual ~TimerUltimateNextRoundStart();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerEventChristmas : public TimerSig
{
public:
    TimerEventChristmas();
    virtual ~TimerEventChristmas();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerStatisticsDetailChannelInfo : public TimerSig
{
public:
    TimerStatisticsDetailChannelInfo();
    virtual ~TimerStatisticsDetailChannelInfo();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerStatisticLevel : public TimerSig
{
public:
    TimerStatisticLevel();
    virtual ~TimerStatisticLevel();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    static int CalcNextTime();               // ORIG T 0x863591a（定义在 GameMain.cpp）
    static int m_nTimeTable[5];               // ORIG D 0x93ace24 {1,10,14,17,21}

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerScheduleOneHour : public TimerSig
{
public:
    TimerScheduleOneHour();
    virtual ~TimerScheduleOneHour();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerSaveLogGamechannel : public TimerSig
{
public:
    TimerSaveLogGamechannel();
    virtual ~TimerSaveLogGamechannel();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerQueryCounter : public TimerSig
{
public:
    TimerQueryCounter();
    virtual ~TimerQueryCounter();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerNatType : public TimerSig
{
public:
    TimerNatType();
    virtual ~TimerNatType();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerNatTypeUser : public TimerSig
{
public:
    TimerNatTypeUser();
    virtual ~TimerNatTypeUser();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerCheckMailTimeOut : public TimerSig
{
public:
    TimerCheckMailTimeOut();
    virtual ~TimerCheckMailTimeOut();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerCreature : public TimerSig
{
public:
    TimerCreature();
    virtual ~TimerCreature();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerForbidChat : public TimerSig
{
public:
    TimerForbidChat();
    virtual ~TimerForbidChat();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerForbidMove : public TimerSig
{
public:
    TimerForbidMove();
    virtual ~TimerForbidMove();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerScheduleFiveMinPerUser : public TimerSig
{
public:
    TimerScheduleFiveMinPerUser();
    virtual ~TimerScheduleFiveMinPerUser();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerScheduleTenMinPerUser : public TimerSig
{
public:
    TimerScheduleTenMinPerUser();
    virtual ~TimerScheduleTenMinPerUser();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerQueueSizeLog : public TimerSig
{
public:
    TimerQueueSizeLog();
    virtual ~TimerQueueSizeLog();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerScheduleGuildWarEnd : public TimerSig
{
public:
    TimerScheduleGuildWarEnd();
    virtual ~TimerScheduleGuildWarEnd();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerSetPlayResult : public TimerSig
{
public:
    TimerSetPlayResult();
    virtual ~TimerSetPlayResult();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerStartRelayBattle : public TimerSig
{
public:
    TimerStartRelayBattle();
    virtual ~TimerStartRelayBattle();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerBossDieTimeout : public TimerSig
{
public:
    TimerBossDieTimeout();
    virtual ~TimerBossDieTimeout();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerEventCoinRefill : public TimerSig
{
public:
    TimerEventCoinRefill();
    virtual ~TimerEventCoinRefill();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerPvPDieUserRevive0 : public TimerSig
{
public:
    TimerPvPDieUserRevive0();
    virtual ~TimerPvPDieUserRevive0();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerPvPDieUserRevive1 : public TimerSig
{
public:
    TimerPvPDieUserRevive1();
    virtual ~TimerPvPDieUserRevive1();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerPvPDieUserRevive2 : public TimerSig
{
public:
    TimerPvPDieUserRevive2();
    virtual ~TimerPvPDieUserRevive2();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerPvPDieUserRevive3 : public TimerSig
{
public:
    TimerPvPDieUserRevive3();
    virtual ~TimerPvPDieUserRevive3();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerPvPDieUserRevive4 : public TimerSig
{
public:
    TimerPvPDieUserRevive4();
    virtual ~TimerPvPDieUserRevive4();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerPvPDieUserRevive5 : public TimerSig
{
public:
    TimerPvPDieUserRevive5();
    virtual ~TimerPvPDieUserRevive5();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerPvPDieUserRevive6 : public TimerSig
{
public:
    TimerPvPDieUserRevive6();
    virtual ~TimerPvPDieUserRevive6();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerPvPDieUserRevive7 : public TimerSig
{
public:
    TimerPvPDieUserRevive7();
    virtual ~TimerPvPDieUserRevive7();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerDungeonDataStatistic : public TimerSig
{
public:
    TimerDungeonDataStatistic();
    virtual ~TimerDungeonDataStatistic();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerNotResponseUserTimeout : public TimerSig
{
public:
    TimerNotResponseUserTimeout();
    virtual ~TimerNotResponseUserTimeout();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerLightServerNotice : public TimerSig
{
public:
    TimerLightServerNotice();
    virtual ~TimerLightServerNotice();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerInsertUserPlayTime : public TimerSig
{
public:
    TimerInsertUserPlayTime();
    virtual ~TimerInsertUserPlayTime();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerCheckForcedDisconnect : public TimerSig
{
public:
    TimerCheckForcedDisconnect();
    virtual ~TimerCheckForcedDisconnect();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerCheckPvPRoomUser : public TimerSig
{
public:
    TimerCheckPvPRoomUser();
    virtual ~TimerCheckPvPRoomUser();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerCheckRecvPvpEndResult : public TimerSig
{
public:
    TimerCheckRecvPvpEndResult();
    virtual ~TimerCheckRecvPvpEndResult();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerCheckHumanCertifyTimeout : public TimerSig
{
public:
    TimerCheckHumanCertifyTimeout();
    virtual ~TimerCheckHumanCertifyTimeout();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerUpdateItemGenerateStatistics : public TimerSig
{
public:
    TimerUpdateItemGenerateStatistics();
    virtual ~TimerUpdateItemGenerateStatistics();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    // ORIG T 0x08637318 _ZN33TimerUpdateItemGenerateStatistics15registNextTimerEl
    // （CItemGeneratorMgr::initialize 尾部按当前秒注册下一次统计定时器；定义待定时器域批次）
    static void registNextTimer(long sec);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerUpdatePvPGrade : public TimerSig
{
public:
    TimerUpdatePvPGrade();
    virtual ~TimerUpdatePvPGrade();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    static void registNextTimer(long sec);   // ORIG T 0x86373f2（参数为 long）

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerDetectDisconnect : public TimerSig
{
public:
    TimerDetectDisconnect();
    virtual ~TimerDetectDisconnect();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    static void registNextTimer();   // ORIG T 0x86374e0

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerCompleteLoadAssaultTimeout : public TimerSig
{
public:
    TimerCompleteLoadAssaultTimeout();
    virtual ~TimerCompleteLoadAssaultTimeout();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerConnectP2PAssaultTimeout : public TimerSig
{
public:
    TimerConnectP2PAssaultTimeout();
    virtual ~TimerConnectP2PAssaultTimeout();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerEndAssault : public TimerSig
{
public:
    TimerEndAssault();
    virtual ~TimerEndAssault();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerCloseAssault : public TimerSig
{
public:
    TimerCloseAssault();
    virtual ~TimerCloseAssault();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerCompleteLoadAfterAssaultTimeout : public TimerSig
{
public:
    TimerCompleteLoadAfterAssaultTimeout();
    virtual ~TimerCompleteLoadAfterAssaultTimeout();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerConnectP2PAfterAssaultTimeout : public TimerSig
{
public:
    TimerConnectP2PAfterAssaultTimeout();
    virtual ~TimerConnectP2PAfterAssaultTimeout();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerDeathTower : public TimerSig
{
public:
    TimerDeathTower();
    virtual ~TimerDeathTower();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerDeathTowerTopRankerReload : public TimerSig
{
public:
    TimerDeathTowerTopRankerReload();
    virtual ~TimerDeathTowerTopRankerReload();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    static void registNextTimer(long ms);    // ORIG T 0x8637854

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerScheduleTenSecond : public TimerSig
{
public:
    TimerScheduleTenSecond();
    virtual ~TimerScheduleTenSecond();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    static void registNextTimer(unsigned int sec);  // ORIG T 0x8637a62

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerFinishItemEffect : public TimerSig
{
public:
    TimerFinishItemEffect();
    virtual ~TimerFinishItemEffect();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerStartMatching : public TimerSig
{
public:
    TimerStartMatching();
    virtual ~TimerStartMatching();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerGlobalEffectItemBegin : public TimerSig
{
public:
    TimerGlobalEffectItemBegin();
    virtual ~TimerGlobalEffectItemBegin();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    static void registTimer(unsigned int type, unsigned int beginSec);   // ORIG T 0x8637e78

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerGlobalEffectItemTimeout : public TimerSig
{
public:
    TimerGlobalEffectItemTimeout();
    virtual ~TimerGlobalEffectItemTimeout();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    static void registTimer(unsigned int type, unsigned int timeoutSec);  // ORIG T 0x8637ef6

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerVendingMachineItemNotify : public TimerSig
{
public:
    TimerVendingMachineItemNotify();
    virtual ~TimerVendingMachineItemNotify();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    static void registTimer(int uid, unsigned long delayMs, unsigned int type, unsigned int param);  // ORIG T 0x8638074

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerConnectMonitorServer : public TimerSig
{
public:
    TimerConnectMonitorServer();
    virtual ~TimerConnectMonitorServer();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerAssaultSuperPowerState : public TimerSig
{
public:
    TimerAssaultSuperPowerState();
    virtual ~TimerAssaultSuperPowerState();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerNPCMoodChange : public TimerSig
{
public:
    TimerNPCMoodChange();
    virtual ~TimerNPCMoodChange();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    // ORIG T 0x086381ec _ZN18TimerNPCMoodChange15registNextTimerEl：
    // 纯 ret（空实现）；CNPCScriptList::initScript 末尾以参数 0 调用。
    static void registNextTimer(long sec);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerElevatorDungeon : public TimerSig
{
public:
    TimerElevatorDungeon();
    virtual ~TimerElevatorDungeon();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerRefreshPowerWarProcessInfo : public TimerSig
{
public:
    TimerRefreshPowerWarProcessInfo();
    virtual ~TimerRefreshPowerWarProcessInfo();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerGeneratePowerUpNotiTime : public TimerSig
{
public:
    TimerGeneratePowerUpNotiTime();
    virtual ~TimerGeneratePowerUpNotiTime();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerCheckPowerWarAbuser : public TimerSig
{
public:
    TimerCheckPowerWarAbuser();
    virtual ~TimerCheckPowerWarAbuser();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerSetPunishUser : public TimerSig
{
public:
    TimerSetPunishUser();
    virtual ~TimerSetPunishUser();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerSetTradePunishUser : public TimerSig
{
public:
    TimerSetTradePunishUser();
    virtual ~TimerSetTradePunishUser();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerRevivalCharac : public TimerSig
{
public:
    TimerRevivalCharac();
    virtual ~TimerRevivalCharac();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerRequestExit : public TimerSig
{
public:
    TimerRequestExit();
    virtual ~TimerRequestExit();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerVillageMonsterUpdateRate : public TimerSig
{
public:
    TimerVillageMonsterUpdateRate();
    virtual ~TimerVillageMonsterUpdateRate();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerVillageMonsterRegen : public TimerSig
{
public:
    TimerVillageMonsterRegen();
    virtual ~TimerVillageMonsterRegen();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerStartVillageAttacked : public TimerSig
{
public:
    TimerStartVillageAttacked();
    virtual ~TimerStartVillageAttacked();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerVillageAttackedPlayTimeout : public TimerSig
{
public:
    TimerVillageAttackedPlayTimeout();
    virtual ~TimerVillageAttackedPlayTimeout();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerItemUnlockWaiting : public TimerSig
{
public:
    TimerItemUnlockWaiting();
    virtual ~TimerItemUnlockWaiting();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerUpdateCompoundEmblemStatics : public TimerSig
{
public:
    TimerUpdateCompoundEmblemStatics();
    virtual ~TimerUpdateCompoundEmblemStatics();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerStartExpEventAfterVillageAttack : public TimerSig
{
public:
    TimerStartExpEventAfterVillageAttack();
    virtual ~TimerStartExpEventAfterVillageAttack();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_Security_NonClient_CheckResponse : public TimerSig
{
public:
    Timer_Security_NonClient_CheckResponse();
    virtual ~Timer_Security_NonClient_CheckResponse();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_Midnight : public TimerSig
{
public:
    Timer_Midnight();
    virtual ~Timer_Midnight();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerEndPowerWarVictoriousReward : public TimerSig
{
public:
    TimerEndPowerWarVictoriousReward();
    virtual ~TimerEndPowerWarVictoriousReward();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_TenMinuteSchedule : public TimerSig
{
public:
    Timer_TenMinuteSchedule();
    virtual ~Timer_TenMinuteSchedule();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_UpdateOnlinePreliminaryTeam : public TimerSig
{
public:
    Timer_UpdateOnlinePreliminaryTeam();
    virtual ~Timer_UpdateOnlinePreliminaryTeam();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_SecretShopStatistic : public TimerSig
{
public:
    Timer_SecretShopStatistic();
    virtual ~Timer_SecretShopStatistic();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    static void RegistNextTimer();           // ORIG T 0x86397ee

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_DailySchedule : public TimerSig
{
public:
    Timer_DailySchedule();
    virtual ~Timer_DailySchedule();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    static void RegistNextTimer();           // ORIG T 0x812c940

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_ConnectP2PPvPTimeout : public TimerSig
{
public:
    Timer_ConnectP2PPvPTimeout();
    virtual ~Timer_ConnectP2PPvPTimeout();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_RoutingItemWait : public TimerSig
{
public:
    Timer_RoutingItemWait();
    virtual ~Timer_RoutingItemWait();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_PcRoomAuthRetry : public TimerSig
{
public:
    Timer_PcRoomAuthRetry();
    virtual ~Timer_PcRoomAuthRetry();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_DungeonInoutOpenTime : public TimerSig
{
public:
    Timer_DungeonInoutOpenTime();
    virtual ~Timer_DungeonInoutOpenTime();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    static void RegistNextTimer();           // ORIG T 0x8639b20（定义在 GameMain.cpp）
    static bool checkDungeonInoutTime(int day);  // ORIG T 0x8639a12

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_DungeonInoutCloseTime : public TimerSig
{
public:
    Timer_DungeonInoutCloseTime();
    virtual ~Timer_DungeonInoutCloseTime();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_FairPvPRematch : public TimerSig
{
public:
    Timer_FairPvPRematch();
    virtual ~Timer_FairPvPRematch();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_ReturnUserTimeout : public TimerSig
{
public:
    Timer_ReturnUserTimeout();
    virtual ~Timer_ReturnUserTimeout();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_HadesHeartBeat : public TimerSig
{
public:
    Timer_HadesHeartBeat();
    virtual ~Timer_HadesHeartBeat();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_TOD_Send_ClearReward : public TimerSig
{
public:
    Timer_TOD_Send_ClearReward();
    virtual ~Timer_TOD_Send_ClearReward();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_OneADayItemShop : public TimerSig
{
public:
    Timer_OneADayItemShop();
    virtual ~Timer_OneADayItemShop();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    static void RegistNextTimer();           // ORIG T 0x863a93c

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_Schedule_MidNight : public TimerSig
{
public:
    Timer_Schedule_MidNight();
    virtual ~Timer_Schedule_MidNight();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_Send_Ontime_Reward : public TimerSig
{
public:
    Timer_Send_Ontime_Reward();
    virtual ~Timer_Send_Ontime_Reward();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_EventManagement : public TimerSig
{
public:
    Timer_EventManagement();
    virtual ~Timer_EventManagement();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    static void RegistNextTimer(int intervalMs);  // ORIG T 0x863abd4

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_QuickPartyWaitingPartyMatching : public TimerSig
{
public:
    Timer_QuickPartyWaitingPartyMatching();
    virtual ~Timer_QuickPartyWaitingPartyMatching();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_QuickPartyWaitingStartGame : public TimerSig
{
public:
    Timer_QuickPartyWaitingStartGame();
    virtual ~Timer_QuickPartyWaitingStartGame();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_AssaultGhostTimer : public TimerSig
{
public:
    Timer_AssaultGhostTimer();
    virtual ~Timer_AssaultGhostTimer();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_GenerateCaptchaData : public TimerSig
{
public:
    Timer_GenerateCaptchaData();
    virtual ~Timer_GenerateCaptchaData();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    static void RegistNextTimer(int sec);    // ORIG T 0x863b2ba

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_TournamentDugeonClearStateNotiReward : public TimerSig
{
public:
    Timer_TournamentDugeonClearStateNotiReward();
    virtual ~Timer_TournamentDugeonClearStateNotiReward();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_TournametDungeonClearStateSelect : public TimerSig
{
public:
    Timer_TournametDungeonClearStateSelect();
    virtual ~Timer_TournametDungeonClearStateSelect();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_TowerOnTime : public TimerSig
{
public:
    Timer_TowerOnTime();
    virtual ~Timer_TowerOnTime();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_NoticeEmergencyPatchMessage : public TimerSig
{
public:
    Timer_NoticeEmergencyPatchMessage();
    virtual ~Timer_NoticeEmergencyPatchMessage();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_DungeonStatisticEntrance_Hour : public TimerSig
{
public:
    Timer_DungeonStatisticEntrance_Hour();
    virtual ~Timer_DungeonStatisticEntrance_Hour();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    static void RegistNextTimer();        // ORIG T 0x863b9fa

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_RefreshServerState : public TimerSig
{
public:
    Timer_RefreshServerState();
    virtual ~Timer_RefreshServerState();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_StayTimeEvent : public TimerSig
{
public:
    Timer_StayTimeEvent();
    virtual ~Timer_StayTimeEvent();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_CreateDnfDbLoad : public TimerSig
{
public:
    Timer_CreateDnfDbLoad();
    virtual ~Timer_CreateDnfDbLoad();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

namespace advancealtar
{
class Timer_MainTick : public TimerSig
{
public:
    Timer_MainTick();
    virtual ~Timer_MainTick();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
static void RegistNextTimerGage();  // ORIG T 0x812f7b8（定义在 GameMain.cpp）
    static const unsigned int kMainTickMs;  // ORIG R 0x8b6c5e4

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_StageControl : public TimerSig
{
public:
    Timer_StageControl();
    virtual ~Timer_StageControl();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

}  // namespace advancealtar

class TimerDispatcher
{
public:
    TimerDispatcher();
    ~TimerDispatcher();
    TimerSig* get_dispatcher(int msg);                     // ORIG T 0x86329f6
    int dispatch(TimerEntry& entry);                       // ORIG T 0x8632a18

private:
    // 注册表：下标即分发码；ORIG 构造按槽位直接写入。
    TimerSig* m_dispatchers[175];
};

static_assert(sizeof(TimerDispatcher) == 0x2bc, "TimerDispatcher layout mismatch");

#endif  // DNF_GAME_TIMER_DISPATCHER_H_
