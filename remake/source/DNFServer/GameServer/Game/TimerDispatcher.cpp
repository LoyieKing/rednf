#include "TimerDispatcher.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <new>
#include <string>
#include <vector>

#include "GlobalData.h"
#include "GameWorld.h"
#include "CGameManager.h"
#include "CDataManager.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CParty.h"
#include "PvP_Room.h"
#include "WarRoom.h"
#include "CBattle_Field.h"
#include "CPowerManager.h"
#include "PacketGuard.h"
#include "CEnvironment.h"
#include "CEventScriptMng.h"
#include "CEventTable.h"
#include "CServerEvent.h"
#include "InterDispatcher.h"
#include "CLogGameChannel.h"
#include "CCommonStatisticsMgr.h"
#include "CDungeonEntranceLog.h"
#include "CLevelDungeonPlayStatistic.h"
#include "CCirculationStatistic.h"
#include "CInventory.h"
#include "CUserProc.h"
#include "MsgQueueMgr.h"
#include "CStreamGuard.h"
#include "CGuildServerProxy.h"   // CEventManager 完整定义（GlobalData.h 仅前向）
#include "CMailBoxHelper.h"
#include "CHandlePremium.h"
#include "SPremiumInfo.h"
// ======== 基类 ========

TimerSig::~TimerSig() {}

// ======== 主分发器（逐指令转写自 docs/class_func_reports/TimerDispatcher/）=====

// ORIG 0x863120a：清零槽 0..0xae 后逐项 new 并按 ORIG 顺序写入槽位。
TimerDispatcher::TimerDispatcher()
{
    for (int i = 0; i <= 0xae; ++i)
    {
        m_dispatchers[i] = 0;
    }
    m_dispatchers[0] = new TimerCheckConn();  // slot +0x000
    m_dispatchers[1] = new TimerDBUpdate();  // slot +0x004
    m_dispatchers[2] = new TimerStamina();  // slot +0x008
    m_dispatchers[11] = new TimerMapLoading();  // slot +0x02c
    m_dispatchers[12] = new TimerSetResult();  // slot +0x030
    m_dispatchers[13] = new TimerDungeonClear();  // slot +0x034
    m_dispatchers[14] = new TimerDungeonFail();  // slot +0x038
    m_dispatchers[15] = new TimerPartyWalkOutLackUser();  // slot +0x03c
    m_dispatchers[49] = new TimerPVPWalkOutLackUser();  // slot +0x0c4
    m_dispatchers[80] = new TimerFatigueReset();  // slot +0x140
    m_dispatchers[81] = new TimerAvatarExpire();  // slot +0x144
    m_dispatchers[82] = new TimerStatisticsLog();  // slot +0x148
    m_dispatchers[16] = new TimerCardSelect();  // slot +0x040
    m_dispatchers[17] = new TimerClearDungeonReward();  // slot +0x044
    m_dispatchers[18] = new TimerClearDungeonScoreScoll();  // slot +0x048
    m_dispatchers[19] = new TimerClearDungeonCardSelectRight();  // slot +0x04c
    m_dispatchers[4] = new TimerPremiumServiceOver();  // slot +0x010
    m_dispatchers[5] = new TimerPremiumServiceUpdate();  // slot +0x014
    m_dispatchers[6] = new TimerPremiumServiceStart();  // slot +0x018
    m_dispatchers[7] = new TimerCheckPaidPCRoomTerm();  // slot +0x01c
    m_dispatchers[20] = new TimerEPLPReturnVillage();  // slot +0x050
    m_dispatchers[21] = new TimerEPLPCommandWait();  // slot +0x054
    m_dispatchers[65] = new TimerWarRoomWaitCount();  // slot +0x104
    m_dispatchers[66] = new TimerWarRoomReadyLockCount();  // slot +0x108
    m_dispatchers[67] = new TimerWarRoomBattlePauseCount();  // slot +0x10c
    m_dispatchers[68] = new TimerWarRoomResultCount();  // slot +0x110
    m_dispatchers[69] = new TImerWarRoomTimeBomb();  // slot +0x114
    m_dispatchers[70] = new TimerWarRoomCheckFail();  // slot +0x118
    m_dispatchers[71] = new TimerWarRoomChangeHost();  // slot +0x11c
    m_dispatchers[22] = new TimerBloodSmallRoundRegister();  // slot +0x058
    m_dispatchers[23] = new TimerBloodPhaseRegister0();  // slot +0x05c
    m_dispatchers[24] = new TimerBloodPhaseRegister1();  // slot +0x060
    m_dispatchers[25] = new TimerBloodPhaseRegister2();  // slot +0x064
    m_dispatchers[26] = new TimerBloodPhaseRegister3();  // slot +0x068
    m_dispatchers[27] = new TimerBloodPhaseRegister4();  // slot +0x06c
    m_dispatchers[28] = new TimerBloodPhaseRegister5();  // slot +0x070
    m_dispatchers[29] = new TimerBloodPhaseRegister6();  // slot +0x074
    m_dispatchers[30] = new TimerBloodPhaseRegister7();  // slot +0x078
    m_dispatchers[31] = new TimerBloodPhaseRegister8();  // slot +0x07c
    m_dispatchers[32] = new TimerBloodPhaseRegister9();  // slot +0x080
    m_dispatchers[33] = new TimerBloodCrazyMonster();  // slot +0x084
    m_dispatchers[34] = new TimerBloodDungeonClearReward();  // slot +0x088
    m_dispatchers[35] = new TimerBloodDungeonClearReward();  // slot +0x08c
    m_dispatchers[36] = new TimerBloodDungeonClearReward();  // slot +0x090
    m_dispatchers[37] = new TimerBloodDungeonClearReward();  // slot +0x094
    m_dispatchers[38] = new TimerUltimateNextRoundStart();  // slot +0x098
    m_dispatchers[83] = new TimerEventChristmas();  // slot +0x14c
    m_dispatchers[84] = new TimerStatisticsDetailChannelInfo();  // slot +0x150
    m_dispatchers[85] = new TimerStatisticLevel();  // slot +0x154
    m_dispatchers[86] = new TimerScheduleOneHour();  // slot +0x158
    m_dispatchers[88] = new TimerSaveLogGamechannel();  // slot +0x160
    m_dispatchers[89] = new TimerQueryCounter();  // slot +0x164
    m_dispatchers[90] = new TimerNatType();  // slot +0x168
    m_dispatchers[91] = new TimerNatTypeUser();  // slot +0x16c
    m_dispatchers[8] = new TimerCheckMailTimeOut();  // slot +0x020
    m_dispatchers[92] = new TimerCreature();  // slot +0x170
    m_dispatchers[93] = new TimerForbidChat();  // slot +0x174
    m_dispatchers[94] = new TimerForbidMove();  // slot +0x178
    m_dispatchers[95] = new TimerScheduleFiveMinPerUser();  // slot +0x17c
    m_dispatchers[96] = new TimerScheduleTenMinPerUser();  // slot +0x180
    m_dispatchers[97] = new TimerQueueSizeLog();  // slot +0x184
    m_dispatchers[98] = new TimerScheduleGuildWarEnd();  // slot +0x188
    m_dispatchers[41] = new TimerSetPlayResult();  // slot +0x0a4
    m_dispatchers[99] = new TimerStartRelayBattle();  // slot +0x18c
    m_dispatchers[100] = new TimerBossDieTimeout();  // slot +0x190
    m_dispatchers[101] = new TimerEventCoinRefill();  // slot +0x194
    m_dispatchers[51] = new TimerPvPDieUserRevive0();  // slot +0x0cc
    m_dispatchers[52] = new TimerPvPDieUserRevive1();  // slot +0x0d0
    m_dispatchers[53] = new TimerPvPDieUserRevive2();  // slot +0x0d4
    m_dispatchers[54] = new TimerPvPDieUserRevive3();  // slot +0x0d8
    m_dispatchers[55] = new TimerPvPDieUserRevive4();  // slot +0x0dc
    m_dispatchers[56] = new TimerPvPDieUserRevive5();  // slot +0x0e0
    m_dispatchers[57] = new TimerPvPDieUserRevive6();  // slot +0x0e4
    m_dispatchers[58] = new TimerPvPDieUserRevive7();  // slot +0x0e8
    m_dispatchers[102] = new TimerDungeonDataStatistic();  // slot +0x198
    m_dispatchers[103] = new TimerNotResponseUserTimeout();  // slot +0x19c
    m_dispatchers[104] = new TimerLightServerNotice();  // slot +0x1a0
    m_dispatchers[105] = new TimerInsertUserPlayTime();  // slot +0x1a4
    m_dispatchers[106] = new TimerCheckForcedDisconnect();  // slot +0x1a8
    m_dispatchers[60] = new TimerCheckPvPRoomUser();  // slot +0x0f0
    m_dispatchers[61] = new TimerCheckRecvPvpEndResult();  // slot +0x0f4
    m_dispatchers[9] = new TimerCheckHumanCertifyTimeout();  // slot +0x024
    m_dispatchers[107] = new TimerUpdateItemGenerateStatistics();  // slot +0x1ac
    m_dispatchers[108] = new TimerUpdatePvPGrade();  // slot +0x1b0
    m_dispatchers[109] = new TimerDetectDisconnect();  // slot +0x1b4
    m_dispatchers[110] = new TimerCompleteLoadAssaultTimeout();  // slot +0x1b8
    m_dispatchers[111] = new TimerConnectP2PAssaultTimeout();  // slot +0x1bc
    m_dispatchers[112] = new TimerEndAssault();  // slot +0x1c0
    m_dispatchers[113] = new TimerCloseAssault();  // slot +0x1c4
    m_dispatchers[42] = new TimerCompleteLoadAfterAssaultTimeout();  // slot +0x0a8
    m_dispatchers[114] = new TimerConnectP2PAfterAssaultTimeout();  // slot +0x1c8
    m_dispatchers[73] = new TimerDeathTower();  // slot +0x124
    m_dispatchers[75] = new TimerDeathTower();  // slot +0x12c
    m_dispatchers[76] = new TimerDeathTower();  // slot +0x130
    m_dispatchers[77] = new TimerDeathTower();  // slot +0x134
    m_dispatchers[78] = new TimerDeathTower();  // slot +0x138
    m_dispatchers[74] = new TimerDeathTower();  // slot +0x128
    m_dispatchers[115] = new TimerDeathTowerTopRankerReload();  // slot +0x1cc
    m_dispatchers[87] = new TimerScheduleTenSecond();  // slot +0x15c
    m_dispatchers[118] = new TimerFinishItemEffect();  // slot +0x1d8
    m_dispatchers[121] = new TimerStartMatching();  // slot +0x1e4
    m_dispatchers[119] = new TimerGlobalEffectItemBegin();  // slot +0x1dc
    m_dispatchers[120] = new TimerGlobalEffectItemTimeout();  // slot +0x1e0
    m_dispatchers[10] = new TimerVendingMachineItemNotify();  // slot +0x028
    m_dispatchers[122] = new TimerConnectMonitorServer();  // slot +0x1e8
    m_dispatchers[126] = new TimerAssaultSuperPowerState();  // slot +0x1f8
    m_dispatchers[127] = new TimerAssaultSuperPowerState();  // slot +0x1fc
    m_dispatchers[116] = new TimerNPCMoodChange();  // slot +0x1d0
    m_dispatchers[40] = new TimerElevatorDungeon();  // slot +0x0a0
    m_dispatchers[123] = new TimerRefreshPowerWarProcessInfo();  // slot +0x1ec
    m_dispatchers[125] = new TimerGeneratePowerUpNotiTime();  // slot +0x1f4
    m_dispatchers[128] = new TimerCheckPowerWarAbuser();  // slot +0x200
    m_dispatchers[129] = new TimerSetPunishUser();  // slot +0x204
    m_dispatchers[151] = new TimerSetTradePunishUser();  // slot +0x25c
    m_dispatchers[130] = new TimerRevivalCharac();  // slot +0x208
    m_dispatchers[132] = new TimerRequestExit();  // slot +0x210
    m_dispatchers[133] = new TimerVillageMonsterUpdateRate();  // slot +0x214
    m_dispatchers[134] = new TimerVillageMonsterRegen();  // slot +0x218
    m_dispatchers[135] = new TimerStartVillageAttacked();  // slot +0x21c
    m_dispatchers[39] = new TimerVillageAttackedPlayTimeout();  // slot +0x09c
    m_dispatchers[136] = new TimerItemUnlockWaiting();  // slot +0x220
    m_dispatchers[137] = new TimerUpdateCompoundEmblemStatics();  // slot +0x224
    m_dispatchers[138] = new TimerStartExpEventAfterVillageAttack();  // slot +0x228
    m_dispatchers[140] = new Timer_Security_NonClient_CheckResponse();  // slot +0x230
    m_dispatchers[141] = new Timer_Midnight();  // slot +0x234
    m_dispatchers[139] = new TimerEndPowerWarVictoriousReward();  // slot +0x22c
    m_dispatchers[142] = new Timer_TenMinuteSchedule();  // slot +0x238
    m_dispatchers[143] = new Timer_UpdateOnlinePreliminaryTeam();  // slot +0x23c
    m_dispatchers[144] = new Timer_SecretShopStatistic();  // slot +0x240
    m_dispatchers[145] = new Timer_DailySchedule();  // slot +0x244
    m_dispatchers[59] = new Timer_ConnectP2PPvPTimeout();  // slot +0x0ec
    m_dispatchers[43] = new Timer_RoutingItemWait();  // slot +0x0ac
    m_dispatchers[146] = new Timer_PcRoomAuthRetry();  // slot +0x248
    m_dispatchers[147] = new Timer_DungeonInoutOpenTime();  // slot +0x24c
    m_dispatchers[148] = new Timer_DungeonInoutCloseTime();  // slot +0x250
    m_dispatchers[62] = new Timer_FairPvPRematch();  // slot +0x0f8
    m_dispatchers[149] = new Timer_ReturnUserTimeout();  // slot +0x254
    m_dispatchers[150] = new Timer_HadesHeartBeat();  // slot +0x258
    m_dispatchers[44] = new Timer_TOD_Send_ClearReward();  // slot +0x0b0
    m_dispatchers[153] = new Timer_OneADayItemShop();  // slot +0x264
    m_dispatchers[154] = new Timer_Schedule_MidNight();  // slot +0x268
    m_dispatchers[155] = new Timer_Send_Ontime_Reward();  // slot +0x26c
    m_dispatchers[156] = new Timer_EventManagement();  // slot +0x270
    m_dispatchers[158] = new Timer_QuickPartyWaitingPartyMatching();  // slot +0x278
    m_dispatchers[45] = new Timer_QuickPartyWaitingStartGame();  // slot +0x0b4
    m_dispatchers[160] = new Timer_AssaultGhostTimer();  // slot +0x280
    m_dispatchers[161] = new Timer_GenerateCaptchaData();  // slot +0x284
    m_dispatchers[46] = new Timer_TournamentDugeonClearStateNotiReward();  // slot +0x0b8
    m_dispatchers[47] = new Timer_TournametDungeonClearStateSelect();  // slot +0x0bc
    m_dispatchers[163] = new Timer_TowerOnTime();  // slot +0x28c
    m_dispatchers[164] = new Timer_TowerOnTime();  // slot +0x290
    m_dispatchers[165] = new Timer_TowerOnTime();  // slot +0x294
    m_dispatchers[162] = new Timer_NoticeEmergencyPatchMessage();  // slot +0x288
    m_dispatchers[166] = new advancealtar::Timer_MainTick();  // slot +0x298
    m_dispatchers[167] = new advancealtar::Timer_StageControl();  // slot +0x29c
    m_dispatchers[168] = new advancealtar::Timer_StageControl();  // slot +0x2a0
    m_dispatchers[169] = new advancealtar::Timer_StageControl();  // slot +0x2a4
    m_dispatchers[170] = new advancealtar::Timer_StageControl();  // slot +0x2a8
    m_dispatchers[171] = new Timer_DungeonStatisticEntrance_Hour();  // slot +0x2ac
    m_dispatchers[172] = new Timer_RefreshServerState();  // slot +0x2b0
    m_dispatchers[173] = new Timer_StayTimeEvent();  // slot +0x2b4
    m_dispatchers[174] = new Timer_CreateDnfDbLoad();  // slot +0x2b8
}

