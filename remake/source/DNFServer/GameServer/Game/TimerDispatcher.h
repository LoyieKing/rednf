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
#include "advancealtar.h"  // advancealtar::StageEndType（RegistTimer* 静态参数）

class CUser;
namespace QuickParty { class CQuickParty; }
class TimerSig
{
public:
    virtual int dispatch_sig(int p1, int p2, unsigned int p3) = 0;
    // ORIG 无 TimerSig 独立符号（各 Timer 类 vtable/D1 自含基类析构内联）——
    // 基类析构 inline 避免产生 key function 弱符号（与 ORIG nm 一致）。
    virtual ~TimerSig() {}
};

class TimerCheckConn : public TimerSig
{
public:
    TimerCheckConn() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerCheckConn() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerDBUpdate : public TimerSig
{
public:
    TimerDBUpdate() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerDBUpdate() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerStamina : public TimerSig
{
public:
    TimerStamina() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerStamina() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

// [b28d-Main·检查点21] 非多态：ORIG 只有 `_ZN15TimerPvpRankRes12dispatch_sigEiij`
//   （0x08633be8，函数体不访问 this），无 vtable/typeinfo/ctor/dtor 符号
//   （对比 TimerMapLoading 有 typeinfo 0x08ceaa90）⇒ 原码不继承 TimerSig。
//   注意：必须是**非静态成员**——ORIG 以 this@ebp+0x8、p1@+0xc 取参（`mov 0xc(%ebp),%edx`
//   喂 GetPvp 第一实参）；改 static 会使实参整体下移 4 字节（P 级回归实证）。
class TimerPvpRankRes
{
public:
    int dispatch_sig(int p1, int p2, unsigned int p3);
};

class TimerMapLoading : public TimerSig
{
public:
    TimerMapLoading() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerMapLoading() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerSetResult : public TimerSig
{
public:
    TimerSetResult() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerSetResult() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerDungeonClear : public TimerSig
{
public:
    TimerDungeonClear() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerDungeonClear() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerDungeonFail : public TimerSig
{
public:
    TimerDungeonFail() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerDungeonFail() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerPartyWalkOutLackUser : public TimerSig
{
public:
    TimerPartyWalkOutLackUser() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerPartyWalkOutLackUser() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerPVPWalkOutLackUser : public TimerSig
{
public:
    TimerPVPWalkOutLackUser() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerPVPWalkOutLackUser() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerFatigueReset : public TimerSig
{
public:
    TimerFatigueReset() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerFatigueReset() throw();
    bool RegistNextTimer();     // ORIG 0x08633a04：调用点 0x086339d4 传 this ⇒ 非 static；
                                // 返回 byte 物化 mov %al,-0x9 ⇒ bool
    int updateAbnormalClientCount();     // ORIG T 0x086339fa（定义 TimerDispatcher.cpp）

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerAvatarExpire : public TimerSig
{
public:
    TimerAvatarExpire() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerAvatarExpire() throw();
    bool RegistTimer(CUser* user);                // ORIG 0x08632b32 非静态（this 未用），单出口 return true
    int CheckExpiredAvatar(CUser* user);          // ORIG T 0x08632a82 非静态（user=0xc(%ebp)）

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerStatisticsLog : public TimerSig
{
public:
    TimerStatisticsLog() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerStatisticsLog() throw();
    bool RegistNextTimer();     // ORIG 调用点 0x08633f08 段尾传 this（mov 0x8(%ebp)）⇒ 非 static

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerCardSelect : public TimerSig
{
public:
    TimerCardSelect() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerCardSelect() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerClearDungeonReward : public TimerSig
{
public:
    TimerClearDungeonReward() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerClearDungeonReward() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerClearDungeonScoreScoll : public TimerSig
{
public:
    TimerClearDungeonScoreScoll() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerClearDungeonScoreScoll() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerClearDungeonCardSelectRight : public TimerSig
{
public:
    TimerClearDungeonCardSelectRight() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerClearDungeonCardSelectRight() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerPremiumServiceOver : public TimerSig
{
public:
    TimerPremiumServiceOver() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerPremiumServiceOver() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerPremiumServiceUpdate : public TimerSig
{
public:
    TimerPremiumServiceUpdate() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerPremiumServiceUpdate() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerPremiumServiceStart : public TimerSig
{
public:
    TimerPremiumServiceStart() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerPremiumServiceStart() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerCheckPaidPCRoomTerm : public TimerSig
{
public:
    TimerCheckPaidPCRoomTerm() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerCheckPaidPCRoomTerm() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerEPLPReturnVillage : public TimerSig
{
public:
    TimerEPLPReturnVillage() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerEPLPReturnVillage() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerEPLPCommandWait : public TimerSig
{
public:
    TimerEPLPCommandWait() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerEPLPCommandWait() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerWarRoomWaitCount : public TimerSig
{
public:
    TimerWarRoomWaitCount() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerWarRoomWaitCount() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerWarRoomReadyLockCount : public TimerSig
{
public:
    TimerWarRoomReadyLockCount() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerWarRoomReadyLockCount() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerWarRoomBattlePauseCount : public TimerSig
{
public:
    TimerWarRoomBattlePauseCount() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerWarRoomBattlePauseCount() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerWarRoomResultCount : public TimerSig
{
public:
    TimerWarRoomResultCount() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerWarRoomResultCount() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TImerWarRoomTimeBomb : public TimerSig
{
public:
    TImerWarRoomTimeBomb() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TImerWarRoomTimeBomb() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerWarRoomCheckFail : public TimerSig
{
public:
    TimerWarRoomCheckFail() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerWarRoomCheckFail() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerWarRoomChangeHost : public TimerSig
{
public:
    TimerWarRoomChangeHost() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerWarRoomChangeHost() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerBloodSmallRoundRegister : public TimerSig
{
public:
    TimerBloodSmallRoundRegister() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerBloodSmallRoundRegister() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerBloodPhaseRegister0 : public TimerSig
{
public:
    TimerBloodPhaseRegister0() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerBloodPhaseRegister0() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerBloodPhaseRegister1 : public TimerSig
{
public:
    TimerBloodPhaseRegister1() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerBloodPhaseRegister1() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerBloodPhaseRegister2 : public TimerSig
{
public:
    TimerBloodPhaseRegister2() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerBloodPhaseRegister2() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerBloodPhaseRegister3 : public TimerSig
{
public:
    TimerBloodPhaseRegister3() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerBloodPhaseRegister3() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerBloodPhaseRegister4 : public TimerSig
{
public:
    TimerBloodPhaseRegister4() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerBloodPhaseRegister4() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerBloodPhaseRegister5 : public TimerSig
{
public:
    TimerBloodPhaseRegister5() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerBloodPhaseRegister5() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerBloodPhaseRegister6 : public TimerSig
{
public:
    TimerBloodPhaseRegister6() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerBloodPhaseRegister6() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerBloodPhaseRegister7 : public TimerSig
{
public:
    TimerBloodPhaseRegister7() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerBloodPhaseRegister7() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerBloodPhaseRegister8 : public TimerSig
{
public:
    TimerBloodPhaseRegister8() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerBloodPhaseRegister8() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerBloodPhaseRegister9 : public TimerSig
{
public:
    TimerBloodPhaseRegister9() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerBloodPhaseRegister9() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerBloodCrazyMonster : public TimerSig
{
public:
    TimerBloodCrazyMonster() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerBloodCrazyMonster() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerBloodDungeonClearReward : public TimerSig
{
public:
    TimerBloodDungeonClearReward() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerBloodDungeonClearReward() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerUltimateNextRoundStart : public TimerSig
{
public:
    TimerUltimateNextRoundStart() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerUltimateNextRoundStart() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerEventChristmas : public TimerSig
{
public:
    TimerEventChristmas() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerEventChristmas() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerStatisticsDetailChannelInfo : public TimerSig
{
public:
    TimerStatisticsDetailChannelInfo() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerStatisticsDetailChannelInfo() throw();
    bool RegistNextTimer();   // ORIG 0x08635782（Ev 非 static；调用点传 this）；
                              // SIGFIX：ORIG 串 "bool TimerStatisticsDetailChannelInfo::RegistNextTimer()"

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerStatisticLevel : public TimerSig
{
public:
    TimerStatisticLevel() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerStatisticLevel() throw();
    static int CalcNextTime();               // ORIG T 0x863591a（定义在 GameMain.cpp）
    static int m_nTimeTable[5];               // ORIG D 0x93ace24 {1,10,14,17,21}

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerScheduleOneHour : public TimerSig
{
public:
    TimerScheduleOneHour() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerScheduleOneHour() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerSaveLogGamechannel : public TimerSig
{
public:
    TimerSaveLogGamechannel() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerSaveLogGamechannel() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerQueryCounter : public TimerSig
{
public:
    TimerQueryCounter() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerQueryCounter() throw();
    int RegistNextTimer();      // ORIG 0x08635bdc：调用点传 this（mov 0x8(%ebp)）⇒ 非 static

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerNatType : public TimerSig
{
public:
    TimerNatType() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerNatType() throw();
    int RegistNextTimer();   // ORIG 0x08635c5a（Ev 非 static；dispatch_sig 传 this）

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerNatTypeUser : public TimerSig
{
public:
    TimerNatTypeUser() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerNatTypeUser() throw();
    int RegistNextTimer();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerCheckMailTimeOut : public TimerSig
{
public:
    TimerCheckMailTimeOut() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerCheckMailTimeOut() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerCreature : public TimerSig
{
public:
    TimerCreature() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerCreature() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerForbidChat : public TimerSig
{
public:
    TimerForbidChat() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerForbidChat() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerForbidMove : public TimerSig
{
public:
    TimerForbidMove() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerForbidMove() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerScheduleFiveMinPerUser : public TimerSig
{
public:
    TimerScheduleFiveMinPerUser() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerScheduleFiveMinPerUser() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerScheduleTenMinPerUser : public TimerSig
{
public:
    TimerScheduleTenMinPerUser() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerScheduleTenMinPerUser() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerQueueSizeLog : public TimerSig
{
public:
    TimerQueueSizeLog() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerQueueSizeLog() throw();
    bool RegistNextTimer();   // ORIG 调用点 0x08636322 传 this（0x8(%ebp)）⇒ 非 static

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerScheduleGuildWarEnd : public TimerSig
{
public:
    TimerScheduleGuildWarEnd() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerScheduleGuildWarEnd() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerSetPlayResult : public TimerSig
{
public:
    TimerSetPlayResult() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerSetPlayResult() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerStartRelayBattle : public TimerSig
{
public:
    TimerStartRelayBattle() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerStartRelayBattle() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerBossDieTimeout : public TimerSig
{
public:
    TimerBossDieTimeout() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerBossDieTimeout() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerEventCoinRefill : public TimerSig
{
public:
    TimerEventCoinRefill() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerEventCoinRefill() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerPvPDieUserRevive0 : public TimerSig
{
public:
    TimerPvPDieUserRevive0() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerPvPDieUserRevive0() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerPvPDieUserRevive1 : public TimerSig
{
public:
    TimerPvPDieUserRevive1() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerPvPDieUserRevive1() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerPvPDieUserRevive2 : public TimerSig
{
public:
    TimerPvPDieUserRevive2() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerPvPDieUserRevive2() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerPvPDieUserRevive3 : public TimerSig
{
public:
    TimerPvPDieUserRevive3() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerPvPDieUserRevive3() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerPvPDieUserRevive4 : public TimerSig
{
public:
    TimerPvPDieUserRevive4() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerPvPDieUserRevive4() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerPvPDieUserRevive5 : public TimerSig
{
public:
    TimerPvPDieUserRevive5() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerPvPDieUserRevive5() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerPvPDieUserRevive6 : public TimerSig
{
public:
    TimerPvPDieUserRevive6() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerPvPDieUserRevive6() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerPvPDieUserRevive7 : public TimerSig
{
public:
    TimerPvPDieUserRevive7() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerPvPDieUserRevive7() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerDungeonDataStatistic : public TimerSig
{
public:
    TimerDungeonDataStatistic() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerDungeonDataStatistic() throw();
    bool RegistNextTimer();        // ORIG T 0x08636856：调用点 0x08636805 传 this ⇒
                                   // 非 static；`mov %al,-0x9` byte 物化 ⇒ bool

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerNotResponseUserTimeout : public TimerSig
{
public:
    TimerNotResponseUserTimeout() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerNotResponseUserTimeout() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerLightServerNotice : public TimerSig
{
public:
    TimerLightServerNotice() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerLightServerNotice() throw();
    bool RegistNextTimer(int count);   // ORIG T 0x08636c14（恒返 1，al；读 0xc(%ebp)
                                       // 为参 ⇒ this@0x8 非 static 成员）

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerInsertUserPlayTime : public TimerSig
{
public:
    TimerInsertUserPlayTime() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerInsertUserPlayTime() throw();
    bool RegistNextTimer();            // ORIG T 0x08636d44；调用点 0x08636d23 传 this ⇒ 非 static

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerCheckForcedDisconnect : public TimerSig
{
public:
    TimerCheckForcedDisconnect() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerCheckForcedDisconnect() throw();
    bool RegistNextTimer(CUser* user);   // ORIG 0x08636f26：体内取 0xc(%ebp)=user ⇒ 带 this 成员

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerCheckPvPRoomUser : public TimerSig
{
public:
    TimerCheckPvPRoomUser() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerCheckPvPRoomUser() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerCheckRecvPvpEndResult : public TimerSig
{
public:
    TimerCheckRecvPvpEndResult() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerCheckRecvPvpEndResult() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerCheckHumanCertifyTimeout : public TimerSig
{
public:
    TimerCheckHumanCertifyTimeout() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerCheckHumanCertifyTimeout() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerUpdateItemGenerateStatistics : public TimerSig
{
public:
    TimerUpdateItemGenerateStatistics() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerUpdateItemGenerateStatistics() throw();
    // ORIG T 0x08637318 _ZN33TimerUpdateItemGenerateStatistics15registNextTimerEl
    // （CItemGeneratorMgr::initialize 尾部按当前秒注册下一次统计定时器；定义待定时器域批次）
    static void registNextTimer(long sec);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerUpdatePvPGrade : public TimerSig
{
public:
    TimerUpdatePvPGrade() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerUpdatePvPGrade() throw();
    static void registNextTimer(long sec);   // ORIG T 0x86373f2（参数为 long）

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerDetectDisconnect : public TimerSig
{
public:
    TimerDetectDisconnect() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerDetectDisconnect() throw();
    static void registNextTimer();   // ORIG T 0x86374e0

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerCompleteLoadAssaultTimeout : public TimerSig
{
public:
    TimerCompleteLoadAssaultTimeout() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerCompleteLoadAssaultTimeout() throw();
    static void registNextTimer(long sec, int p1, int p2);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerConnectP2PAssaultTimeout : public TimerSig
{
public:
    TimerConnectP2PAssaultTimeout() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerConnectP2PAssaultTimeout() throw();
    static void registNextTimer(long sec, int p1, int p2);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerEndAssault : public TimerSig
{
public:
    TimerEndAssault() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerEndAssault() throw();
    static void registNextTimer(long sec, int p1, int p2);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerCloseAssault : public TimerSig
{
public:
    TimerCloseAssault() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerCloseAssault() throw();
    static void registNextTimer(long sec, int p1, int p2);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerCompleteLoadAfterAssaultTimeout : public TimerSig
{
public:
    TimerCompleteLoadAfterAssaultTimeout() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerCompleteLoadAfterAssaultTimeout() throw();
    static void registNextTimer(long sec, int p1, int p2);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerConnectP2PAfterAssaultTimeout : public TimerSig
{
public:
    TimerConnectP2PAfterAssaultTimeout() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerConnectP2PAfterAssaultTimeout() throw();
    static void registNextTimer(long sec, int p1, int p2);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerDeathTower : public TimerSig
{
public:
    TimerDeathTower() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerDeathTower() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerDeathTowerTopRankerReload : public TimerSig
{
public:
    TimerDeathTowerTopRankerReload() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerDeathTowerTopRankerReload() throw();
    static void registNextTimer(long ms);    // ORIG T 0x8637854

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerScheduleTenSecond : public TimerSig
{
public:
    TimerScheduleTenSecond() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerScheduleTenSecond() throw();
    static void registNextTimer(unsigned int sec);  // ORIG T 0x8637a62

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerFinishItemEffect : public TimerSig
{
public:
    TimerFinishItemEffect() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerFinishItemEffect() throw();

private:
    void _processSepecialEffectContinuousItem(CUser* user, unsigned int param);  // ORIG T 0x08637d64
};


class TimerStartMatching : public TimerSig
{
public:
    TimerStartMatching() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerStartMatching() throw();
    static void registNextTimer(long sec, int p1, int p2);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerGlobalEffectItemBegin : public TimerSig
{
public:
    TimerGlobalEffectItemBegin() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerGlobalEffectItemBegin() throw();
    static void registTimer(unsigned int type, unsigned int beginSec);   // ORIG T 0x8637e78

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerGlobalEffectItemTimeout : public TimerSig
{
public:
    TimerGlobalEffectItemTimeout() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerGlobalEffectItemTimeout() throw();
    static void registTimer(unsigned int type, unsigned int timeoutSec);  // ORIG T 0x8637ef6

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerVendingMachineItemNotify : public TimerSig
{
public:
    TimerVendingMachineItemNotify() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerVendingMachineItemNotify() throw();
    static void registTimer(int uid, unsigned long delayMs, unsigned int type, unsigned int param);  // ORIG T 0x8638074

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerConnectMonitorServer : public TimerSig
{
public:
    TimerConnectMonitorServer() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerConnectMonitorServer() throw();
    static void registNextTimer(long sec, int p1, int p2);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerAssaultSuperPowerState : public TimerSig
{
public:
    TimerAssaultSuperPowerState() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerAssaultSuperPowerState() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerNPCMoodChange : public TimerSig
{
public:
    TimerNPCMoodChange() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerNPCMoodChange() throw();
    // ORIG T 0x086381ec _ZN18TimerNPCMoodChange15registNextTimerEl：
    // 纯 ret（空实现）；CNPCScriptList::initScript 末尾以参数 0 调用。
    static void registNextTimer(long sec);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerElevatorDungeon : public TimerSig
{
public:
    TimerElevatorDungeon() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerElevatorDungeon() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerRefreshPowerWarProcessInfo : public TimerSig
{
public:
    TimerRefreshPowerWarProcessInfo() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerRefreshPowerWarProcessInfo() throw();
    static void registNextTimer(long sec);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerGeneratePowerUp : public TimerSig
{
public:
    // [SweepTimerDispatcher2] ORIG 对 `new TimerGeneratePowerUp()` 布 EH landing
    // （C1 0x086321d6 唯一 _ZdlPv/_Unwind_Resume 处，ctor 含 2 个 vector 成员）⇒ 本类
    // ctor **不加** throw()（其余 146 类 ctor 均 throw() 消 landing）。
    TimerGeneratePowerUp();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerGeneratePowerUp() throw();

    void generatePowerUp(std::vector<CUser*>& users, int count);
    void sendPowerUpInfo(int team1Count, int team2Count);
    static void registNextTimer(long sec);   // ORIG static（sec@+0x8）

private:
    std::vector<CUser*> m_team1Users;
    std::vector<CUser*> m_team2Users;
};

class TimerGeneratePowerUpNotiTime : public TimerSig
{
public:
    TimerGeneratePowerUpNotiTime() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerGeneratePowerUpNotiTime() throw();
    static void registNextTimer(long sec);  // ORIG T 0x0863847e，无 this
    void send_noti_time(int sec);           // ORIG T 0x086384c2，thiscall
private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerCheckPowerWarAbuser : public TimerSig
{
public:
    TimerCheckPowerWarAbuser() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerCheckPowerWarAbuser() throw();
    static void registNextTimer(long sec);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerSetPunishUser : public TimerSig
{
public:
    TimerSetPunishUser() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerSetPunishUser() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerSetTradePunishUser : public TimerSig
{
public:
    TimerSetTradePunishUser() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerSetTradePunishUser() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerRevivalCharac : public TimerSig
{
public:
    TimerRevivalCharac() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerRevivalCharac() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerRequestExit : public TimerSig
{
public:
    TimerRequestExit() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerRequestExit() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerVillageMonsterUpdateRate : public TimerSig
{
public:
    TimerVillageMonsterUpdateRate() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerVillageMonsterUpdateRate() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerVillageMonsterRegen : public TimerSig
{
public:
    TimerVillageMonsterRegen() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerVillageMonsterRegen() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerStartVillageAttacked : public TimerSig
{
public:
    TimerStartVillageAttacked() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerStartVillageAttacked() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerVillageAttackedPlayTimeout : public TimerSig
{
public:
    TimerVillageAttackedPlayTimeout() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerVillageAttackedPlayTimeout() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerItemUnlockWaiting : public TimerSig
{
public:
    TimerItemUnlockWaiting() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerItemUnlockWaiting() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerUpdateCompoundEmblemStatics : public TimerSig
{
public:
    TimerUpdateCompoundEmblemStatics() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerUpdateCompoundEmblemStatics() throw();
    static int registNextTimer();  // ORIG T 0x0863939a，eax=1

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerStartExpEventAfterVillageAttack : public TimerSig
{
public:
    TimerStartExpEventAfterVillageAttack() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerStartExpEventAfterVillageAttack() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_Security_NonClient_CheckResponse : public TimerSig
{
public:
    Timer_Security_NonClient_CheckResponse() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_Security_NonClient_CheckResponse() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_Midnight : public TimerSig
{
public:
    Timer_Midnight() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_Midnight() throw();
    static bool RegistNextTimer();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class TimerEndPowerWarVictoriousReward : public TimerSig
{
public:
    TimerEndPowerWarVictoriousReward() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~TimerEndPowerWarVictoriousReward() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_TenMinuteSchedule : public TimerSig
{
public:
    Timer_TenMinuteSchedule() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_TenMinuteSchedule() throw();
    static bool RegistNextTimer(int type);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_UpdateOnlinePreliminaryTeam : public TimerSig
{
public:
    Timer_UpdateOnlinePreliminaryTeam() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_UpdateOnlinePreliminaryTeam() throw();
    static bool RegistNextTimer(int team);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_SecretShopStatistic : public TimerSig
{
public:
    Timer_SecretShopStatistic() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_SecretShopStatistic() throw();
    static void RegistNextTimer();           // ORIG T 0x86397ee

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_DailySchedule : public TimerSig
{
public:
    Timer_DailySchedule() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_DailySchedule() throw();
    static void RegistNextTimer();           // ORIG T 0x812c940

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_ConnectP2PPvPTimeout : public TimerSig
{
public:
    Timer_ConnectP2PPvPTimeout() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_ConnectP2PPvPTimeout() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_RoutingItemWait : public TimerSig
{
public:
    Timer_RoutingItemWait() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_RoutingItemWait() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_PcRoomAuthRetry : public TimerSig
{
public:
    Timer_PcRoomAuthRetry() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_PcRoomAuthRetry() throw();
    static void registNextTimer(long sec, int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_DungeonInoutOpenTime : public TimerSig
{
public:
    Timer_DungeonInoutOpenTime() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_DungeonInoutOpenTime() throw();
    static void RegistNextTimer();           // ORIG T 0x8639b20（定义在 GameMain.cpp）
    static bool checkDungeonInoutTime(int day);  // ORIG T 0x8639a12

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_DungeonInoutCloseTime : public TimerSig
{
public:
    Timer_DungeonInoutCloseTime() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_DungeonInoutCloseTime() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_FairPvPRematch : public TimerSig
{
public:
    Timer_FairPvPRematch() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_FairPvPRematch() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_ReturnUserTimeout : public TimerSig
{
public:
    Timer_ReturnUserTimeout() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_ReturnUserTimeout() throw();
    static void registNextTimer(int p1, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_HadesHeartBeat : public TimerSig
{
public:
    Timer_HadesHeartBeat() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_HadesHeartBeat() throw();
    static void registNextTimer();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_TOD_Send_ClearReward : public TimerSig
{
public:
    Timer_TOD_Send_ClearReward() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_TOD_Send_ClearReward() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_OneADayItemShop : public TimerSig
{
public:
    Timer_OneADayItemShop() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_OneADayItemShop() throw();
    static void RegistNextTimer();           // ORIG T 0x863a93c

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_Schedule_MidNight : public TimerSig
{
public:
    Timer_Schedule_MidNight() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_Schedule_MidNight() throw();
    static void RegistNextTimer();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_Send_Ontime_Reward : public TimerSig
{
public:
    Timer_Send_Ontime_Reward() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_Send_Ontime_Reward() throw();
    static void RegistNextTimer();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_EventManagement : public TimerSig
{
public:
    Timer_EventManagement() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_EventManagement() throw();
    static void RegistNextTimer(int intervalMs);  // ORIG T 0x863abd4

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_QuickPartyWaitingPartyMatching : public TimerSig
{
public:
    Timer_QuickPartyWaitingPartyMatching() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_QuickPartyWaitingPartyMatching() throw();
    bool cancel_quick_party(QuickParty::CQuickParty* qp);  // ORIG T 0x863ad96

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_QuickPartyWaitingStartGame : public TimerSig
{
public:
    Timer_QuickPartyWaitingStartGame() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_QuickPartyWaitingStartGame() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_AssaultGhostTimer : public TimerSig
{
public:
    Timer_AssaultGhostTimer() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_AssaultGhostTimer() throw();
    static void RegisterNextTimer(CUser* user, int p2, unsigned int p3);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_GenerateCaptchaData : public TimerSig
{
public:
    Timer_GenerateCaptchaData() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_GenerateCaptchaData() throw();
    static void RegistNextTimer(int sec);    // ORIG T 0x863b2ba

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_TournamentDugeonClearStateNotiReward : public TimerSig
{
public:
    Timer_TournamentDugeonClearStateNotiReward() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_TournamentDugeonClearStateNotiReward() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_TournametDungeonClearStateSelect : public TimerSig
{
public:
    Timer_TournametDungeonClearStateSelect() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_TournametDungeonClearStateSelect() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_TowerOnTime : public TimerSig
{
public:
    Timer_TowerOnTime() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_TowerOnTime() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_NoticeEmergencyPatchMessage : public TimerSig
{
public:
    Timer_NoticeEmergencyPatchMessage() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_NoticeEmergencyPatchMessage() throw();
    static void RegistNextTimer();

private:
    // ORIG 对象尺寸 0x8：vptr + 剩余公告次数（ctor 0x0863bc2e objdump 实证
    // movl $0xa,0x4(%eax)；dispatch_sig 0x0863b7c2 读 this+0x04 递减）
    int m_remainingNotiCount;   // +0x04
};

class Timer_DungeonStatisticEntrance_Hour : public TimerSig
{
public:
    Timer_DungeonStatisticEntrance_Hour() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_DungeonStatisticEntrance_Hour() throw();
    static void RegistNextTimer();        // ORIG T 0x863b9fa

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_RefreshServerState : public TimerSig
{
public:
    Timer_RefreshServerState() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_RefreshServerState() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_StayTimeEvent : public TimerSig
{
public:
    Timer_StayTimeEvent() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_StayTimeEvent() throw();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_CreateDnfDbLoad : public TimerSig
{
public:
    Timer_CreateDnfDbLoad() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_CreateDnfDbLoad() throw();

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
    Timer_MainTick() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_MainTick() throw();
static void RegistNextTimerGage();  // ORIG T 0x812f7b8（定义在 GameMain.cpp）
    static const unsigned int kMainTickMs;  // ORIG R 0x8b6c5e4

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Timer_StageControl : public TimerSig
{
public:
    Timer_StageControl() throw();
    virtual int dispatch_sig(int p1, int p2, unsigned int p3);
    virtual ~Timer_StageControl() throw();

    // ORIG T 0x812f850 / 0x812f89c / 0x812f8ec / 0x0812f956（无 this 静态）。
    static void RegistTimerCheckFinsh(int stage);
    static void RegistTimerClearReward(int stage, advancealtar::StageEndType::T endType);
    static void RegistTimerEPLP(int stage, advancealtar::StageEndType::T endType);
    static void RegistTimerEPLPReturnVillage(int stage);

    // ORIG R 0x8b6c5e8..0x8b6c5fc（外部链接类静态成员，类外定义 TimerDispatcher.cpp）
    static const unsigned int kFailEPLPMs = 1000;    // 0x8b6c5f4（in-class 常量，ORIG 使用处折叠为立即数）
    static const unsigned int kClearEPLPMs = 1000;   // 0x8b6c5f0
    static const unsigned int kCheckFinishMs = 5000; // 0x8b6c5e8
    static const unsigned int kClearRewardMs = 3000; // 0x8b6c5ec
    static const unsigned int kEPLPReturnVillageMs = 50000; // 0x8b6c5f8

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
