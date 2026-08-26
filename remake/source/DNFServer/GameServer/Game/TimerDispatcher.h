#ifndef DNF_GAME_TIMER_DISPATCHER_H_
#define DNF_GAME_TIMER_DISPATCHER_H_

// df_game_r 还原 —— TimerDispatcher / TimerSig / 全部 Timer_* 定时器分发器
// 本头为这些类型的唯一声明点。全部类名/槽位/注册顺序取自 ORIG 构造函数
// （0x863120a）objdump 逐条转写；每对象 operator new(4)，对象尺寸=4（仅 vptr）。
// TimerDispatcher 自身无虚表（ORIG nm 无对应 _ZTV），为普通类。
// 具体类的 dispatch_sig 等函数体属后续还原批次：当前为带 ORIG 地址注释的 stub
// （与 Dispatchers.cpp 既有 stub 约定一致），符号名/签名与 ORIG 一致。

#include "TimerQueue.h"   // TimerEntry / TIMER_MESSAGE 权威定义复用
#include <vector>

class CUser;

class TimerSig
{
public:
    virtual int dispatch_sig(int p1, int p2, unsigned int p3) = 0;
    virtual ~TimerSig();
};

class TimerCheckConn : public TimerSig
{
public:
    TimerCheckConn();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerCheckConn();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerDBUpdate : public TimerSig
{
public:
    TimerDBUpdate();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerDBUpdate();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerStamina : public TimerSig
{
public:
    TimerStamina();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerStamina();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerPvpRankRes : public TimerSig
{
public:
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
};

class TimerMapLoading : public TimerSig
{
public:
    TimerMapLoading();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerMapLoading();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerSetResult : public TimerSig
{
public:
    TimerSetResult();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerSetResult();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerDungeonClear : public TimerSig
{
public:
    TimerDungeonClear();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerDungeonClear();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerDungeonFail : public TimerSig
{
public:
    TimerDungeonFail();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerDungeonFail();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerPartyWalkOutLackUser : public TimerSig
{
public:
    TimerPartyWalkOutLackUser();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerPartyWalkOutLackUser();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerPVPWalkOutLackUser : public TimerSig
{
public:
    TimerPVPWalkOutLackUser();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerPVPWalkOutLackUser();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerFatigueReset : public TimerSig
{
public:
    TimerFatigueReset();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerFatigueReset();
    static bool RegistNextTimer();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerAvatarExpire : public TimerSig
{
public:
    TimerAvatarExpire();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerAvatarExpire();
    static void RegistTimer(CUser* user);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerStatisticsLog : public TimerSig
{
public:
    TimerStatisticsLog();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerStatisticsLog();
    static bool RegistNextTimer();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerCardSelect : public TimerSig
{
public:
    TimerCardSelect();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerCardSelect();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerClearDungeonReward : public TimerSig
{
public:
    TimerClearDungeonReward();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerClearDungeonReward();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerClearDungeonScoreScoll : public TimerSig
{
public:
    TimerClearDungeonScoreScoll();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerClearDungeonScoreScoll();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerClearDungeonCardSelectRight : public TimerSig
{
public:
    TimerClearDungeonCardSelectRight();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerClearDungeonCardSelectRight();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerPremiumServiceOver : public TimerSig
{
public:
    TimerPremiumServiceOver();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerPremiumServiceOver();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerPremiumServiceUpdate : public TimerSig
{
public:
    TimerPremiumServiceUpdate();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerPremiumServiceUpdate();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerPremiumServiceStart : public TimerSig
{
public:
    TimerPremiumServiceStart();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerPremiumServiceStart();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerCheckPaidPCRoomTerm : public TimerSig
{
public:
    TimerCheckPaidPCRoomTerm();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerCheckPaidPCRoomTerm();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerEPLPReturnVillage : public TimerSig
{
public:
    TimerEPLPReturnVillage();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerEPLPReturnVillage();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerEPLPCommandWait : public TimerSig
{
public:
    TimerEPLPCommandWait();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerEPLPCommandWait();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerWarRoomWaitCount : public TimerSig
{
public:
    TimerWarRoomWaitCount();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerWarRoomWaitCount();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerWarRoomReadyLockCount : public TimerSig
{
public:
    TimerWarRoomReadyLockCount();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerWarRoomReadyLockCount();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerWarRoomBattlePauseCount : public TimerSig
{
public:
    TimerWarRoomBattlePauseCount();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerWarRoomBattlePauseCount();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerWarRoomResultCount : public TimerSig
{
public:
    TimerWarRoomResultCount();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerWarRoomResultCount();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TImerWarRoomTimeBomb : public TimerSig
{
public:
    TImerWarRoomTimeBomb();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TImerWarRoomTimeBomb();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerWarRoomCheckFail : public TimerSig
{
public:
    TimerWarRoomCheckFail();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerWarRoomCheckFail();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerWarRoomChangeHost : public TimerSig
{
public:
    TimerWarRoomChangeHost();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerWarRoomChangeHost();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerBloodSmallRoundRegister : public TimerSig
{
public:
    TimerBloodSmallRoundRegister();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerBloodSmallRoundRegister();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerBloodPhaseRegister0 : public TimerSig
{
public:
    TimerBloodPhaseRegister0();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerBloodPhaseRegister0();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerBloodPhaseRegister1 : public TimerSig
{
public:
    TimerBloodPhaseRegister1();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerBloodPhaseRegister1();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerBloodPhaseRegister2 : public TimerSig
{
public:
    TimerBloodPhaseRegister2();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerBloodPhaseRegister2();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerBloodPhaseRegister3 : public TimerSig
{
public:
    TimerBloodPhaseRegister3();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerBloodPhaseRegister3();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerBloodPhaseRegister4 : public TimerSig
{
public:
    TimerBloodPhaseRegister4();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerBloodPhaseRegister4();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerBloodPhaseRegister5 : public TimerSig
{
public:
    TimerBloodPhaseRegister5();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerBloodPhaseRegister5();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerBloodPhaseRegister6 : public TimerSig
{
public:
    TimerBloodPhaseRegister6();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerBloodPhaseRegister6();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerBloodPhaseRegister7 : public TimerSig
{
public:
    TimerBloodPhaseRegister7();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerBloodPhaseRegister7();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerBloodPhaseRegister8 : public TimerSig
{
public:
    TimerBloodPhaseRegister8();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerBloodPhaseRegister8();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerBloodPhaseRegister9 : public TimerSig
{
public:
    TimerBloodPhaseRegister9();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerBloodPhaseRegister9();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerBloodCrazyMonster : public TimerSig
{
public:
    TimerBloodCrazyMonster();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerBloodCrazyMonster();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerBloodDungeonClearReward : public TimerSig
{
public:
    TimerBloodDungeonClearReward();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerBloodDungeonClearReward();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerUltimateNextRoundStart : public TimerSig
{
public:
    TimerUltimateNextRoundStart();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerUltimateNextRoundStart();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerEventChristmas : public TimerSig
{
public:
    TimerEventChristmas();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerEventChristmas();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerStatisticsDetailChannelInfo : public TimerSig
{
public:
    TimerStatisticsDetailChannelInfo();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerStatisticsDetailChannelInfo();
    static void RegistNextTimer();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerStatisticLevel : public TimerSig
{
public:
    TimerStatisticLevel();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerStatisticLevel();
    static int CalcNextTime();               // ORIG T 0x863591a（定义在 GameMain.cpp）
    static int m_nTimeTable[5];               // ORIG D 0x93ace24 {1,10,14,17,21}

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerScheduleOneHour : public TimerSig
{
public:
    TimerScheduleOneHour();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerScheduleOneHour();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerSaveLogGamechannel : public TimerSig
{
public:
    TimerSaveLogGamechannel();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerSaveLogGamechannel();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerQueryCounter : public TimerSig
{
public:
    TimerQueryCounter();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerQueryCounter();
    static void RegistNextTimer();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerNatType : public TimerSig
{
public:
    TimerNatType();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerNatType();
    static void RegistNextTimer();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerNatTypeUser : public TimerSig
{
public:
    TimerNatTypeUser();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerNatTypeUser();
    static void RegistNextTimer();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerCheckMailTimeOut : public TimerSig
{
public:
    TimerCheckMailTimeOut();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerCheckMailTimeOut();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerCreature : public TimerSig
{
public:
    TimerCreature();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerCreature();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerForbidChat : public TimerSig
{
public:
    TimerForbidChat();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerForbidChat();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerForbidMove : public TimerSig
{
public:
    TimerForbidMove();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerForbidMove();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerScheduleFiveMinPerUser : public TimerSig
{
public:
    TimerScheduleFiveMinPerUser();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerScheduleFiveMinPerUser();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerScheduleTenMinPerUser : public TimerSig
{
public:
    TimerScheduleTenMinPerUser();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerScheduleTenMinPerUser();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerQueueSizeLog : public TimerSig
{
public:
    TimerQueueSizeLog();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerQueueSizeLog();
    static bool RegistNextTimer();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerScheduleGuildWarEnd : public TimerSig
{
public:
    TimerScheduleGuildWarEnd();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerScheduleGuildWarEnd();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerSetPlayResult : public TimerSig
{
public:
    TimerSetPlayResult();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerSetPlayResult();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerStartRelayBattle : public TimerSig
{
public:
    TimerStartRelayBattle();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerStartRelayBattle();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerBossDieTimeout : public TimerSig
{
public:
    TimerBossDieTimeout();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerBossDieTimeout();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerEventCoinRefill : public TimerSig
{
public:
    TimerEventCoinRefill();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerEventCoinRefill();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerPvPDieUserRevive0 : public TimerSig
{
public:
    TimerPvPDieUserRevive0();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerPvPDieUserRevive0();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerPvPDieUserRevive1 : public TimerSig
{
public:
    TimerPvPDieUserRevive1();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerPvPDieUserRevive1();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerPvPDieUserRevive2 : public TimerSig
{
public:
    TimerPvPDieUserRevive2();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerPvPDieUserRevive2();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerPvPDieUserRevive3 : public TimerSig
{
public:
    TimerPvPDieUserRevive3();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerPvPDieUserRevive3();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerPvPDieUserRevive4 : public TimerSig
{
public:
    TimerPvPDieUserRevive4();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerPvPDieUserRevive4();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerPvPDieUserRevive5 : public TimerSig
{
public:
    TimerPvPDieUserRevive5();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerPvPDieUserRevive5();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerPvPDieUserRevive6 : public TimerSig
{
public:
    TimerPvPDieUserRevive6();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerPvPDieUserRevive6();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerPvPDieUserRevive7 : public TimerSig
{
public:
    TimerPvPDieUserRevive7();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerPvPDieUserRevive7();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerDungeonDataStatistic : public TimerSig
{
public:
    TimerDungeonDataStatistic();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerDungeonDataStatistic();
    static void RegistNextTimer();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerNotResponseUserTimeout : public TimerSig
{
public:
    TimerNotResponseUserTimeout();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerNotResponseUserTimeout();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerLightServerNotice : public TimerSig
{
public:
    TimerLightServerNotice();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerLightServerNotice();
    static void RegistNextTimer(int count);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerInsertUserPlayTime : public TimerSig
{
public:
    TimerInsertUserPlayTime();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerInsertUserPlayTime();
    static void RegistNextTimer();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerCheckForcedDisconnect : public TimerSig
{
public:
    TimerCheckForcedDisconnect();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerCheckForcedDisconnect();
    static bool RegistNextTimer(CUser* user);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerCheckPvPRoomUser : public TimerSig
{
public:
    TimerCheckPvPRoomUser();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerCheckPvPRoomUser();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerCheckRecvPvpEndResult : public TimerSig
{
public:
    TimerCheckRecvPvpEndResult();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerCheckRecvPvpEndResult();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerCheckHumanCertifyTimeout : public TimerSig
{
public:
    TimerCheckHumanCertifyTimeout();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerCheckHumanCertifyTimeout();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerUpdateItemGenerateStatistics : public TimerSig
{
public:
    TimerUpdateItemGenerateStatistics();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerUpdateItemGenerateStatistics();
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
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerUpdatePvPGrade();
    static void registNextTimer(long sec);   // ORIG T 0x86373f2（参数为 long）

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerDetectDisconnect : public TimerSig
{
public:
    TimerDetectDisconnect();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerDetectDisconnect();
    static void registNextTimer();   // ORIG T 0x86374e0

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerCompleteLoadAssaultTimeout : public TimerSig
{
public:
    TimerCompleteLoadAssaultTimeout();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerCompleteLoadAssaultTimeout();
    static void registNextTimer(long sec, int p1, int p2);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerConnectP2PAssaultTimeout : public TimerSig
{
public:
    TimerConnectP2PAssaultTimeout();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerConnectP2PAssaultTimeout();
    static void registNextTimer(long sec, int p1, int p2);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerEndAssault : public TimerSig
{
public:
    TimerEndAssault();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerEndAssault();
    static void registNextTimer(long sec, int p1, int p2);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerCloseAssault : public TimerSig
{
public:
    TimerCloseAssault();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerCloseAssault();
    static void registNextTimer(long sec, int p1, int p2);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerCompleteLoadAfterAssaultTimeout : public TimerSig
{
public:
    TimerCompleteLoadAfterAssaultTimeout();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerCompleteLoadAfterAssaultTimeout();
    static void registNextTimer(long sec, int p1, int p2);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerConnectP2PAfterAssaultTimeout : public TimerSig
{
public:
    TimerConnectP2PAfterAssaultTimeout();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerConnectP2PAfterAssaultTimeout();
    static void registNextTimer(long sec, int p1, int p2);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerDeathTower : public TimerSig
{
public:
    TimerDeathTower();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerDeathTower();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerDeathTowerTopRankerReload : public TimerSig
{
public:
    TimerDeathTowerTopRankerReload();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerDeathTowerTopRankerReload();
    static void registNextTimer(long ms);    // ORIG T 0x8637854

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerScheduleTenSecond : public TimerSig
{
public:
    TimerScheduleTenSecond();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerScheduleTenSecond();
    static void registNextTimer(unsigned int sec);  // ORIG T 0x8637a62

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerFinishItemEffect : public TimerSig
{
public:
    TimerFinishItemEffect();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerFinishItemEffect();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerStartMatching : public TimerSig
{
public:
    TimerStartMatching();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerStartMatching();
    static void registNextTimer(long sec, int p1, int p2);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerGlobalEffectItemBegin : public TimerSig
{
public:
    TimerGlobalEffectItemBegin();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerGlobalEffectItemBegin();
    static void registTimer(unsigned int type, unsigned int beginSec);   // ORIG T 0x8637e78

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerGlobalEffectItemTimeout : public TimerSig
{
public:
    TimerGlobalEffectItemTimeout();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerGlobalEffectItemTimeout();
    static void registTimer(unsigned int type, unsigned int timeoutSec);  // ORIG T 0x8637ef6

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerVendingMachineItemNotify : public TimerSig
{
public:
    TimerVendingMachineItemNotify();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerVendingMachineItemNotify();
    static void registTimer(int uid, unsigned long delayMs, unsigned int type, unsigned int param);  // ORIG T 0x8638074

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerConnectMonitorServer : public TimerSig
{
public:
    TimerConnectMonitorServer();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerConnectMonitorServer();
    static void registNextTimer(long sec, int p1, int p2);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerAssaultSuperPowerState : public TimerSig
{
public:
    TimerAssaultSuperPowerState();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerAssaultSuperPowerState();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerNPCMoodChange : public TimerSig
{
public:
    TimerNPCMoodChange();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerNPCMoodChange();
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
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerElevatorDungeon();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerRefreshPowerWarProcessInfo : public TimerSig
{
public:
    TimerRefreshPowerWarProcessInfo();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerRefreshPowerWarProcessInfo();
    static void registNextTimer(long sec);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerGeneratePowerUp : public TimerSig
{
public:
    TimerGeneratePowerUp();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerGeneratePowerUp();

    void generatePowerUp(std::vector<CUser*>& users, int count);
    void sendPowerUpInfo(int team1Count, int team2Count);
    void registNextTimer(long sec);

private:
    std::vector<CUser*> m_team1Users;
    std::vector<CUser*> m_team2Users;
};

class TimerGeneratePowerUpNotiTime : public TimerSig
{
public:
    TimerGeneratePowerUpNotiTime();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerGeneratePowerUpNotiTime();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerCheckPowerWarAbuser : public TimerSig
{
public:
    TimerCheckPowerWarAbuser();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerCheckPowerWarAbuser();
    static void registNextTimer(long sec);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerSetPunishUser : public TimerSig
{
public:
    TimerSetPunishUser();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerSetPunishUser();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerSetTradePunishUser : public TimerSig
{
public:
    TimerSetTradePunishUser();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerSetTradePunishUser();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerRevivalCharac : public TimerSig
{
public:
    TimerRevivalCharac();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerRevivalCharac();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerRequestExit : public TimerSig
{
public:
    TimerRequestExit();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerRequestExit();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerVillageMonsterUpdateRate : public TimerSig
{
public:
    TimerVillageMonsterUpdateRate();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerVillageMonsterUpdateRate();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerVillageMonsterRegen : public TimerSig
{
public:
    TimerVillageMonsterRegen();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerVillageMonsterRegen();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerStartVillageAttacked : public TimerSig
{
public:
    TimerStartVillageAttacked();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerStartVillageAttacked();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerVillageAttackedPlayTimeout : public TimerSig
{
public:
    TimerVillageAttackedPlayTimeout();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerVillageAttackedPlayTimeout();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerItemUnlockWaiting : public TimerSig
{
public:
    TimerItemUnlockWaiting();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerItemUnlockWaiting();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerUpdateCompoundEmblemStatics : public TimerSig
{
public:
    TimerUpdateCompoundEmblemStatics();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerUpdateCompoundEmblemStatics();
    static void registNextTimer();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerStartExpEventAfterVillageAttack : public TimerSig
{
public:
    TimerStartExpEventAfterVillageAttack();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerStartExpEventAfterVillageAttack();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_Security_NonClient_CheckResponse : public TimerSig
{
public:
    Timer_Security_NonClient_CheckResponse();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_Security_NonClient_CheckResponse();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_Midnight : public TimerSig
{
public:
    Timer_Midnight();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_Midnight();
    static bool RegistNextTimer();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerEndPowerWarVictoriousReward : public TimerSig
{
public:
    TimerEndPowerWarVictoriousReward();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerEndPowerWarVictoriousReward();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_TenMinuteSchedule : public TimerSig
{
public:
    Timer_TenMinuteSchedule();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_TenMinuteSchedule();
    static bool RegistNextTimer(int type);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_UpdateOnlinePreliminaryTeam : public TimerSig
{
public:
    Timer_UpdateOnlinePreliminaryTeam();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_UpdateOnlinePreliminaryTeam();
    static bool RegistNextTimer(int team);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_SecretShopStatistic : public TimerSig
{
public:
    Timer_SecretShopStatistic();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_SecretShopStatistic();
    static void RegistNextTimer();           // ORIG T 0x86397ee

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_DailySchedule : public TimerSig
{
public:
    Timer_DailySchedule();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_DailySchedule();
    static void RegistNextTimer();           // ORIG T 0x812c940

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_ConnectP2PPvPTimeout : public TimerSig
{
public:
    Timer_ConnectP2PPvPTimeout();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_ConnectP2PPvPTimeout();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_RoutingItemWait : public TimerSig
{
public:
    Timer_RoutingItemWait();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_RoutingItemWait();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_PcRoomAuthRetry : public TimerSig
{
public:
    Timer_PcRoomAuthRetry();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_PcRoomAuthRetry();
    static void registNextTimer(long sec, int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_DungeonInoutOpenTime : public TimerSig
{
public:
    Timer_DungeonInoutOpenTime();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_DungeonInoutOpenTime();
    static void RegistNextTimer();           // ORIG T 0x8639b20（定义在 GameMain.cpp）
    static bool checkDungeonInoutTime(int day);  // ORIG T 0x8639a12

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_DungeonInoutCloseTime : public TimerSig
{
public:
    Timer_DungeonInoutCloseTime();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_DungeonInoutCloseTime();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_FairPvPRematch : public TimerSig
{
public:
    Timer_FairPvPRematch();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_FairPvPRematch();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_ReturnUserTimeout : public TimerSig
{
public:
    Timer_ReturnUserTimeout();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_ReturnUserTimeout();
    static void registNextTimer(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_HadesHeartBeat : public TimerSig
{
public:
    Timer_HadesHeartBeat();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_HadesHeartBeat();
    static void registNextTimer();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_TOD_Send_ClearReward : public TimerSig
{
public:
    Timer_TOD_Send_ClearReward();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_TOD_Send_ClearReward();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_OneADayItemShop : public TimerSig
{
public:
    Timer_OneADayItemShop();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_OneADayItemShop();
    static void RegistNextTimer();           // ORIG T 0x863a93c

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_Schedule_MidNight : public TimerSig
{
public:
    Timer_Schedule_MidNight();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_Schedule_MidNight();
    static void RegistNextTimer();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_Send_Ontime_Reward : public TimerSig
{
public:
    Timer_Send_Ontime_Reward();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_Send_Ontime_Reward();
    static void RegistNextTimer();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_EventManagement : public TimerSig
{
public:
    Timer_EventManagement();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_EventManagement();
    static void RegistNextTimer(int intervalMs);  // ORIG T 0x863abd4

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_QuickPartyWaitingPartyMatching : public TimerSig
{
public:
    Timer_QuickPartyWaitingPartyMatching();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_QuickPartyWaitingPartyMatching();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_QuickPartyWaitingStartGame : public TimerSig
{
public:
    Timer_QuickPartyWaitingStartGame();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_QuickPartyWaitingStartGame();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_AssaultGhostTimer : public TimerSig
{
public:
    Timer_AssaultGhostTimer();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_AssaultGhostTimer();
    static void RegisterNextTimer(CUser* user, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_GenerateCaptchaData : public TimerSig
{
public:
    Timer_GenerateCaptchaData();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_GenerateCaptchaData();
    static void RegistNextTimer(int sec);    // ORIG T 0x863b2ba

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_TournamentDugeonClearStateNotiReward : public TimerSig
{
public:
    Timer_TournamentDugeonClearStateNotiReward();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_TournamentDugeonClearStateNotiReward();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_TournametDungeonClearStateSelect : public TimerSig
{
public:
    Timer_TournametDungeonClearStateSelect();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_TournametDungeonClearStateSelect();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_TowerOnTime : public TimerSig
{
public:
    Timer_TowerOnTime();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_TowerOnTime();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_NoticeEmergencyPatchMessage : public TimerSig
{
public:
    Timer_NoticeEmergencyPatchMessage();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_NoticeEmergencyPatchMessage();
    static void RegistNextTimer();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_DungeonStatisticEntrance_Hour : public TimerSig
{
public:
    Timer_DungeonStatisticEntrance_Hour();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_DungeonStatisticEntrance_Hour();
    static void RegistNextTimer();        // ORIG T 0x863b9fa

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_RefreshServerState : public TimerSig
{
public:
    Timer_RefreshServerState();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_RefreshServerState();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_StayTimeEvent : public TimerSig
{
public:
    Timer_StayTimeEvent();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_StayTimeEvent();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_CreateDnfDbLoad : public TimerSig
{
public:
    Timer_CreateDnfDbLoad();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_CreateDnfDbLoad();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_CheckAuctionReady : public TimerSig
{
public:
    Timer_CheckAuctionReady();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_CheckAuctionReady();
    static void RegistNextTimer();
};

namespace advancealtar
{
class Timer_MainTick : public TimerSig
{
public:
    Timer_MainTick();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_MainTick();
static void RegistNextTimerGage();  // ORIG T 0x812f7b8（定义在 GameMain.cpp）
    static const unsigned int kMainTickMs;  // ORIG R 0x8b6c5e4

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_StageControl : public TimerSig
{
public:
    Timer_StageControl();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_StageControl();

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