// ORIG 0x863298e：非空槽经虚表[2]（D0）删除后置 0。
TimerDispatcher::~TimerDispatcher()
{
    for (int i = 0; i <= 0xae; ++i)
    {
        if (m_dispatchers[i] != 0)
        {
            delete m_dispatchers[i];
        }
        m_dispatchers[i] = 0;
    }
}

// ORIG 0x86329f6：空槽返回 0。
TimerSig* TimerDispatcher::get_dispatcher(int msg)
{
    if (m_dispatchers[msg] == 0)
    {
        return 0;
    }
    return m_dispatchers[msg];
}

// ORIG 0x8632a18 逐指令转写：entry.msg 选槽 → dispatch_sig(param1,param2,param3)。
int TimerDispatcher::dispatch(TimerEntry& entry)
{
    TimerSig* d = get_dispatcher(entry.msg);
    if (d == 0)
    {
        return 0;
    }
    return d->dispatch_sig(entry.param1, entry.param2, entry.param3);
}

// ======== 具体分发器生命周期 + dispatch_sig stub ========
// stub 体待后续批次按 docs/class_func_reports 还原；地址=该函数 ORIG 起点。

TimerCheckConn::TimerCheckConn() {}
TimerCheckConn::~TimerCheckConn() {}
int TimerCheckConn::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p2, p1);
    if (user == 0 || (static_cast<unsigned int>(user->get_unique_id()) & 0xffffU) != p3)
    {
        return 0;
    }
    user->incre_check_count();
    // ORIG 无 SendCheckConnPacket 符号：内联构造空包头（put_header(0,0)+finalize）发送。
    PacketGuard packet;
    packet.put_header(0, 0);
    packet.finalize(true);
    user->Send(packet);
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, user->GetUID(),
                                (TIMER_MESSAGE)0, user->GetCheckConnInterval(),
                                user->get_unique_id(), 0);
    return 1;
}

TimerDBUpdate::TimerDBUpdate() {}
TimerDBUpdate::~TimerDBUpdate() {}
int TimerDBUpdate::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, p2);
    if (user == 0)
    {
        return 0;
    }
    // UpdateData/GetUID 为 CUser 成员（CUser 继承 CUserCharacInfo），保持 CUser*。
    const int elapsedHours =
        (GlobalData::s_systemTime_.getCurSec() - user->GetLoginTick()) / 0xe10;
    APSystem::_ActionGroupIndex group23 = { 0x23 };
    APSystem::_ActionGroupIndex group24 = { 0x24 };
    if (elapsedHours == 1)
    {
        APSystem::CUserProc::ClearActionAndSendtoUser(user, group23, 0,
                                                       ENUM_PACKETCLASS_0);
    }
    else if (elapsedHours > 0xb && elapsedHours < 0xd)
    {
        APSystem::CUserProc::ClearActionAndSendtoUser(user, group24, 0,
                                                       ENUM_PACKETCLASS_0);
    }
    if (user->GetLoginTick() != p2 ||
        (p3 != 0 && static_cast<unsigned int>(user->getCurCharacNo()) != p3))
    {
        return 0;
    }
    user->UpdateData();
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, user->GetUID(),
                                (TIMER_MESSAGE)1, get_rand_int(300) + 600,
                                user->GetLoginTick(),
                                static_cast<unsigned int>(user->getCurCharacNo()));
    return 1;
}

TimerStamina::TimerStamina() {}
TimerStamina::~TimerStamina() {}
int TimerStamina::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, p2);
    if (user == 0 || user->GetLoginTick() != p2 ||
        (p3 != 0 && static_cast<unsigned int>(user->get_charac_no(-1)) != p3))
    {
        return 0;
    }
    user->recover_stamina();
    user->IncCurCharacLuckPointUpdateTimeCount();
    const int updateCount = user->GetCurCharacLuckPointUpdateTimeCount();
    CDataManager* data = G_CDataManager();
    if (data->m_serverParameter.m_84 <= updateCount)
    {
        user->IncCurCharacLuckPoint(data->m_serverParameter.m_80);
        user->SetCurCharacLuckPointUpdateTimeCount(0);
    }
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, user->GetUID(),
                                (TIMER_MESSAGE)2, 0x3c, user->GetLoginTick(),
                                static_cast<unsigned int>(user->getCurCharacNo()));
    return 1;
}

int TimerPvpRankRes::dispatch_sig(int p1, int p2, unsigned int p3)
{
    PvP_Room* room = G_CGameManager()->GetPvp(p2, 0, 0x32);
    if (room == 0 || room->get_timer_key((TIMER_MESSAGE)0x32) != p3)
    {
        return 0;
    }
    room->pvp_reward();
    return 1;
}

TimerMapLoading::TimerMapLoading() {}
TimerMapLoading::~TimerMapLoading() {}
int TimerMapLoading::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty();
    if (party != 0 && party->check_timer_key((TIMER_MESSAGE)0xb, p3))
    {
        party->map_load_forced_finish();
        return 1;
    }
    return 0;
}

TimerSetResult::TimerSetResult() {}
TimerSetResult::~TimerSetResult() {}
int TimerSetResult::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty();
    if (party != 0 && party->check_timer_key((TIMER_MESSAGE)0xc, p3))
    {
        party->forced_set_play_result();
        return 1;
    }
    return 0;
}

TimerDungeonClear::TimerDungeonClear() {}
TimerDungeonClear::~TimerDungeonClear() {}
int TimerDungeonClear::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty();
    if (party != 0 && party->check_timer_key((TIMER_MESSAGE)0xd, p3))
    {
        party->dungeon_clear();
        return 1;
    }
    return 0;
}

TimerDungeonFail::TimerDungeonFail() {}
TimerDungeonFail::~TimerDungeonFail() {}
int TimerDungeonFail::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty();
    if (party != 0 && party->check_timer_key((TIMER_MESSAGE)0xe, p3))
    {
        party->party_dungeon_fail();
        return 1;
    }
    return 0;
}

TimerPartyWalkOutLackUser::TimerPartyWalkOutLackUser() {}
TimerPartyWalkOutLackUser::~TimerPartyWalkOutLackUser() {}
int TimerPartyWalkOutLackUser::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty();
    if (party != 0 && party->check_timer_key((TIMER_MESSAGE)0xf, p3))
    {
        // ORIG 透传 OnTimerWalkOutLackUser 的返回值；CParty.h 当前声明为 void，
        // 返回值缺口记入报告。此处调用后按 ORIG 主路径返回 1。
        party->OnTimerWalkOutLackUser();
        return 1;
    }
    return 0;
}

TimerPVPWalkOutLackUser::TimerPVPWalkOutLackUser() {}
TimerPVPWalkOutLackUser::~TimerPVPWalkOutLackUser() {}
int TimerPVPWalkOutLackUser::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CGameManager* manager = G_CGameManager();
    PvP_Room* room = manager->GetPvp(p2, 0, 0x31);
    if (room == 0 || room->get_timer_key((TIMER_MESSAGE)0x31) != p3)
    {
        return 0;
    }
    if (!room->OnTimerWalkOutLackUser())
    {
        room->reset();
        manager->PutPvp(room);
    }
    return 1;
}

TimerFatigueReset::TimerFatigueReset() {}
TimerFatigueReset::~TimerFatigueReset() {}
int TimerFatigueReset::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x8633750)：函数体未还原
}

TimerAvatarExpire::TimerAvatarExpire() {}
TimerAvatarExpire::~TimerAvatarExpire() {}
int TimerAvatarExpire::dispatch_sig(int p1, int p2, unsigned int p3)
{
    return 1;
}

TimerStatisticsLog::TimerStatisticsLog() {}
TimerStatisticsLog::~TimerStatisticsLog() {}
int TimerStatisticsLog::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x8633f1c)：函数体未还原
}

TimerCardSelect::TimerCardSelect() {}
TimerCardSelect::~TimerCardSelect() {}
int TimerCardSelect::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty();
    if (party == 0 || !party->check_timer_key((TIMER_MESSAGE)0x10, p3))
    {
        return 0;
    }
    const char state = party->get_dungeon_clear_state();
    if (state != 0 && state != 5)
    {
        if (!party->IsCompleteCardSelectOfMembers(eClearRewardCardType_t_0))
        {
            party->CompleteCardSelect();
            party->SendEachCardInfo();
        }
        party->set_dungeon_clear_state(5);
        party->dungeon_clear();
    }
    return 1;
}

TimerClearDungeonReward::TimerClearDungeonReward() {}
TimerClearDungeonReward::~TimerClearDungeonReward() {}
int TimerClearDungeonReward::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty();
    if (party == 0 || !party->check_timer_key((TIMER_MESSAGE)0x11, p3))
    {
        return 0;
    }
    if (party->get_dungeon_clear_state() == 2)
    {
        PacketGuard packet;
        packet.clear();
        packet.put_header(1, 0x48);
        packet.put_byte(1);
        packet.finalize(true);
        party->send_to_party(packet);
        party->set_dungeon_clear_state(3);
        party->insert_time_dungeon_score_scroll_msg(0x1e);
    }
    return 1;
}

TimerClearDungeonScoreScoll::TimerClearDungeonScoreScoll() {}
TimerClearDungeonScoreScoll::~TimerClearDungeonScoreScoll() {}
int TimerClearDungeonScoreScoll::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty();
    if (party == 0 || !party->check_timer_key((TIMER_MESSAGE)0x12, p3) ||
        party->get_dungeon_clear_state() != 3)
    {
        return 0;
    }
    PacketGuard packet;
    packet.put_header(1, 0x49);
    packet.put_byte(1);
    for (int i = 0; i < 4; ++i)
    {
        CUser* user = party->get_user(i);
        if (user == 0)
        {
            packet.put_short(-1);
        }
        else
        {
            packet.put_short(user->getCurCharacTotalFatigue() ==
                             user->getCurCharacTotalMaxFatigue() ? 0 : 1);
        }
    }
    packet.finalize(true);
    party->send_to_party(packet);
    party->set_dungeon_clear_state(4);
    party->insert_time_dungeon_score_scroll_msg(0x1e);
    party->insert_time_card_select_msg(4);
    return 1;
}

TimerClearDungeonCardSelectRight::TimerClearDungeonCardSelectRight() {}
TimerClearDungeonCardSelectRight::~TimerClearDungeonCardSelectRight() {}
int TimerClearDungeonCardSelectRight::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty();
    if (party == 0 || !party->check_timer_key((TIMER_MESSAGE)0x13, p3))
    {
        return 0;
    }
    if (party->get_dungeon_clear_state() == 4)
    {
        party->set_dungeon_clear_state(5);
        party->insert_time_dungeon_clear_msg(5);
    }
    return 1;
}

TimerPremiumServiceOver::TimerPremiumServiceOver() {}
TimerPremiumServiceOver::~TimerPremiumServiceOver() {}
int TimerPremiumServiceOver::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, p2);
    if (user == 0 || static_cast<unsigned int>(user->get_acc_id()) != p3 ||
        !user->IsPremiumUser())
    {
        return 0;
    }
    WongWork::CHandlePremium::handlePremiumTimeout(user);   // ORIG 0x863485a
    return 1;
}

TimerPremiumServiceUpdate::TimerPremiumServiceUpdate() {}
TimerPremiumServiceUpdate::~TimerPremiumServiceUpdate() {}
int TimerPremiumServiceUpdate::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, p2);
    if (user == 0 || static_cast<unsigned int>(user->get_acc_id()) != p3 ||
        !user->IsPremiumUser())
    {
        return 0;
    }
    WongWork::CUserPremium* premium = user->GetPremiumInfoW();
    premium->ReCalcAdvantage();
    premium->RecalcAdditionalInfo(user);
    if (user->get_state() > 3)
    {
        user->SendFatigue();
    }
    const time_t now = static_cast<time_t>(GlobalData::s_systemTime_.getCurSec());
    tm midnight = *localtime(&now);
    midnight.tm_hour = 0;
    midnight.tm_min = 0;
    midnight.tm_sec = 0;
    const int delay = static_cast<int>(mktime(&midnight) + 0x15180 - now);
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, p1, (TIMER_MESSAGE)5,
                                delay, p2, p3);
    return 1;
}

TimerPremiumServiceStart::TimerPremiumServiceStart() {}
TimerPremiumServiceStart::~TimerPremiumServiceStart() {}
int TimerPremiumServiceStart::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, p2);
    if (user == 0 || static_cast<unsigned int>(user->get_acc_id()) != p3)
    {
        return 0;
    }
    WongWork::CUserPremium* premium = user->GetPremiumInfoW();
    if (!premium->startPremium(p2, false))
    {
        return 0;
    }
    if (user->get_state() > 2)
    {
        WongWork::SUserPremiumInfo* info = premium->GetPremiumInfo(p2);
        if (info != 0 && info->m_type != 0)
        {
            WongWork::CHandlePremium::handleNotifyPremiumInfo(
                user, info->m_type, info->m_end - info->m_start);
            if (GlobalData::g_SPremiumInfo[info->m_type].m_kind != 2)
            {
                WongWork::CHandlePremium::handlePremiumEffect(
                    user, static_cast<ENUM_PREMIUM_TYPE>(info->m_type));
            }
        }
    }
    return 1;
}

TimerCheckPaidPCRoomTerm::TimerCheckPaidPCRoomTerm() {}
TimerCheckPaidPCRoomTerm::~TimerCheckPaidPCRoomTerm() {}
int TimerCheckPaidPCRoomTerm::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, p2);
    if (user == 0 || static_cast<unsigned int>(user->get_acc_id()) != p3)
    {
        return 0;
    }
    // ORIG isAffectedPremium(ENUM_PREMIUM_TYPE) const 单参数（_ZNK5CUser17isAffectedPremiumE17ENUM_PREMIUM_TYPE）。
    if (user->isAffectedPremium(static_cast<ENUM_PREMIUM_TYPE>(p2)))
    {
        user->SendLogoutToPCRoomServer();
        user->SendLoginToPCRoomServer();
        return 1;
    }
    WongWork::CHandlePremium::handleNotifyPremiumInfo(user, p2, 0);
    return 0;
}

TimerEPLPReturnVillage::TimerEPLPReturnVillage() {}
TimerEPLPReturnVillage::~TimerEPLPReturnVillage() {}
int TimerEPLPReturnVillage::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty();
    if (party != 0 && party->check_timer_key((TIMER_MESSAGE)0x14, p3))
    {
        party->ReturnToVillage();
        return 1;
    }
    return 0;
}

TimerEPLPCommandWait::TimerEPLPCommandWait() {}
TimerEPLPCommandWait::~TimerEPLPCommandWait() {}
int TimerEPLPCommandWait::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty();
    if (party == 0 || !party->check_timer_key((TIMER_MESSAGE)0x15, p3))
    {
        return 0;
    }
    if (party->GetSelectedEPLPCmd() == 3)
    {
        return 0;
    }
    if (party->GetEPLPState() == 2)
    {
        party->ReturnToVillage();
    }
    return 1;
}

TimerWarRoomWaitCount::TimerWarRoomWaitCount() {}
TimerWarRoomWaitCount::~TimerWarRoomWaitCount() {}
int TimerWarRoomWaitCount::dispatch_sig(int p1, int p2, unsigned int p3)
{
    WarRoom* room = G_CGameManager()->GetWarRoom();
    if (room != 0 && room->CheckTimerKey((TIMER_MESSAGE)0x41, p3))
    {
        room->ChangeState(WARROOM_STATE_2);
        return 1;
    }
    return 0;
}

TimerWarRoomReadyLockCount::TimerWarRoomReadyLockCount() {}
TimerWarRoomReadyLockCount::~TimerWarRoomReadyLockCount() {}
int TimerWarRoomReadyLockCount::dispatch_sig(int p1, int p2, unsigned int p3)
{
    WarRoom* room = G_CGameManager()->GetWarRoom();
    if (room != 0 && room->CheckTimerKey((TIMER_MESSAGE)0x42, p3))
    {
        room->ChangeState(WARROOM_STATE_3);
        return 1;
    }
    return 0;
}

TimerWarRoomBattlePauseCount::TimerWarRoomBattlePauseCount() {}
TimerWarRoomBattlePauseCount::~TimerWarRoomBattlePauseCount() {}
int TimerWarRoomBattlePauseCount::dispatch_sig(int p1, int p2, unsigned int p3)
{
    WarRoom* room = G_CGameManager()->GetWarRoom();
    if (room != 0 && room->CheckTimerKey((TIMER_MESSAGE)0x43, p3))
    {
        room->ChangeState(WARROOM_STATE_3);
        return 1;
    }
    return 0;
}

TimerWarRoomResultCount::TimerWarRoomResultCount() {}
TimerWarRoomResultCount::~TimerWarRoomResultCount() {}
int TimerWarRoomResultCount::dispatch_sig(int p1, int p2, unsigned int p3)
{
    WarRoom* room = G_CGameManager()->GetWarRoom();
    if (room != 0 && room->CheckTimerKey((TIMER_MESSAGE)0x44, p3))
    {
        room->HandleTimerResultCount();
        return 1;
    }
    return 0;
}

TImerWarRoomTimeBomb::TImerWarRoomTimeBomb() {}
TImerWarRoomTimeBomb::~TImerWarRoomTimeBomb() {}
int TImerWarRoomTimeBomb::dispatch_sig(int p1, int p2, unsigned int p3)
{
    WarRoom* room = G_CGameManager()->GetWarRoom();
    if (room != 0 && room->CheckTimerKey((TIMER_MESSAGE)0x45, p3))
    {
        room->HandleTimerTimeBomb();
        return 1;
    }
    return 0;
}

TimerWarRoomCheckFail::TimerWarRoomCheckFail() {}
TimerWarRoomCheckFail::~TimerWarRoomCheckFail() {}
int TimerWarRoomCheckFail::dispatch_sig(int p1, int p2, unsigned int p3)
{
    WarRoom* room = G_CGameManager()->GetWarRoom();
    if (room != 0 && room->CheckTimerKey((TIMER_MESSAGE)0x46, p3))
    {
        room->WarRoomFail();
        return 1;
    }
    return 0;
}

TimerWarRoomChangeHost::TimerWarRoomChangeHost() {}
TimerWarRoomChangeHost::~TimerWarRoomChangeHost() {}
int TimerWarRoomChangeHost::dispatch_sig(int p1, int p2, unsigned int p3)
{
    WarRoom* room = G_CGameManager()->GetWarRoom();
    if (room != 0 && room->CheckTimerKey((TIMER_MESSAGE)0x47, p3))
    {
        room->CheckHostChange();
        return 1;
    }
    return 0;
}

TimerBloodSmallRoundRegister::TimerBloodSmallRoundRegister() {}
TimerBloodSmallRoundRegister::~TimerBloodSmallRoundRegister() {}
int TimerBloodSmallRoundRegister::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty();
    if (party == 0 || !party->check_timer_key((TIMER_MESSAGE)0x16, p3))
    {
        return 0;
    }
    reinterpret_cast<CBattle_Field*>(reinterpret_cast<char*>(party) + 0xb24)
        ->BloodSpawnTimerManager(false, true);   // ORIG：party+0xb24 即 CBattle_Field 子对象
    return 1;
}

TimerBloodPhaseRegister0::TimerBloodPhaseRegister0() {}
TimerBloodPhaseRegister0::~TimerBloodPhaseRegister0() {}
int TimerBloodPhaseRegister0::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty();
    if (party == 0 || !party->check_timer_key((TIMER_MESSAGE)0x17, p3))
    {
        return 0;
    }
    CBattle_Field* field = reinterpret_cast<CBattle_Field*>(reinterpret_cast<char*>(party) + 0xb24);   // ORIG：party+0xb24 子对象
    short nextRound = static_cast<short>(field->m_bloodSpawnTime[0]);
    field->BloodPhaseSpawn(static_cast<short>(field->m_bloodSpawnCnt[0]),
                           nextRound, 0x17, 0);
    return 1;
}

TimerBloodPhaseRegister1::TimerBloodPhaseRegister1() {}
TimerBloodPhaseRegister1::~TimerBloodPhaseRegister1() {}
int TimerBloodPhaseRegister1::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty();
    if (party == 0 || !party->check_timer_key((TIMER_MESSAGE)0x18, p3))
    {
        return 0;
    }
    CBattle_Field* field = reinterpret_cast<CBattle_Field*>(reinterpret_cast<char*>(party) + 0xb24);   // ORIG：party+0xb24 子对象
    short nextRound = static_cast<short>(field->m_bloodSpawnTime[1]);
    field->BloodPhaseSpawn(static_cast<short>(field->m_bloodSpawnCnt[1]),
                           nextRound, 0x18, 1);
    return 1;
}

TimerBloodPhaseRegister2::TimerBloodPhaseRegister2() {}
TimerBloodPhaseRegister2::~TimerBloodPhaseRegister2() {}
int TimerBloodPhaseRegister2::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty();
    if (party == 0 || !party->check_timer_key((TIMER_MESSAGE)0x19, p3))
    {
        return 0;
    }
    CBattle_Field* field = reinterpret_cast<CBattle_Field*>(reinterpret_cast<char*>(party) + 0xb24);   // ORIG：party+0xb24 子对象
    short nextRound = static_cast<short>(field->m_bloodSpawnTime[2]);
    field->BloodPhaseSpawn(static_cast<short>(field->m_bloodSpawnCnt[2]),
                           nextRound, 0x19, 2);
    return 1;
}

TimerBloodPhaseRegister3::TimerBloodPhaseRegister3() {}
TimerBloodPhaseRegister3::~TimerBloodPhaseRegister3() {}
int TimerBloodPhaseRegister3::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty();
    if (party == 0 || !party->check_timer_key((TIMER_MESSAGE)0x1a, p3))
    {
        return 0;
    }
    CBattle_Field* field = reinterpret_cast<CBattle_Field*>(reinterpret_cast<char*>(party) + 0xb24);   // ORIG：party+0xb24 子对象
    short nextRound = static_cast<short>(field->m_bloodSpawnTime[3]);
    field->BloodPhaseSpawn(static_cast<short>(field->m_bloodSpawnCnt[3]),
                           nextRound, 0x1a, 3);
    return 1;
}

TimerBloodPhaseRegister4::TimerBloodPhaseRegister4() {}
TimerBloodPhaseRegister4::~TimerBloodPhaseRegister4() {}
int TimerBloodPhaseRegister4::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty();
    if (party == 0 || !party->check_timer_key((TIMER_MESSAGE)0x1b, p3))
    {
        return 0;
    }
    CBattle_Field* field = reinterpret_cast<CBattle_Field*>(reinterpret_cast<char*>(party) + 0xb24);   // ORIG：party+0xb24 子对象
    short nextRound = static_cast<short>(field->m_bloodSpawnTime[4]);
    field->BloodPhaseSpawn(static_cast<short>(field->m_bloodSpawnCnt[4]),
                           nextRound, 0x1b, 4);
    return 1;
}

TimerBloodPhaseRegister5::TimerBloodPhaseRegister5() {}
TimerBloodPhaseRegister5::~TimerBloodPhaseRegister5() {}
int TimerBloodPhaseRegister5::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty();
    if (party == 0 || !party->check_timer_key((TIMER_MESSAGE)0x1c, p3))
    {
        return 0;
    }
    CBattle_Field* field = reinterpret_cast<CBattle_Field*>(reinterpret_cast<char*>(party) + 0xb24);   // ORIG：party+0xb24 子对象
    short nextRound = static_cast<short>(field->m_bloodSpawnTime[5]);
    field->BloodPhaseSpawn(static_cast<short>(field->m_bloodSpawnCnt[5]),
                           nextRound, 0x1c, 5);
    return 1;
}

TimerBloodPhaseRegister6::TimerBloodPhaseRegister6() {}
TimerBloodPhaseRegister6::~TimerBloodPhaseRegister6() {}
int TimerBloodPhaseRegister6::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty();
    if (party == 0 || !party->check_timer_key((TIMER_MESSAGE)0x1d, p3))
    {
        return 0;
    }
    CBattle_Field* field = reinterpret_cast<CBattle_Field*>(reinterpret_cast<char*>(party) + 0xb24);   // ORIG：party+0xb24 子对象
    short nextRound = static_cast<short>(field->m_bloodSpawnTime[6]);
    field->BloodPhaseSpawn(static_cast<short>(field->m_bloodSpawnCnt[6]),
                           nextRound, 0x1d, 6);
    return 1;
}

TimerBloodPhaseRegister7::TimerBloodPhaseRegister7() {}
TimerBloodPhaseRegister7::~TimerBloodPhaseRegister7() {}
int TimerBloodPhaseRegister7::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty();
    if (party == 0 || !party->check_timer_key((TIMER_MESSAGE)0x1e, p3))
    {
        return 0;
    }
    CBattle_Field* field = reinterpret_cast<CBattle_Field*>(reinterpret_cast<char*>(party) + 0xb24);   // ORIG：party+0xb24 子对象
    short nextRound = static_cast<short>(field->m_bloodSpawnTime[7]);
    field->BloodPhaseSpawn(static_cast<short>(field->m_bloodSpawnCnt[7]),
                           nextRound, 0x1e, 7);
    return 1;
}

TimerBloodPhaseRegister8::TimerBloodPhaseRegister8() {}
TimerBloodPhaseRegister8::~TimerBloodPhaseRegister8() {}
int TimerBloodPhaseRegister8::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty();
    if (party == 0 || !party->check_timer_key((TIMER_MESSAGE)0x1f, p3))
    {
        return 0;
    }
    CBattle_Field* field = reinterpret_cast<CBattle_Field*>(reinterpret_cast<char*>(party) + 0xb24);   // ORIG：party+0xb24 子对象
    short nextRound = static_cast<short>(field->m_bloodSpawnTime[8]);
    field->BloodPhaseSpawn(static_cast<short>(field->m_bloodSpawnCnt[8]),
                           nextRound, 0x1f, 8);
    return 1;
}

TimerBloodPhaseRegister9::TimerBloodPhaseRegister9() {}
TimerBloodPhaseRegister9::~TimerBloodPhaseRegister9() {}
int TimerBloodPhaseRegister9::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty();
    if (party == 0 || !party->check_timer_key((TIMER_MESSAGE)0x20, p3))
    {
        return 0;
    }
    CBattle_Field* field = reinterpret_cast<CBattle_Field*>(reinterpret_cast<char*>(party) + 0xb24);   // ORIG：party+0xb24 子对象
    short nextRound = static_cast<short>(field->m_bloodSpawnTime[9]);
    field->BloodPhaseSpawn(static_cast<short>(field->m_bloodSpawnCnt[9]),
                           nextRound, 0x20, 9);
    return 1;
}

TimerBloodCrazyMonster::TimerBloodCrazyMonster() {}
TimerBloodCrazyMonster::~TimerBloodCrazyMonster() {}
int TimerBloodCrazyMonster::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty();
    if (party == 0 || !party->check_timer_key((TIMER_MESSAGE)0x21, p3))
    {
        return 0;
    }
    reinterpret_cast<CBattle_Field*>(reinterpret_cast<char*>(party) + 0xb24)
        ->BloodCrazyMonsterSpawnPacket();   // ORIG：party+0xb24 子对象
    return 1;
}

TimerBloodDungeonClearReward::TimerBloodDungeonClearReward() {}
TimerBloodDungeonClearReward::~TimerBloodDungeonClearReward() {}
int TimerBloodDungeonClearReward::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty();
    if (party == 0 || !party->check_timer_key((TIMER_MESSAGE)p3, p2))
    {
        return 0;
    }
    return party->onBloodRewardTimer((TIMER_MESSAGE)p3) ? 1 : 0;
}

TimerUltimateNextRoundStart::TimerUltimateNextRoundStart() {}
TimerUltimateNextRoundStart::~TimerUltimateNextRoundStart() {}
int TimerUltimateNextRoundStart::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty();
    if (party == 0 || !party->check_timer_key((TIMER_MESSAGE)0x26, p3))
    {
        return 0;
    }
    CBattle_Field* field = reinterpret_cast<CBattle_Field*>(reinterpret_cast<char*>(party) + 0xb24);   // ORIG：party+0xb24 子对象
    const char difficulty = field->getUltimateDifficylty();
    field->sendUltimateRandomDifficulty(p2, static_cast<int>(difficulty));
    field->setUltimateDifficulty(difficulty);
    field->startUltimateRound();
    return 1;
}

TimerEventChristmas::TimerEventChristmas() {}
TimerEventChristmas::~TimerEventChristmas() {}
int TimerEventChristmas::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 三方法为静态语义（调用点无 this 压栈，直接操作全局 0x943c138），
    // CServerEvent.h 当前声明为非静态成员（static 标注缺口记报告）；
    // 经临时对象调用，非虚成员 ABI 不受影响。
    CServerEvent().SubExpRate(p2);
    CServerEvent().TurnOffEvent(p3);
    const short interval = static_cast<short>(CServerEvent().GetEventInterval(p3));
    Inter_MonitorServerEventStart::SendEventNotiPacket(false,
                                                        static_cast<short>(p3),
                                                        interval, 0);
    return 1;
}

TimerStatisticsDetailChannelInfo::TimerStatisticsDetailChannelInfo() {}
TimerStatisticsDetailChannelInfo::~TimerStatisticsDetailChannelInfo() {}
int TimerStatisticsDetailChannelInfo::dispatch_sig(int p1, int p2, unsigned int p3)
{
    G_GameWorld()->updateDetailChannelServiceInfo();
    RegistNextTimer();
    return 1;
}

TimerStatisticLevel::TimerStatisticLevel() {}
TimerStatisticLevel::~TimerStatisticLevel() {}
int TimerStatisticLevel::dispatch_sig(int p1, int p2, unsigned int p3)
{
    G_GameWorld()->UpdateStatistic4ChannelUserLev();
    // ORIG：InsertTimer(OBJ_TYPE_2, 0, 0x55, CalcNextTime()-getCurSec(), 0, 0)。
    const int delay = CalcNextTime() - GlobalData::s_systemTime_.getCurSec();
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x55, delay, 0, 0);
    return 1;
}

TimerScheduleOneHour::TimerScheduleOneHour() {}
TimerScheduleOneHour::~TimerScheduleOneHour() {}
int TimerScheduleOneHour::dispatch_sig(int p1, int p2, unsigned int p3)
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x56, 0xe10, 0, 0);
    return 1;
}

TimerSaveLogGamechannel::TimerSaveLogGamechannel() {}
TimerSaveLogGamechannel::~TimerSaveLogGamechannel() {}
int TimerSaveLogGamechannel::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // CLogGameChannel 三方法为无参实例调用（ORIG 0x08550456/0x085506c6/0x085503fe）。
    GlobalData::s_pLogGameChannel->ReqSaveLog();
    GlobalData::s_pLogGameChannel->ResetInOutUser();
    GlobalData::s_pLogGameChannel->SetNextLogTimer();
    return 1;
}

TimerQueryCounter::TimerQueryCounter() {}
TimerQueryCounter::~TimerQueryCounter() {}
int TimerQueryCounter::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x8635a60)：函数体未还原
}

TimerNatType::TimerNatType() {}
TimerNatType::~TimerNatType() {}
int TimerNatType::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG：UpdateNatType 为静态语义（Ghidra 无实例实参），GetStatisticsNatType 返回值未用。
    StatisticsNatType::UpdateNatType();
    RegistNextTimer();
    return 1;
}

TimerNatTypeUser::TimerNatTypeUser() {}
TimerNatTypeUser::~TimerNatTypeUser() {}
int TimerNatTypeUser::dispatch_sig(int p1, int p2, unsigned int p3)
{
    G_GameWorld()->UpdateNatTypeUser();
    RegistNextTimer();
    return 1;
}

TimerCheckMailTimeOut::TimerCheckMailTimeOut() {}
TimerCheckMailTimeOut::~TimerCheckMailTimeOut() {}
int TimerCheckMailTimeOut::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, p2);
    if (user == 0 || user->get_state() < 2 || user->GetLoginTick() != p2 ||
        static_cast<unsigned int>(user->getCurCharacNo()) != p3)
    {
        return 0;
    }
    PacketGuard packet;
    packet.put_header(0, 0x62);
    int countIndex = packet.get_index();
    packet.put_int(0);
    const int expired = static_cast<int>(WongWork::CMailBoxHelper::CheckMailTimeout(user, &packet));
    if (expired != 0)
    {
        packet.put_int(countIndex, expired);
        packet.finalize(true);
        user->Send(packet);
    }
    WongWork::CMailBoxHelper::SetPeriodCheckTimer(user);
    return 1;
}

TimerCreature::TimerCreature() {}
TimerCreature::~TimerCreature() {}
int TimerCreature::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, p2);
    if (user == 0 || (static_cast<unsigned int>(user->get_unique_id()) & 0xffffU) !=
                      static_cast<unsigned int>(p2) ||
        static_cast<unsigned int>(user->getCurCharacNo()) != p3)
    {
        return 0;
    }
    // CheckDiedCreature 是 user_creature::CCreatureMgr 成员（ORIG 0x08339acc）：
    // user->getCurCharacInvenW()->GetCreatureMgrW() 取管理器实例。
    const char saveInventory = user->isSaveInven();
    user->getCurCharacInvenW()->GetCreatureMgrW()->CheckDiedCreature();
    if (!saveInventory)
    {
        user->disableSaveInven();
    }
    return 1;
}

TimerForbidChat::TimerForbidChat() {}
TimerForbidChat::~TimerForbidChat() {}
int TimerForbidChat::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, p2);
    if (user == 0)
    {
        return 0;
    }
    G_GameWorld()->EnableChat(user->get_charac_name_by_charac_no(p3));
    return 1;
}

TimerForbidMove::TimerForbidMove() {}
TimerForbidMove::~TimerForbidMove() {}
int TimerForbidMove::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, p2);
    if (user == 0)
    {
        return 0;
    }
    G_GameWorld()->EnableMove(user->get_charac_name_by_charac_no(p3));
    return 1;
}

TimerScheduleFiveMinPerUser::TimerScheduleFiveMinPerUser() {}
TimerScheduleFiveMinPerUser::~TimerScheduleFiveMinPerUser() {}
int TimerScheduleFiveMinPerUser::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, p2);
    if (user == 0 || user->GetLoginTick() != p2 ||
        (p3 != 0 && static_cast<unsigned int>(user->getCurCharacNo()) != p3))
    {
        return 0;
    }
    user->WorkPerFiveMin();
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, user->GetUID(),
                                (TIMER_MESSAGE)0x5f, 300,
                                user->GetLoginTick(),
                                static_cast<unsigned int>(user->getCurCharacNo()));
    return 1;
}

TimerScheduleTenMinPerUser::TimerScheduleTenMinPerUser() {}
TimerScheduleTenMinPerUser::~TimerScheduleTenMinPerUser() {}
int TimerScheduleTenMinPerUser::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, p2);
    if (user == 0 || user->GetLoginTick() != p2 ||
        (p3 != 0 && static_cast<unsigned int>(user->getCurCharacNo()) != p3))
    {
        return 0;
    }
    user->WorkPerTenMin();
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, user->GetUID(),
                                (TIMER_MESSAGE)0x60, 600,
                                user->GetLoginTick(),
                                static_cast<unsigned int>(user->getCurCharacNo()));
    return 1;
}

TimerQueueSizeLog::TimerQueueSizeLog() {}
TimerQueueSizeLog::~TimerQueueSizeLog() {}
int TimerQueueSizeLog::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // remain/put 为 MsgQueueMgr 成员（ORIG this=s_msgq_mgr）；2=DB_Q、4=LOGDB_Q。
    const int remaining = static_cast<int>(GlobalData::s_msgq_mgr->remain(MsgQueueMgr::DB_Q));
    if (remaining > 10)
    {
        Stream* stream = GlobalData::s_stream_pool->Acquire("TimerDispatcher.cpp",
                                                             0xa7f);
        CStreamGuard guard(stream, true);
        guard << 0x9b << -1 << remaining << G_CEnvironment()->get_channel_no();
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    }
    RegistNextTimer();
    return 1;
}

TimerScheduleGuildWarEnd::TimerScheduleGuildWarEnd() {}
TimerScheduleGuildWarEnd::~TimerScheduleGuildWarEnd() {}
int TimerScheduleGuildWarEnd::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x86331d0)：函数体未还原
}

TimerSetPlayResult::TimerSetPlayResult() {}
TimerSetPlayResult::~TimerSetPlayResult() {}
int TimerSetPlayResult::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty();
    if (party != 0 && party->check_timer_key((TIMER_MESSAGE)0x29, p3))
    {
        party->SetPlayResult(party->getManager());
        return 1;
    }
    return 0;
}

TimerStartRelayBattle::TimerStartRelayBattle() {}
TimerStartRelayBattle::~TimerStartRelayBattle() {}
int TimerStartRelayBattle::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x863640c)：函数体未还原
}
int TimerBossDieTimeout::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty();
    if (party == 0)
    {
        return 0;
    }
    // ORIG 透传 OnTimeoutBossDie 返回值；CParty.h 声明为 void（缺口记报告），
    // 此处调用后按 ORIG 主路径返回 1。
    party->OnTimeoutBossDie(p2, p3);
    return 1;
}

TimerEventCoinRefill::TimerEventCoinRefill() {}
TimerEventCoinRefill::~TimerEventCoinRefill() {}
int TimerEventCoinRefill::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x86364c4)：函数体未还原
}

TimerPvPDieUserRevive0::TimerPvPDieUserRevive0() {}
TimerPvPDieUserRevive0::~TimerPvPDieUserRevive0() {}
int TimerPvPDieUserRevive0::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x86365da)：函数体未还原
}

TimerPvPDieUserRevive1::TimerPvPDieUserRevive1() {}
TimerPvPDieUserRevive1::~TimerPvPDieUserRevive1() {}
int TimerPvPDieUserRevive1::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x8636604)：函数体未还原
}

TimerPvPDieUserRevive2::TimerPvPDieUserRevive2() {}
TimerPvPDieUserRevive2::~TimerPvPDieUserRevive2() {}
int TimerPvPDieUserRevive2::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x863662e)：函数体未还原
}

TimerPvPDieUserRevive3::TimerPvPDieUserRevive3() {}
TimerPvPDieUserRevive3::~TimerPvPDieUserRevive3() {}
int TimerPvPDieUserRevive3::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x8636658)：函数体未还原
}

TimerPvPDieUserRevive4::TimerPvPDieUserRevive4() {}
TimerPvPDieUserRevive4::~TimerPvPDieUserRevive4() {}
int TimerPvPDieUserRevive4::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x8636682)：函数体未还原
}

TimerPvPDieUserRevive5::TimerPvPDieUserRevive5() {}
TimerPvPDieUserRevive5::~TimerPvPDieUserRevive5() {}
int TimerPvPDieUserRevive5::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x86366ac)：函数体未还原
}

TimerPvPDieUserRevive6::TimerPvPDieUserRevive6() {}
TimerPvPDieUserRevive6::~TimerPvPDieUserRevive6() {}
int TimerPvPDieUserRevive6::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x86366d6)：函数体未还原
}

TimerPvPDieUserRevive7::TimerPvPDieUserRevive7() {}
TimerPvPDieUserRevive7::~TimerPvPDieUserRevive7() {}
int TimerPvPDieUserRevive7::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x8636700)：函数体未还原
}

TimerDungeonDataStatistic::TimerDungeonDataStatistic() {}
TimerDungeonDataStatistic::~TimerDungeonDataStatistic() {}
int TimerDungeonDataStatistic::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x863672a)：函数体未还原
}

TimerNotResponseUserTimeout::TimerNotResponseUserTimeout() {}
TimerNotResponseUserTimeout::~TimerNotResponseUserTimeout() {}
int TimerNotResponseUserTimeout::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x863695a)：函数体未还原
}

TimerLightServerNotice::TimerLightServerNotice() {}
TimerLightServerNotice::~TimerLightServerNotice() {}
int TimerLightServerNotice::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x8636a7c)：函数体未还原
}

TimerInsertUserPlayTime::TimerInsertUserPlayTime() {}
TimerInsertUserPlayTime::~TimerInsertUserPlayTime() {}
int TimerInsertUserPlayTime::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x8636d0a)：函数体未还原
}

TimerCheckForcedDisconnect::TimerCheckForcedDisconnect() {}
TimerCheckForcedDisconnect::~TimerCheckForcedDisconnect() {}
int TimerCheckForcedDisconnect::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x8636e06)：函数体未还原
}

TimerCheckPvPRoomUser::TimerCheckPvPRoomUser() {}
TimerCheckPvPRoomUser::~TimerCheckPvPRoomUser() {}
int TimerCheckPvPRoomUser::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x8636f90)：函数体未还原
}

TimerCheckRecvPvpEndResult::TimerCheckRecvPvpEndResult() {}
TimerCheckRecvPvpEndResult::~TimerCheckRecvPvpEndResult() {}
int TimerCheckRecvPvpEndResult::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x863703a)：函数体未还原
}

TimerCheckHumanCertifyTimeout::TimerCheckHumanCertifyTimeout() {}
TimerCheckHumanCertifyTimeout::~TimerCheckHumanCertifyTimeout() {}
int TimerCheckHumanCertifyTimeout::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x863713a)：函数体未还原
}

TimerUpdateItemGenerateStatistics::TimerUpdateItemGenerateStatistics() {}
TimerUpdateItemGenerateStatistics::~TimerUpdateItemGenerateStatistics() {}
int TimerUpdateItemGenerateStatistics::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x86372d4)：函数体未还原
}

TimerUpdatePvPGrade::TimerUpdatePvPGrade() {}
TimerUpdatePvPGrade::~TimerUpdatePvPGrade() {}
int TimerUpdatePvPGrade::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x86373c6)：函数体未还原
}

TimerDetectDisconnect::TimerDetectDisconnect() {}
TimerDetectDisconnect::~TimerDetectDisconnect() {}
int TimerDetectDisconnect::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x86374bc)：函数体未还原
}

TimerCompleteLoadAssaultTimeout::TimerCompleteLoadAssaultTimeout() {}
TimerCompleteLoadAssaultTimeout::~TimerCompleteLoadAssaultTimeout() {}
int TimerCompleteLoadAssaultTimeout::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x863752c)：函数体未还原
}

TimerConnectP2PAssaultTimeout::TimerConnectP2PAssaultTimeout() {}
TimerConnectP2PAssaultTimeout::~TimerConnectP2PAssaultTimeout() {}
int TimerConnectP2PAssaultTimeout::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x863759c)：函数体未还原
}

TimerEndAssault::TimerEndAssault() {}
TimerEndAssault::~TimerEndAssault() {}
int TimerEndAssault::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x863760c)：函数体未还原
}

TimerCloseAssault::TimerCloseAssault() {}
TimerCloseAssault::~TimerCloseAssault() {}
int TimerCloseAssault::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x863767c)：函数体未还原
}

TimerCompleteLoadAfterAssaultTimeout::TimerCompleteLoadAfterAssaultTimeout() {}
TimerCompleteLoadAfterAssaultTimeout::~TimerCompleteLoadAfterAssaultTimeout() {}
int TimerCompleteLoadAfterAssaultTimeout::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x86376ec)：函数体未还原
}

TimerConnectP2PAfterAssaultTimeout::TimerConnectP2PAfterAssaultTimeout() {}
TimerConnectP2PAfterAssaultTimeout::~TimerConnectP2PAfterAssaultTimeout() {}
int TimerConnectP2PAfterAssaultTimeout::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x863775c)：函数体未还原
}

TimerDeathTower::TimerDeathTower() {}
TimerDeathTower::~TimerDeathTower() {}
int TimerDeathTower::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x86377cc)：函数体未还原
}

TimerDeathTowerTopRankerReload::TimerDeathTowerTopRankerReload() {}
TimerDeathTowerTopRankerReload::~TimerDeathTowerTopRankerReload() {}
int TimerDeathTowerTopRankerReload::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x8637816)：函数体未还原
}

TimerScheduleTenSecond::TimerScheduleTenSecond() {}
TimerScheduleTenSecond::~TimerScheduleTenSecond() {}
int TimerScheduleTenSecond::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x8637936)：函数体未还原
}

TimerFinishItemEffect::TimerFinishItemEffect() {}
TimerFinishItemEffect::~TimerFinishItemEffect() {}
int TimerFinishItemEffect::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x8637aac)：函数体未还原
}

TimerStartMatching::TimerStartMatching() {}
TimerStartMatching::~TimerStartMatching() {}
int TimerStartMatching::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x8637d92)：函数体未还原
}

TimerGlobalEffectItemBegin::TimerGlobalEffectItemBegin() {}
TimerGlobalEffectItemBegin::~TimerGlobalEffectItemBegin() {}
int TimerGlobalEffectItemBegin::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x8637e56)：函数体未还原
}

TimerGlobalEffectItemTimeout::TimerGlobalEffectItemTimeout() {}
TimerGlobalEffectItemTimeout::~TimerGlobalEffectItemTimeout() {}
int TimerGlobalEffectItemTimeout::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x8637ec2)：函数体未还原
}

TimerVendingMachineItemNotify::TimerVendingMachineItemNotify() {}
TimerVendingMachineItemNotify::~TimerVendingMachineItemNotify() {}
int TimerVendingMachineItemNotify::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x8637f40)：函数体未还原
}

TimerConnectMonitorServer::TimerConnectMonitorServer() {}
TimerConnectMonitorServer::~TimerConnectMonitorServer() {}
int TimerConnectMonitorServer::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x86380bc)：函数体未还原
}

TimerAssaultSuperPowerState::TimerAssaultSuperPowerState() {}
TimerAssaultSuperPowerState::~TimerAssaultSuperPowerState() {}
int TimerAssaultSuperPowerState::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x863414e)：函数体未还原
}

TimerNPCMoodChange::TimerNPCMoodChange() {}
TimerNPCMoodChange::~TimerNPCMoodChange() {}
int TimerNPCMoodChange::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x86381e2)：函数体未还原
}

TimerElevatorDungeon::TimerElevatorDungeon() {}
TimerElevatorDungeon::~TimerElevatorDungeon() {}
int TimerElevatorDungeon::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x86381f2)：函数体未还原
}

TimerRefreshPowerWarProcessInfo::TimerRefreshPowerWarProcessInfo() {}
TimerRefreshPowerWarProcessInfo::~TimerRefreshPowerWarProcessInfo() {}
int TimerRefreshPowerWarProcessInfo::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x86382d8)：函数体未还原
}

TimerGeneratePowerUpNotiTime::TimerGeneratePowerUpNotiTime() {}
TimerGeneratePowerUpNotiTime::~TimerGeneratePowerUpNotiTime() {}
int TimerGeneratePowerUpNotiTime::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x863835a)：函数体未还原
}

TimerCheckPowerWarAbuser::TimerCheckPowerWarAbuser() {}
TimerCheckPowerWarAbuser::~TimerCheckPowerWarAbuser() {}
int TimerCheckPowerWarAbuser::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x863825c)：函数体未还原
}

TimerSetPunishUser::TimerSetPunishUser() {}
TimerSetPunishUser::~TimerSetPunishUser() {}
int TimerSetPunishUser::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, p2);
    if (user == 0 || (static_cast<unsigned int>(user->get_unique_id()) & 0xffffU) != p3)
    {
        return 0;
    }
    user->setHackUserFlag();
    user->SetTradePunishType(0x40);   // ORIG T _ZN5CUser17SetTradePunishTypeEi
    if (user->isARSUserKick())
    {
        user->DisConnSig(static_cast<DISCONN_SIG>(0x2e), 1, 0);   // ORIG 0x8638ba0
    }
    return 1;
}

TimerSetTradePunishUser::TimerSetTradePunishUser() {}
TimerSetTradePunishUser::~TimerSetTradePunishUser() {}
int TimerSetTradePunishUser::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p2, p1);
    if (user == 0 || (static_cast<unsigned int>(user->get_unique_id()) & 0xffffU) != p3)
    {
        return 0;
    }
    user->SetTradePunishType(4);
    return 1;
}

TimerRevivalCharac::TimerRevivalCharac() {}
TimerRevivalCharac::~TimerRevivalCharac() {}
int TimerRevivalCharac::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, p2);
    if (user == 0 || static_cast<unsigned int>(user->GetRevivalTime()) != p3)
    {
        return 0;
    }
    PacketGuard packet;
    packet.put_header(0, 0xe4);
    packet.finalize(true);
    user->Send(packet);
    return 1;
}

TimerRequestExit::TimerRequestExit() {}
TimerRequestExit::~TimerRequestExit() {}
int TimerRequestExit::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, p2);
    if (user == 0 || user->getCurCharacR() == 0 ||
        static_cast<unsigned int>(user->getCurCharacNo()) != p3)
    {
        return 0;
    }
    user->DisConnSig(static_cast<DISCONN_SIG>(0x2f), 1, 0);   // ORIG 0x8638f6a
    return 1;
}

TimerVillageMonsterUpdateRate::TimerVillageMonsterUpdateRate() {}
TimerVillageMonsterUpdateRate::~TimerVillageMonsterUpdateRate() {}
int TimerVillageMonsterUpdateRate::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // OnRun/OnRegen/OnStart 为非静态成员（CVillageMonsterMgr.h），经全局管理器实例调用。
    GlobalData::s_villageMonsterMgr->OnRunVillageMonster();
    return 1;
}

TimerVillageMonsterRegen::TimerVillageMonsterRegen() {}
TimerVillageMonsterRegen::~TimerVillageMonsterRegen() {}
int TimerVillageMonsterRegen::dispatch_sig(int p1, int p2, unsigned int p3)
{
    GlobalData::s_villageMonsterMgr->OnRegenVillageMonster(p2,
                                                           static_cast<int>(p3));
    return 1;
}

// ORIG 0x8639042：村战开启定时器（报告 TimerStartVillageAttacked/dispatch_sig.md）。
int TimerStartVillageAttacked::dispatch_sig(int /*p1*/, int /*p2*/,
                                            unsigned int /*p3*/)
{
    GlobalData::s_villageMonsterMgr->OnStartVillageAttacked();
    return 1;
}

TimerVillageAttackedPlayTimeout::TimerVillageAttackedPlayTimeout() {}
TimerVillageAttackedPlayTimeout::~TimerVillageAttackedPlayTimeout() {}
int TimerVillageAttackedPlayTimeout::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x863905c)：函数体未还原
}

TimerItemUnlockWaiting::TimerItemUnlockWaiting() {}
TimerItemUnlockWaiting::~TimerItemUnlockWaiting() {}
int TimerItemUnlockWaiting::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x86390ea)：函数体未还原
}

TimerUpdateCompoundEmblemStatics::TimerUpdateCompoundEmblemStatics() {}
TimerUpdateCompoundEmblemStatics::~TimerUpdateCompoundEmblemStatics() {}
int TimerUpdateCompoundEmblemStatics::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x8639174)：函数体未还原
}

TimerStartExpEventAfterVillageAttack::TimerStartExpEventAfterVillageAttack() {}
TimerStartExpEventAfterVillageAttack::~TimerStartExpEventAfterVillageAttack() {}
int TimerStartExpEventAfterVillageAttack::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // TriggerEventStart 为 CEventManager 成员；Word_Param 低 ushort=100（ORIG movw $0x64）。
    Word_Param param;
    param.m_duration = 100;
    param.m_field2 = 0;   // ORIG 未初始化，重建取 0
    GlobalData::s_event_manager->TriggerEventStart(3, param);
    return 1;
}

Timer_Security_NonClient_CheckResponse::Timer_Security_NonClient_CheckResponse() {}
Timer_Security_NonClient_CheckResponse::~Timer_Security_NonClient_CheckResponse() {}
int Timer_Security_NonClient_CheckResponse::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x86394f0)：函数体未还原
}

Timer_Midnight::Timer_Midnight() {}
Timer_Midnight::~Timer_Midnight() {}
int Timer_Midnight::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x8639512)：函数体未还原
}

TimerEndPowerWarVictoriousReward::TimerEndPowerWarVictoriousReward() {}
TimerEndPowerWarVictoriousReward::~TimerEndPowerWarVictoriousReward() {}
int TimerEndPowerWarVictoriousReward::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x86394ce)：函数体未还原
}

Timer_TenMinuteSchedule::Timer_TenMinuteSchedule() {}
Timer_TenMinuteSchedule::~Timer_TenMinuteSchedule() {}
int Timer_TenMinuteSchedule::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x8639668)：函数体未还原
}

Timer_UpdateOnlinePreliminaryTeam::Timer_UpdateOnlinePreliminaryTeam() {}
Timer_UpdateOnlinePreliminaryTeam::~Timer_UpdateOnlinePreliminaryTeam() {}
int Timer_UpdateOnlinePreliminaryTeam::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x8639722)：函数体未还原
}

Timer_SecretShopStatistic::Timer_SecretShopStatistic() {}
Timer_SecretShopStatistic::~Timer_SecretShopStatistic() {}
int Timer_SecretShopStatistic::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x86397ce)：函数体未还原
}

Timer_DailySchedule::Timer_DailySchedule() {}
Timer_DailySchedule::~Timer_DailySchedule() {}
int Timer_DailySchedule::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x812c920)：函数体未还原
}

Timer_ConnectP2PPvPTimeout::Timer_ConnectP2PPvPTimeout() {}
Timer_ConnectP2PPvPTimeout::~Timer_ConnectP2PPvPTimeout() {}
int Timer_ConnectP2PPvPTimeout::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x8639834)：函数体未还原
}

Timer_RoutingItemWait::Timer_RoutingItemWait() {}
Timer_RoutingItemWait::~Timer_RoutingItemWait() {}
int Timer_RoutingItemWait::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x86398a8)：函数体未还原
}

Timer_PcRoomAuthRetry::Timer_PcRoomAuthRetry() {}
Timer_PcRoomAuthRetry::~Timer_PcRoomAuthRetry() {}
int Timer_PcRoomAuthRetry::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x8639924)：函数体未还原
}

Timer_DungeonInoutOpenTime::Timer_DungeonInoutOpenTime() {}
Timer_DungeonInoutOpenTime::~Timer_DungeonInoutOpenTime() {}
int Timer_DungeonInoutOpenTime::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x8639e0a)：函数体未还原
}

Timer_DungeonInoutCloseTime::Timer_DungeonInoutCloseTime() {}
Timer_DungeonInoutCloseTime::~Timer_DungeonInoutCloseTime() {}
int Timer_DungeonInoutCloseTime::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x863a1d0)：函数体未还原
}

Timer_FairPvPRematch::Timer_FairPvPRematch() {}
Timer_FairPvPRematch::~Timer_FairPvPRematch() {}
int Timer_FairPvPRematch::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x863a430)：函数体未还原
}

Timer_ReturnUserTimeout::Timer_ReturnUserTimeout() {}
Timer_ReturnUserTimeout::~Timer_ReturnUserTimeout() {}
int Timer_ReturnUserTimeout::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x863a4c8)：函数体未还原
}

Timer_HadesHeartBeat::Timer_HadesHeartBeat() {}
Timer_HadesHeartBeat::~Timer_HadesHeartBeat() {}
int Timer_HadesHeartBeat::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x863a760)：函数体未还原
}

Timer_TOD_Send_ClearReward::Timer_TOD_Send_ClearReward() {}
Timer_TOD_Send_ClearReward::~Timer_TOD_Send_ClearReward() {}
int Timer_TOD_Send_ClearReward::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x863a89e)：函数体未还原
}

Timer_OneADayItemShop::Timer_OneADayItemShop() {}
Timer_OneADayItemShop::~Timer_OneADayItemShop() {}
int Timer_OneADayItemShop::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x863a916)：函数体未还原
}

Timer_Schedule_MidNight::Timer_Schedule_MidNight() {}
Timer_Schedule_MidNight::~Timer_Schedule_MidNight() {}
int Timer_Schedule_MidNight::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x863a982)：函数体未还原
}

Timer_Send_Ontime_Reward::Timer_Send_Ontime_Reward() {}
Timer_Send_Ontime_Reward::~Timer_Send_Ontime_Reward() {}
int Timer_Send_Ontime_Reward::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x863aa90)：函数体未还原
}

Timer_EventManagement::Timer_EventManagement() {}
Timer_EventManagement::~Timer_EventManagement() {}
int Timer_EventManagement::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x863ab92)：函数体未还原
}

Timer_QuickPartyWaitingPartyMatching::Timer_QuickPartyWaitingPartyMatching() {}
Timer_QuickPartyWaitingPartyMatching::~Timer_QuickPartyWaitingPartyMatching() {}
int Timer_QuickPartyWaitingPartyMatching::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x863ac18)：函数体未还原
}

Timer_QuickPartyWaitingStartGame::Timer_QuickPartyWaitingStartGame() {}
Timer_QuickPartyWaitingStartGame::~Timer_QuickPartyWaitingStartGame() {}
int Timer_QuickPartyWaitingStartGame::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x863ae82)：函数体未还原
}

Timer_AssaultGhostTimer::Timer_AssaultGhostTimer() {}
Timer_AssaultGhostTimer::~Timer_AssaultGhostTimer() {}
int Timer_AssaultGhostTimer::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x863b0ba)：函数体未还原
}

Timer_GenerateCaptchaData::Timer_GenerateCaptchaData() {}
Timer_GenerateCaptchaData::~Timer_GenerateCaptchaData() {}
int Timer_GenerateCaptchaData::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x863b29c)：函数体未还原
}

Timer_TournamentDugeonClearStateNotiReward::Timer_TournamentDugeonClearStateNotiReward() {}
Timer_TournamentDugeonClearStateNotiReward::~Timer_TournamentDugeonClearStateNotiReward() {}
int Timer_TournamentDugeonClearStateNotiReward::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x863b2fe)：函数体未还原
}

Timer_TournametDungeonClearStateSelect::Timer_TournametDungeonClearStateSelect() {}
Timer_TournametDungeonClearStateSelect::~Timer_TournametDungeonClearStateSelect() {}
int Timer_TournametDungeonClearStateSelect::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x863b4ea)：函数体未还原
}

Timer_TowerOnTime::Timer_TowerOnTime() {}
Timer_TowerOnTime::~Timer_TowerOnTime() {}
int Timer_TowerOnTime::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x863b8c4)：函数体未还原
}

Timer_NoticeEmergencyPatchMessage::Timer_NoticeEmergencyPatchMessage() {}
Timer_NoticeEmergencyPatchMessage::~Timer_NoticeEmergencyPatchMessage() {}
int Timer_NoticeEmergencyPatchMessage::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x863b6c4)：函数体未还原
}

advancealtar::Timer_MainTick::Timer_MainTick() {}
advancealtar::Timer_MainTick::~Timer_MainTick() {}
int advancealtar::Timer_MainTick::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x812f798)：函数体未还原
}

advancealtar::Timer_StageControl::Timer_StageControl() {}
advancealtar::Timer_StageControl::~Timer_StageControl() {}
int advancealtar::Timer_StageControl::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x812f806)：函数体未还原
}

Timer_DungeonStatisticEntrance_Hour::Timer_DungeonStatisticEntrance_Hour() {}
Timer_DungeonStatisticEntrance_Hour::~Timer_DungeonStatisticEntrance_Hour() {}
int Timer_DungeonStatisticEntrance_Hour::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x863b9da)：函数体未还原
}

Timer_RefreshServerState::Timer_RefreshServerState() {}
Timer_RefreshServerState::~Timer_RefreshServerState() {}
int Timer_RefreshServerState::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x863b90e)：函数体未还原
}

Timer_StayTimeEvent::Timer_StayTimeEvent() {}
Timer_StayTimeEvent::~Timer_StayTimeEvent() {}
int Timer_StayTimeEvent::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x863b928)：函数体未还原
}

Timer_CreateDnfDbLoad::Timer_CreateDnfDbLoad() {}
Timer_CreateDnfDbLoad::~Timer_CreateDnfDbLoad() {}
int Timer_CreateDnfDbLoad::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    return 0;  // TODO(ORIG 0x863ba46)：函数体未还原
}


const unsigned int advancealtar::Timer_MainTick::kMainTickMs = 1000;  // ORIG R 0x8b6c5e4
