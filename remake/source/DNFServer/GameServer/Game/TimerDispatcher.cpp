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
#include "CDungeonExpStatistic.h"
#include "CExpertJob.h"      // UpdateExpertJob / GetInstanceExpertJobStatistic
#include "CInventory.h"
#include "CUserProc.h"
#include "MsgQueueMgr.h"
#include "CStreamGuard.h"
#include "CGuildServerProxy.h"   // CEventManager 完整定义（GlobalData.h 仅前向）
#include "CMailBoxHelper.h"
#include "CHandlePremium.h"
#include "SPremiumInfo.h"
#include "GameServerDemonManager.h"  // GameserverDemonManagerInstance（LinkClosureB-2 声明头）
#include "CShutdowManager.h"
#include "CHackAnalyzer.h"     // ENUM_HACKTYPE/addServerHackCnt（_NS_NonClient_CheckResponse）         // GetInstanceShutdowManager/SendShutdownSignal
#include "ExchangeServer.h"
#include "VendingMachine.h"
#include "CDailyScheduleManager.h"
#include "DB_CreateCharac.h"
#include "W9_EventSmall.h"
#include "Arad_ServerStateManager.h"
#include "CSpecialItemRoutingManager.h"  // 完整类型（CGameManager.h 仅前向）
#include "CCoinRefillEvent.h"           // 金币补充 repeat 事件（本批新建模）
#include "CUserPlayTimeStatisticMgr.h"  // 在线游玩时长统计（本批新建模）
#include "CEventCreateDnf.h"            // CEventCreateDnf（CreateDnf 事件，Timer_CreateDnfDbLoad）
#include "TaiwanInternal.h"             // Taiwan::internal_stream（DBSelectCreateDnfEventInfo）

// ==== UNLINKED_CALLEE 前向声明：符号均 ORIG nm -S --defined-only 实证（T/W），
// 定义由后续批次排期；链接缺口汇总见 /tmp/df_batches/TimerBatch.md ====
void PvPDieUserRevive(int p1, int p2, unsigned int p3, int idx);        // ORIG T 0x08636555
int _NS_NonClient_CheckResponse(int a, int b, int c);                   // ORIG T 0x08274e4e
class DB_UpdatePvPGrade
{
public:
    static void makeRequest();   // ORIG T 0x084293a0（ORIG 无 this 直调）
};
class DB_DeathTowerLoadTopRanker
{
public:
    static void makeRequest(bool reload);   // ORIG T 0x0842a910（定义 W8LT_DBTask.cpp，本批落地）
};
#include "DBSelectCreateDnfEventInfo.h"  // 权威声明（OrigTaiwanDisp 新建）；原局部类按 §4.4 删除
#include "CEventStayTime.h"
// ORIG T _ZN30DB_UpdateCompoundEmblemStatics11makeRequestEijjjjjjj @0x08439dec：
// 定义缺失（无 TU 产出），此处按 ORIG mangled 提供声明，链接由集成者排期补 producer。
class DB_UpdateCompoundEmblemStatics
{
public:
    static void makeRequest(int channelId, unsigned int grade0, unsigned int grade1,
                            unsigned int grade2, unsigned int grade3, unsigned int grade4,
                            unsigned int grade5, unsigned int grade6);
};
#include "CSecretShop.h"
#include "OnlinePreliminary.h"
#include "COnTimeEvent.h"
#include "CHadesServerProxy.h"
#include "CDisconnectDetecter.h"
#include "CompoundEmblemDeps.h"
#include "CGlobalEffectManager.h"
// ======== 基类 ========


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
    m_dispatchers[124] = new TimerGeneratePowerUp();  // slot +0x1f0（ORIG ctor 0x086321d6 实证）
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
    APSystem::_ActionGroupIndex group23(0x23);
    APSystem::_ActionGroupIndex group24(0x24);
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
    PvP_Room* room = G_CGameManager()->GetPvp(p1, 0, 0x32);
    if (room == 0)
    {
        return 0;
    }
    if (room->get_timer_key((TIMER_MESSAGE)0x32) != p2)
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
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
    {
        return 0;
    }
    if (!party->check_timer_key((TIMER_MESSAGE)0xb, p2))
    {
        return 0;
    }
    party->map_load_forced_finish();
    return 1;
}

TimerSetResult::TimerSetResult() {}
TimerSetResult::~TimerSetResult() {}
int TimerSetResult::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
    {
        return 0;
    }
    if (!party->check_timer_key((TIMER_MESSAGE)0xc, p2))
    {
        return 0;
    }
    party->forced_set_play_result();
    return 1;
}

TimerDungeonClear::TimerDungeonClear() {}
TimerDungeonClear::~TimerDungeonClear() {}
int TimerDungeonClear::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
    {
        return 0;
    }
    if (!party->check_timer_key((TIMER_MESSAGE)0xd, p2))
    {
        return 0;
    }
    party->dungeon_clear();
    return 1;
}

TimerDungeonFail::TimerDungeonFail() {}
TimerDungeonFail::~TimerDungeonFail() {}
int TimerDungeonFail::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
    {
        return 0;
    }
    if (!party->check_timer_key((TIMER_MESSAGE)0xe, p2))
    {
        return 0;
    }
    party->party_dungeon_fail();
    return 1;
}

TimerPartyWalkOutLackUser::TimerPartyWalkOutLackUser() {}
TimerPartyWalkOutLackUser::~TimerPartyWalkOutLackUser() {}
int TimerPartyWalkOutLackUser::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x08633e7a：GetParty(p1) 按定时器 party index 取队伍（非无参重载）。
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
    {
        return 0;
    }
    if (!party->check_timer_key((TIMER_MESSAGE)0xf, p2))
    {
        return 0;
    }
    if (!party->OnTimerWalkOutLackUser())
    {
        return 0;
    }
    return 1;
}

TimerPVPWalkOutLackUser::TimerPVPWalkOutLackUser() {}
TimerPVPWalkOutLackUser::~TimerPVPWalkOutLackUser() {}
int TimerPVPWalkOutLackUser::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x08633e86：GetPvp(p1, 0, 0x31)；timer_key 与 p2 比较（非 p3）。
    PvP_Room* room = G_CGameManager()->GetPvp(p1, 0, 0x31);
    if (room == 0)
    {
        return 0;
    }
    if (room->get_timer_key((TIMER_MESSAGE)0x31) != p2)
    {
        return 0;
    }
    if (!room->OnTimerWalkOutLackUser())
    {
        room->reset();
        G_CGameManager()->PutPvp(room);
    }
    return 1;
}

TimerFatigueReset::TimerFatigueReset() {}
TimerFatigueReset::~TimerFatigueReset() {}
// ---- TimerFatigueReset ----
// 报告：docs/class_func_reports/TimerFatigueReset/{dispatch_sig,RegistNextTimer,updateAbnormalClientCount}.md
// 头已有声明：dispatch_sig / ~ / static int RegistNextTimer()（静态与非静态 mangled 相同 _ZN17..15RegistNextTimerEv，无需改头）。
// 类对象仅 vptr。
// ORIG T 0x086339fa（0xa 字节）：mov $0x1,%eax 直返 1。
int TimerFatigueReset::updateAbnormalClientCount()
{
    return 1;
}

// ORIG T 0x08633750 (_ZN17TimerFatigueReset12dispatch_sigEiij)
int TimerFatigueReset::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG：G_CEnvironment()+0x37c（每日重置小时）即 m_serverEnvir.m_fatigueTime；
    // ORIG 每处均重新取 env。
    const int dailyResetHour = G_CEnvironment()->m_serverEnvir.m_fatigueTime;
    static_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng())
        ->update_time_values(dailyResetHour);

    // ORIG 每个 GameWorld 成员调用前都重新取 G_GameWorld()，逐条保留。
    G_GameWorld()->ResetAllFatigue();
    G_GameWorld()->ResetAllCoins();
    G_GameWorld()->DailyEventModify();
    G_GameWorld()->ResetAllEventData();
    G_GameWorld()->ReselectDailyQuest();
    G_GameWorld()->DeleteDailyItem();
    G_GameWorld()->ResetAllCleadpadPoint();
    G_GameWorld()->ResetAllPassPadFailCount();
    G_GameWorld()->AddDailyItem();
    G_GameWorld()->resetDailyData();
    G_GameWorld()->ResetEntireReliablePerson();

    // ORIG：三个统计域上报（CLevelDungeonPlayStatistic@+0x0、CDungeonExpStatistic@+0x18、
    // CExpertJobStatistic 单例），与上面两个 InsertDatabase 同批落库。
    ((CLevelDungeonPlayStatistic*)GlobalData::s_statisticMgr)->InsertDatabase();
    ((CDungeonExpStatistic*)((char*)GlobalData::s_statisticMgr + 0x18))->InsertDatabase();
    GetInstanceExpertJobStatistic()->UpdateExpertJob();

    advancealtar::HistoryLog::updateStatistics();   // 已声明，定义未链接 → UNLINKED_CALLEE

    Stream* stream1 = GlobalData::s_stream_pool->Acquire("TimerDispatcher.cpp", 0x512);
    {
        CStreamGuard guard(stream1, true);          // 080c8c26 ctor(Stream*,bool)
        guard << 0x4e;                              // operator<<(int) 080c8c56
        guard << -1;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q /*2*/, guard);
    }

    Stream* stream2 = GlobalData::s_stream_pool->Acquire("TimerDispatcher.cpp", 0x51c);
    {
        CStreamGuard guard(stream2, true);
        guard << 0x50;
        guard << -1;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q /*4*/, guard);
    }

    DB_LoadAutoMarketConditionsControl::makeRequest(false);   // 定义 W9_EventSmall.cpp:30
    DB_CreateCharac::makeRequestResetLimitCreateMid();        // 定义 DB_CreateCharac.cpp
    G_GameWorld()->ResetOneADayItemShop();

    return RegistNextTimer() == 1;                   // ORIG movzbl+xor1 结构：flag!=0 返 1
}

// ORIG T 0x08633a04 (_ZN17TimerFatigueReset15RegistNextTimerEv)
// ⚠ ORIG 为非静态成员但不使用 this；当前头声明 static int 兼容（mangled 相同）。
int TimerFatigueReset::RegistNextTimer()
{
    int curSec = GlobalData::s_systemTime_.getCurSec();      // -0x4c

    const time_t midNow = static_cast<time_t>(curSec);
    tm timeInfo;                                             // local_7c
    tm* pt = localtime_r(&midNow, &timeInfo);

    const int hour = pt->tm_hour;                            // -0x24
    const int sec  = pt->tm_sec;
    const int min  = pt->tm_min;

    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x572, 0);
        trace("Reset all user's fatigue!(%02d:%02d:%02d)", hour, min, sec);
    }

    // env+0x37c 写回 tm_hour；min/sec 清零（ORIG 0x8(%eax)/0x4(%eax)/(%eax) 槽序）。
    pt->tm_hour = G_CEnvironment()->m_serverEnvir.m_fatigueTime;   // env+0x37c
    pt->tm_min = 0;
    pt->tm_sec = 0;

    int nextMidnight = static_cast<int>(mktime(pt));         // -0x20
    if (G_CEnvironment()->m_serverEnvir.m_fatigueTime <= hour)
        nextMidnight += 0x15180;                             // +86400s 跨天

    const int delta = nextMidnight - curSec;                 // -0x1c

    {
        cMyTrace trace2(__PRETTY_FUNCTION__, 0x587, 0);
        trace2("DELTA %d , CHECK_T %d , CUR_T %dn", delta, nextMidnight, curSec);
    }

    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2,
                                0,
                                (TIMER_MESSAGE)0x50,
                                delta,
                                0,
                                0);
    return 1;
}

// ORIG T 0x08632b32 (_ZN17TimerAvatarExpire11RegistTimerEP5CUser)
// 求值序（右→左）：getCurCharacNo → get_unique_id → env+0x380*0x3c → GetSlotIDX →
// G_TimerQueue；InsertTimer(0, slot, 0x51, env380*60, unique_id, characNo)；返回 1（void 化）。
void TimerAvatarExpire::RegistTimer(CUser* user)
{
	// env+0x380（每日过期时长分钟数）即 m_serverEnvir.m_avatarTime；
    // 内联实参保持 ORIG 求值序（getCurCharacNo → get_unique_id → env → GetSlotIDX → G_TimerQueue）。
    G_TimerQueue()->InsertTimer(
        TimerEntry::OBJ_TYPE_0,
        // ORIG movswl %ax：GetSlotIDX 返回值按 signed short 符号扩展（0x8632b32）。
        static_cast<int>(static_cast<short>(user->GetSlotIDX())),
        (TIMER_MESSAGE)0x51,
        G_CEnvironment()->m_serverEnvir.m_avatarTime * 0x3c,
        static_cast<unsigned short>(user->get_unique_id()),   // ORIG movzwl %ax
        static_cast<unsigned int>(user->getCurCharacNo()));
}

// TimerAvatarExpire ctor W 0x0863bb3e / ~D1 W 0x0863bb8a / ~D0 W 0x0863bb5a
// （ORIG 仅 vptr：TimerSig 基类 vptr 后覆写本类 vtable，与其它 Timer_* 同构）。
TimerAvatarExpire::TimerAvatarExpire() {}
TimerAvatarExpire::~TimerAvatarExpire() {}
// ORIG T 0x08632a78 (_ZN17TimerAvatarExpire12dispatch_sigEiij)：恒返回 1（mov $1,%eax; ret）。
int TimerAvatarExpire::dispatch_sig(int p1, int p2, unsigned int p3)
{
    return 1;
}

// ORIG T 0x08632a82 (_ZN17TimerAvatarExpire18CheckExpiredAvatarEP5CUser)
// PacketGuard 局部 + get_state==0 → 1；getCurCharacR==0 → 0；
// 有到期头像时经 getCurCharacInvenW 发包；恒 1。
int TimerAvatarExpire::CheckExpiredAvatar(CUser* user)
{
    PacketGuard guard;
    if (user->get_state() == 0)
    {
        return 1;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0;
    }
    if (user->getCurCharacInvenR()->isAnyExpiredAvatarItem())
    {
        user->getCurCharacInvenW()->ExpiredAvatarItem(&guard);
    }
    return 1;
}

TimerStatisticsLog::TimerStatisticsLog() {}
TimerStatisticsLog::~TimerStatisticsLog() {}
int TimerStatisticsLog::dispatch_sig(int p1, int p2, unsigned int p3)
{
    Stream* stream = GlobalData::s_stream_pool->Acquire("TimerDispatcher.cpp", 0x62f);
    CStreamGuard guard(stream, true);
    guard << 0xcd;
    guard << -1;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);

    int cacheLoadFailOverflowCount = exchange_server::GetInstanceExchangeServerHitRate()->GetCacheLoadFailOverflowCount();
    int cacheLoadFailCount = exchange_server::GetInstanceExchangeServerHitRate()->GetCacheLoadFailCount();
    int cacheLoadOverflowCount = exchange_server::GetInstanceExchangeServerHitRate()->GetCacheLoadOverflowCount();
    int cacheLoadCount = exchange_server::GetInstanceExchangeServerHitRate()->GetCacheLoadCount();
    int dbLoadOverflowCount = exchange_server::GetInstanceExchangeServerHitRate()->GetDBLoadOverflowCount();
    int dbLoadCount = exchange_server::GetInstanceExchangeServerHitRate()->GetDBLoadCount();

    cMyTrace trace(__PRETTY_FUNCTION__, 0x637, 0);
    trace("EXCHANGE_SERVER_HIT_RATE : db count(%u), overflow db count(%u), cache count(%u), overflow cache count(%u), cache fail count(%u), overflow cache fail count(%u)",
          dbLoadCount, dbLoadOverflowCount, cacheLoadCount, cacheLoadOverflowCount,
          cacheLoadFailCount, cacheLoadFailOverflowCount);

    if (TimerStatisticsLog::RegistNextTimer() == '\\x01')
    {
        return 1;
    }

    return 0;
}

TimerCardSelect::TimerCardSelect() {}
TimerCardSelect::~TimerCardSelect() {}
int TimerCardSelect::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
    {
        return 0;
    }
    if (!party->check_timer_key((TIMER_MESSAGE)0x10, p2))
    {
        return 0;
    }
    // ORIG：两次独立 get_dungeon_clear_state 调用（==0 / ==5 均提前返回）。
    if (party->get_dungeon_clear_state() == 0)
    {
        return 1;
    }
    if (party->get_dungeon_clear_state() == 5)
    {
        return 1;
    }
    if (!party->IsCompleteCardSelectOfMembers(eClearRewardCardType_t_0))
    {
        party->CompleteCardSelect();
        party->SendEachCardInfo();
    }
    party->set_dungeon_clear_state(5);
    party->dungeon_clear();
    return 1;
}

TimerClearDungeonReward::TimerClearDungeonReward() {}
TimerClearDungeonReward::~TimerClearDungeonReward() {}
int TimerClearDungeonReward::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
    {
        return 0;
    }
    if (!party->check_timer_key((TIMER_MESSAGE)0x11, p2))
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
    // ORIG 0x086345f2：GetParty(p1) + check_timer_key(0x12, p2)。
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0 || !party->check_timer_key((TIMER_MESSAGE)0x12, p2) ||
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
    // ORIG 0x086347e2：GetParty(p1) + check_timer_key(0x13, p2)。
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0 || !party->check_timer_key((TIMER_MESSAGE)0x13, p2))
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
    CUser* user = G_CGameManager()->getUser(p1, 5);   // ORIG 第二实参为常量 5
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
    CUser* user = G_CGameManager()->getUser(p1, 6);   // ORIG 第二实参为常量 6
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
    tm midnight;
    localtime_r(&now, &midnight);   // ORIG 0x086349ae：localtime_r（线程安全）
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
    CUser* user = G_CGameManager()->getUser(p1, 6);   // ORIG 第二实参为常量 6
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
            if (g_SPremiumInfo[info->m_type].m_kind != 2)
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
    CUser* user = G_CGameManager()->getUser(p1, 6);   // ORIG 第二实参为常量 6
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
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
    {
        return 0;
    }
    if (!party->check_timer_key((TIMER_MESSAGE)0x14, p2))
    {
        return 0;
    }
    party->ReturnToVillage();
    return 1;
}

TimerEPLPCommandWait::TimerEPLPCommandWait() {}
TimerEPLPCommandWait::~TimerEPLPCommandWait() {}
int TimerEPLPCommandWait::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
    {
        return 0;
    }
    if (!party->check_timer_key((TIMER_MESSAGE)0x15, p2))
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
    WarRoom* room = G_CGameManager()->GetWarRoom(p1);
    if (room == 0)
    {
        return 0;
    }
    if (!room->CheckTimerKey((TIMER_MESSAGE)0x41, p2))
    {
        return 0;
    }
    room->ChangeState(WARROOM_STATE_2);
    return 1;
}

TimerWarRoomReadyLockCount::TimerWarRoomReadyLockCount() {}
TimerWarRoomReadyLockCount::~TimerWarRoomReadyLockCount() {}
int TimerWarRoomReadyLockCount::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x08634d86：GetWarRoom(p1) + CheckTimerKey(0x42, p2)。
    WarRoom* room = G_CGameManager()->GetWarRoom(p1);
    if (room != 0 && room->CheckTimerKey((TIMER_MESSAGE)0x42, p2))
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
    // ORIG 0x08634df6：GetWarRoom(p1) + CheckTimerKey(0x43, p2)。
    WarRoom* room = G_CGameManager()->GetWarRoom(p1);
    if (room != 0 && room->CheckTimerKey((TIMER_MESSAGE)0x43, p2))
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
    WarRoom* room = G_CGameManager()->GetWarRoom(p1);
    if (room == 0)
    {
        return 0;
    }
    if (!room->CheckTimerKey((TIMER_MESSAGE)0x44, p2))
    {
        return 0;
    }
    room->HandleTimerResultCount();
    return 1;
}

TImerWarRoomTimeBomb::TImerWarRoomTimeBomb() {}
TImerWarRoomTimeBomb::~TImerWarRoomTimeBomb() {}
int TImerWarRoomTimeBomb::dispatch_sig(int p1, int p2, unsigned int p3)
{
    WarRoom* room = G_CGameManager()->GetWarRoom(p1);
    if (room == 0)
    {
        return 0;
    }
    if (!room->CheckTimerKey((TIMER_MESSAGE)0x45, p2))
    {
        return 0;
    }
    room->HandleTimerTimeBomb();
    return 1;
}

TimerWarRoomCheckFail::TimerWarRoomCheckFail() {}
TimerWarRoomCheckFail::~TimerWarRoomCheckFail() {}
int TimerWarRoomCheckFail::dispatch_sig(int p1, int p2, unsigned int p3)
{
    WarRoom* room = G_CGameManager()->GetWarRoom(p1);
    if (room == 0)
    {
        return 0;
    }
    if (!room->CheckTimerKey((TIMER_MESSAGE)0x46, p2))
    {
        return 0;
    }
    room->WarRoomFail();
    return 1;
}

TimerWarRoomChangeHost::TimerWarRoomChangeHost() {}
TimerWarRoomChangeHost::~TimerWarRoomChangeHost() {}
int TimerWarRoomChangeHost::dispatch_sig(int p1, int p2, unsigned int p3)
{
    WarRoom* room = G_CGameManager()->GetWarRoom(p1);
    if (room == 0)
    {
        return 0;
    }
    if (!room->CheckTimerKey((TIMER_MESSAGE)0x47, p2))
    {
        return 0;
    }
    room->CheckHostChange();
    return 1;
}

TimerBloodSmallRoundRegister::TimerBloodSmallRoundRegister() {}
TimerBloodSmallRoundRegister::~TimerBloodSmallRoundRegister() {}
int TimerBloodSmallRoundRegister::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x08634fee：GetParty(p1) + check_timer_key(0x16, p2)。
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0 || !party->check_timer_key((TIMER_MESSAGE)0x16, p2))
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
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0 || !party->check_timer_key((TIMER_MESSAGE)0x17, p2))
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
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0 || !party->check_timer_key((TIMER_MESSAGE)0x18, p2))
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
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0 || !party->check_timer_key((TIMER_MESSAGE)0x19, p2))
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
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0 || !party->check_timer_key((TIMER_MESSAGE)0x1a, p2))
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
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0 || !party->check_timer_key((TIMER_MESSAGE)0x1b, p2))
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
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0 || !party->check_timer_key((TIMER_MESSAGE)0x1c, p2))
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
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0 || !party->check_timer_key((TIMER_MESSAGE)0x1d, p2))
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
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0 || !party->check_timer_key((TIMER_MESSAGE)0x1e, p2))
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
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0 || !party->check_timer_key((TIMER_MESSAGE)0x1f, p2))
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
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0 || !party->check_timer_key((TIMER_MESSAGE)0x20, p2))
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
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
    {
        return 0;
    }
    if (!party->check_timer_key((TIMER_MESSAGE)0x21, p2))
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
    // ORIG 0x08638cd0：GetParty(p1)；check_timer_key 以 p3 为 msg、p2 为 param。
    CParty* party = G_CGameManager()->GetParty(p1);
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
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0 || !party->check_timer_key((TIMER_MESSAGE)p3, p2))
    {
        return 0;
    }
    if (party->m_dungeon == 0 ||
        party->m_dungeon->get_blood_dungeon_type() != 2)
    {
        return 0;
    }
    CBattle_Field* field = reinterpret_cast<CBattle_Field*>(reinterpret_cast<char*>(party) + 0xb24);   // ORIG：party+0xb24 子对象
    if (field->getUltimateDifficylty() != 0)
    {
        return 0;
    }
    const int diff = field->getNextRoundRandomDifficulty();
    field->setUltimateDifficulty(static_cast<char>(diff));
    const int roundOrder = static_cast<int>(field->m_bloodRound.getBloodRoundOrder());
    if (roundOrder != field->getBloodMaxRound() - 1)
    {
        field->sendUltimateRandomDifficulty(roundOrder, diff);
    }
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
// ORIG T 0x0863575c (_ZN32TimerStatisticsDetailChannelInfo12dispatch_sigEiij)：
// G_GameWorld()->updateDetailChannelServiceInfo()；RegistNextTimer()；返回 1。
int TimerStatisticsDetailChannelInfo::dispatch_sig(int p1, int p2, unsigned int p3)
{
    G_GameWorld()->updateDetailChannelServiceInfo();
    RegistNextTimer();
    return 1;
}
// ORIG T 0x08635782 (_ZN32TimerStatisticsDetailChannelInfo15RegistNextTimerEv)
// cur=getCurSec()；localtime_r → next=(tm_min%0x1e)*(-0x3c)+(0x708-tm_sec)（对齐半小时）；
// cMyTrace("content min:%d sec:%d next sec:%d  (channel no:%d)")；
// InsertTimer(2, 0, 0x54, next, 0, 0)；返回 1（void 化）。
void TimerStatisticsDetailChannelInfo::RegistNextTimer()
{
    const int curSec = GlobalData::s_systemTime_.getCurSec();
    const time_t now = static_cast<time_t>(curSec);
    tm timeInfo;
    tm* pt = localtime_r(&now, &timeInfo);
    const int nextSec = (pt->tm_min % 0x1e) * -0x3c + (0x708 - pt->tm_sec);
    const int channelNo = G_CEnvironment()->get_channel_no();
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x92c, 0);
        trace("content min:%d sec:%d next sec:%d  (channel no:%d)\n",
              pt->tm_min, pt->tm_sec, nextSec, channelNo);
    }
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x54, nextSec, 0, 0);
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
// ORIG TimerQueryCounter::dispatch_sig @ 0x8635a60 (0x17b)
int TimerQueryCounter::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // 第一段：LOGDB_Q(4)，值 0x7b / -1
    Stream* streamA = GlobalData::s_stream_pool->Acquire("TimerDispatcher.cpp", 0x97b);
    CStreamGuard guard(streamA, true);
    guard << 0x7b << -1;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);

    // 第二段：DB_Q(2)，值 0x7c / -1
    Stream* streamB = GlobalData::s_stream_pool->Acquire("TimerDispatcher.cpp", 0x981);
    CStreamGuard guard2(streamB, true);
    guard2 << 0x7c << -1;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard2);

    RegistNextTimer();
    return 1;
}

// ORIG T 0x8635bdc（RegistNextTimer.md 全文转写；声明已在 TimerDispatcher.h，
// 签名 static void；ORIG 恒返回 eax=1，void 化由现有头口径决定）
void TimerQueryCounter::RegistNextTimer()
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x59,
                                0x708, 0, 0);
}

TimerNatType::TimerNatType() {}
TimerNatType::~TimerNatType() {}
// ORIG T 0x08635c2c (_ZN12TimerNatType12dispatch_sigEiij)：
// G_GameWorld()->GetStatisticsNatType()->UpdateNatType()；RegistNextTimer()；返回 1。
int TimerNatType::dispatch_sig(int p1, int p2, unsigned int p3)
{
    G_GameWorld()->GetStatisticsNatType()->UpdateNatType();
    RegistNextTimer();
    return 1;
}
// ORIG T 0x08635c5a (_ZN12TimerNatType15RegistNextTimerEv)：
// G_TimerQueue()->InsertTimer(2, 0, 0x5a, 0x15180, 0, 0)；返回 1（void 化）。
void TimerNatType::RegistNextTimer()
{
    // ORIG 先把 delay 常量暂存 -0xc(%ebp) 再装入实参槽（gcc4.1 形态），
    // 用局部常量复现该栈槽暂存（同 TimerFatigueReset 先例）。
    const int delay = 0x15180;
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x5a, delay, 0, 0);
}

TimerNatTypeUser::TimerNatTypeUser() {}
TimerNatTypeUser::~TimerNatTypeUser() {}
int TimerNatTypeUser::dispatch_sig(int p1, int p2, unsigned int p3)
{
    G_GameWorld()->UpdateNatTypeUser();
    RegistNextTimer();
    return 1;
}

// ORIG T 0x08635cd0 (_ZN16TimerNatTypeUser15RegistNextTimerEv)：
// G_TimerQueue()->InsertTimer(2, 0, 0x5b, 0xe10, 0, 0)；返回 1（void 化）。
void TimerNatTypeUser::RegistNextTimer()
{
    // ORIG 先把 delay 常量暂存 -0xc(%ebp) 再装入实参槽（gcc4.1 形态），
    // 用局部常量复现该栈槽暂存（同 TimerFatigueReset 先例）。
    const int delay = 0xe10;
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x5b, delay, 0, 0);
}

TimerCheckMailTimeOut::TimerCheckMailTimeOut() {}
TimerCheckMailTimeOut::~TimerCheckMailTimeOut() {}
int TimerCheckMailTimeOut::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, 7);   // ORIG 第二实参为常量 7
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
    CUser* user = G_CGameManager()->getUser(p1, 0xa);   // ORIG 第二实参常量 0xa
    if (user == 0)
        return 0;
    const char* name = user->get_charac_name_by_charac_no(p3);
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0xa52, 0);
        trace("TimerForbidChat %s", name);
    }
    if (G_GameWorld()->EnableChat(user->get_charac_name_by_charac_no(p3)))
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0xa56, 0);
        trace("Enabled to chat for: %s", user->get_charac_name_by_charac_no(p3));
    }
    else
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0xa54, 0);
        trace("Failed to enable to chat for: %s", user->get_charac_name_by_charac_no(p3));
    }
    return 1;
}

TimerForbidMove::TimerForbidMove() {}
TimerForbidMove::~TimerForbidMove() {}
int TimerForbidMove::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, 0xb);   // ORIG 第二实参常量 0xb
    if (user == 0)
        return 0;
    const char* name = user->get_charac_name_by_charac_no(p3);
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0xa64, 0);
        trace("TimerForbidMove %s", name);
    }
    if (G_GameWorld()->EnableMove(user->get_charac_name_by_charac_no(p3)))
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0xa68, 0);
        trace("Enabled to move for: %s", user->get_charac_name_by_charac_no(p3));
    }
    else
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0xa66, 0);
        trace("Failed to enable to move for: %s", user->get_charac_name_by_charac_no(p3));
    }
    return 1;
}

TimerScheduleFiveMinPerUser::TimerScheduleFiveMinPerUser() {}
TimerScheduleFiveMinPerUser::~TimerScheduleFiveMinPerUser() {}
int TimerScheduleFiveMinPerUser::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, 3);   // ORIG 第二实参为常量 3
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
    CUser* user = G_CGameManager()->getUser(p1, 3);   // ORIG 第二实参为常量 3
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

// ORIG T 0x08636348 (_ZN17TimerQueueSizeLog15RegistNextTimerEv)：
// G_TimerQueue()->InsertTimer(2, 0, 0x61, 0x3c, 0, 0)；返回 1。
bool TimerQueueSizeLog::RegistNextTimer()
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x61, 0x3c, 0, 0);
    return 1;
}

TimerScheduleGuildWarEnd::TimerScheduleGuildWarEnd() {}
TimerScheduleGuildWarEnd::~TimerScheduleGuildWarEnd() {}
// TimerScheduleGuildWarEnd::dispatch_sig ORIG T 0x86331d0 (0x57f)
int TimerScheduleGuildWarEnd::dispatch_sig(int p1, int p2, unsigned int p3)
{
    if (G_GameWorld()->GetChannelType() != 6)
    {
        return 1;
    }
    // ORIG 08633209：CEventManager::GetRepeatEvent(0x21)；vptr+0x34 虚调用判活
    CEventBase* event = GlobalData::s_event_manager->GetRepeatEvent(0x21);
    if (!event->IsEventing((CUser*)0))
    {
        return 1;
    }
    if (p3 == 10)
    {
        PacketGuard packet;  // ORIG local @ -0x14(%ebp)
        packet.put_header(0, 0x6d);
        packet.put_byte(10);
        packet.finalize(true);
        G_GameWorld()->send_all(packet);
        // ORIG 086332d5：InsertTimer(OBJ_TYPE_2,0,0x62,300,5,0)
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                    (TIMER_MESSAGE)0x62, 300, 5, 0);
    }
    else if (p3 == 5)
    {
        PacketGuard packet;  // ORIG local @ -0x20(%ebp)
        packet.put_header(0, 0x6d);
        packet.put_byte(5);
        packet.finalize(true);
        G_GameWorld()->send_all(packet);
        // ORIG 086333a9：InsertTimer(OBJ_TYPE_2,0,0x62,60,4,0)
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                    (TIMER_MESSAGE)0x62, 60, 4, 0);
    }
    else if (p3 == 4)
    {
        PacketGuard packet;  // ORIG local @ -0x2c(%ebp)
        packet.put_header(0, 0x6d);
        packet.put_byte(4);
        packet.finalize(true);
        G_GameWorld()->send_all(packet);
        // ORIG 0863347d：InsertTimer(OBJ_TYPE_2,0,0x62,60,3,0)
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                    (TIMER_MESSAGE)0x62, 60, 3, 0);
    }
    else if (p3 == 3)
    {
        PacketGuard packet;  // ORIG local @ -0x38(%ebp)
        packet.put_header(0, 0x6d);
        packet.put_byte(3);
        packet.finalize(true);
        G_GameWorld()->send_all(packet);
        // ORIG 08633551：InsertTimer(OBJ_TYPE_2,0,0x62,60,2,0)
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                    (TIMER_MESSAGE)0x62, 60, 2, 0);
    }
    else if (p3 == 2)
    {
        PacketGuard packet;  // ORIG local @ -0x44(%ebp)
        packet.put_header(0, 0x6d);
        packet.put_byte(2);
        packet.finalize(true);
        G_GameWorld()->send_all(packet);
        // ORIG 08633625：InsertTimer(OBJ_TYPE_2,0,0x62,60,1,0)
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                    (TIMER_MESSAGE)0x62, 60, 1, 0);
    }
    else if (p3 == 1)
    {
        PacketGuard packet;  // ORIG local @ -0x50(%ebp)
        packet.put_header(0, 0x6d);
        packet.put_byte(1);
        packet.finalize(true);
        G_GameWorld()->send_all(packet);
        // ORIG 086336f9：InsertTimer(OBJ_TYPE_2,0,0x62,60,0,0)
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                    (TIMER_MESSAGE)0x62, 60, 0, 0);
    }
    else if (p3 == 0)
    {
        // ORIG 0863373e：TriggerEventEnd(0x21)
        GlobalData::s_event_manager->TriggerEventEnd(0x21);
    }
    return 1;
}

TimerSetPlayResult::TimerSetPlayResult() {}
TimerSetPlayResult::~TimerSetPlayResult() {}
int TimerSetPlayResult::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
    {
        return 0;
    }
    if (!party->check_timer_key((TIMER_MESSAGE)0x29, p2))
    {
        return 0;
    }
    party->SetPlayResult(party->getManager());
    return 1;
}

TimerStartRelayBattle::TimerStartRelayBattle() {}
TimerStartRelayBattle::~TimerStartRelayBattle() {}
int TimerStartRelayBattle::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x0863640c：GetPvp(p1, 0, 99) → relay(=room+0x620) 计时键比对(p2) → OnFight。
    PvP_Room* room = G_CGameManager()->GetPvp(p1, 0, 99);
    if (room == 0)
    {
        return 0;
    }
    if (room->m_relayMgr.GetTimerKey() != p2)   // ORIG room+0x620
    {
        return 0;
    }
    room->m_relayMgr.OnFight();
    return 1;
}
int TimerBossDieTimeout::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
    {
        return 0;
    }
    // ORIG 0x08636426：直接透传 OnTimeoutBossDie 返回值（CParty.h 声明 int）。
    return party->OnTimeoutBossDie(p2, p3);
}

TimerEventCoinRefill::TimerEventCoinRefill() {}
TimerEventCoinRefill::~TimerEventCoinRefill() {}
int TimerEventCoinRefill::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    // ORIG 0x086364c4：GetRepeatEvent(0xd) → receiver->IsEventing(0)（vtbl+0x34）
    // → GameWorld::ResetAllCoins() → trace → registNextRefillTimer()。
    CCoinRefillEvent* event =
        static_cast<CCoinRefillEvent*>(GlobalData::s_event_manager->GetRepeatEvent(0xd));
    if (event->IsEventing(0))   // ORIG 0x086364c4：无 event!=0 检查，直接解引用
    {
        G_GameWorld()->ResetAllCoins();
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0xadc, 0);
        trace("Coin refilled by event!");
        event->registNextRefillTimer();
    }
    return 1;
}

TimerPvPDieUserRevive0::TimerPvPDieUserRevive0() {}
TimerPvPDieUserRevive0::~TimerPvPDieUserRevive0() {}
int TimerPvPDieUserRevive0::dispatch_sig(int p1, int p2, unsigned int p3)
{
    PvPDieUserRevive(p1, p2, p3, 0);
}

TimerPvPDieUserRevive1::TimerPvPDieUserRevive1() {}
TimerPvPDieUserRevive1::~TimerPvPDieUserRevive1() {}
int TimerPvPDieUserRevive1::dispatch_sig(int p1, int p2, unsigned int p3)
{
    PvPDieUserRevive(p1, p2, p3, 1);
}

TimerPvPDieUserRevive2::TimerPvPDieUserRevive2() {}
TimerPvPDieUserRevive2::~TimerPvPDieUserRevive2() {}
int TimerPvPDieUserRevive2::dispatch_sig(int p1, int p2, unsigned int p3)
{
    PvPDieUserRevive(p1, p2, p3, 2);
}

TimerPvPDieUserRevive3::TimerPvPDieUserRevive3() {}
TimerPvPDieUserRevive3::~TimerPvPDieUserRevive3() {}
int TimerPvPDieUserRevive3::dispatch_sig(int p1, int p2, unsigned int p3)
{
    PvPDieUserRevive(p1, p2, p3, 3);
}

TimerPvPDieUserRevive4::TimerPvPDieUserRevive4() {}
TimerPvPDieUserRevive4::~TimerPvPDieUserRevive4() {}
int TimerPvPDieUserRevive4::dispatch_sig(int p1, int p2, unsigned int p3)
{
    PvPDieUserRevive(p1, p2, p3, 4);
}

TimerPvPDieUserRevive5::TimerPvPDieUserRevive5() {}
TimerPvPDieUserRevive5::~TimerPvPDieUserRevive5() {}
int TimerPvPDieUserRevive5::dispatch_sig(int p1, int p2, unsigned int p3)
{
    PvPDieUserRevive(p1, p2, p3, 5);
}

TimerPvPDieUserRevive6::TimerPvPDieUserRevive6() {}
TimerPvPDieUserRevive6::~TimerPvPDieUserRevive6() {}
int TimerPvPDieUserRevive6::dispatch_sig(int p1, int p2, unsigned int p3)
{
    PvPDieUserRevive(p1, p2, p3, 6);
}

TimerPvPDieUserRevive7::TimerPvPDieUserRevive7() {}
TimerPvPDieUserRevive7::~TimerPvPDieUserRevive7() {}
int TimerPvPDieUserRevive7::dispatch_sig(int p1, int p2, unsigned int p3)
{
    PvPDieUserRevive(p1, p2, p3, 7);
}

TimerDungeonDataStatistic::TimerDungeonDataStatistic() {}
TimerDungeonDataStatistic::~TimerDungeonDataStatistic() {}
// ---- TimerDungeonDataStatistic ----
// 报告：docs/class_func_reports/TimerDungeonDataStatistic/{dispatch_sig,RegistNextTimer}.md

// ORIG T 0x0863672a (_ZN25TimerDungeonDataStatistic12dispatch_sigEiij)
int TimerDungeonDataStatistic::dispatch_sig(int p1, int p2, unsigned int p3)
{
    Stream* stream = GlobalData::s_stream_pool->Acquire("TimerDispatcher.cpp", 0xb1a);
    CStreamGuard guard(stream, true);
    guard << 0xaf;
    guard << -1;
    SIG_DUNGEON_DATA_STATISTIC* stat =
        guard.GetInBuffer<SIG_DUNGEON_DATA_STATISTIC>();
    memset(stat, 0, 0x80);
    *(int*)stat = 3;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    GetInstanceVendingMachineStatistic()->updateDataIntoDB();
    return RegistNextTimer() == 1;
}

// ORIG T 0x08636856 (_ZN25TimerDungeonDataStatistic15RegistNextTimerEv)
// 注意：TimerDispatcher.h 现声明为 `static void RegistNextTimer()`，
// ORIG 返回 int（dispatch_sig 以返回值==1 决定结果），集成者需改声明为 int。
int TimerDungeonDataStatistic::RegistNextTimer()
{
    const int curSec = GlobalData::s_systemTime_.getCurSec();
    const time_t nowT = static_cast<time_t>(curSec);
    tm t;
    localtime_r(&nowT, &t);
    const int curHour = t.tm_hour;
    t.tm_hour = 5;
    t.tm_min = 0;
    t.tm_sec = 0;
    t.tm_min = G_CEnvironment()->m_serverEnvir.m_channelNo % 0x1e;  // +0x1b0
    int target = static_cast<int>(mktime(&t));
    if (t.tm_hour <= curHour)
        target += 0x15180;
    const int delay = target - curSec;
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x66,
                                delay, 0, 0);
    return 1;
}

TimerNotResponseUserTimeout::TimerNotResponseUserTimeout() {}
TimerNotResponseUserTimeout::~TimerNotResponseUserTimeout() {}
// ---- TimerNotResponseUserTimeout ----
// 报告：docs/class_func_reports/TimerNotResponseUserTimeout/dispatch_sig.md

// ORIG T 0x0863695a (_ZN27TimerNotResponseUserTimeout12dispatch_sigEiij)
int TimerNotResponseUserTimeout::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, 0xc);
    if (user == NULL)
        return 0;
    if ((unsigned short)user->get_unique_id() != p2)
        return 0;
    if (user->IsRecvEvent() != 1)
    {
        char ip[20];
        ip[0] = '\\0';
        ip[1] = '\\0';
        ip[2] = '\\0';
        ip[3] = '\\0';
        ip[4] = '\\0';
        ip[5] = '\\0';
        ip[6] = '\\0';
        ip[7] = '\\0';
        ip[8] = '\\0';
        ip[9] = '\\0';
        ip[10] = '\\0';
        ip[11] = '\\0';
        ip[12] = '\\0';
        ip[13] = '\\0';
        ip[14] = '\\0';
        ip[15] = '\\0';
        ip[16] = '\\0';
        ip[17] = '\\0';
        ip[18] = '\\0';
        ip[19] = '\\0';
        const char* ipStr = user->m_network.GetPeerIP2(ip, 0x14);  // +0xe0
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0xb73, 0);
        trace("Not response user timeout: idx(%d), IP(%s)", p1, ipStr);
    }
    return 1;
}

TimerLightServerNotice::TimerLightServerNotice() {}
TimerLightServerNotice::~TimerLightServerNotice() {}
int TimerLightServerNotice::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int p3)
{
    // ORIG 0x08636a7c：越界检查（p3 负或 >= size 直接 false）→ 构造 0xc 包
    // {byte 0, short 0, byte 0, int len, str} 广播 → RegistNextTimer(p3)。
    const std::vector<stLightServerNotice_t>& notices =
        G_CDataManager()->m_serverParameter.m_light3c;
    if (static_cast<int>(p3) < 0 || p3 >= notices.size())
    {
        return 0;
    }
    PacketGuard guard;
    InterfacePacketBuf& packet = static_cast<InterfacePacketBuf&>(guard);
    packet.put_header(0, 0xc);
    packet.put_byte(0);
    packet.put_short(0);
    packet.put_byte(0);
    int len = notices[p3].m_message.size();
    packet.put_str(notices[p3].m_message.c_str(), len);
    packet.finalize(true);
    G_GameWorld()->send_all(guard);
    return RegistNextTimer(static_cast<int>(p3)) == 1;
}

// ORIG T 0x08636c14（_ZN22TimerLightServerNotice15RegistNextTimerEi）
bool TimerLightServerNotice::RegistNextTimer(int idx)
{
    int curSec = GlobalData::s_systemTime_.getCurSec();
    tm cur;
    localtime_r(reinterpret_cast<time_t*>(&curSec), &cur);
    int prevHour = cur.tm_hour;
    const stLightServerNotice_t& notice = G_CDataManager()->m_serverParameter.m_light3c[idx];
    cur.tm_hour = notice.m_hour;
    cur.tm_min = notice.m_min;
    cur.tm_sec = 0;
    int next = static_cast<int>(mktime(&cur));
    if (cur.tm_hour <= prevHour)
    {
        next += 0x15180;
    }
    int delay = next - curSec;
    G_TimerQueue()->InsertTimer(static_cast<TimerEntry::OBJ_TYPE>(2), 0,
                                static_cast<TIMER_MESSAGE>(0x68), delay, idx, 0);
    return 1;
}

TimerInsertUserPlayTime::TimerInsertUserPlayTime() {}
TimerInsertUserPlayTime::~TimerInsertUserPlayTime() {}
int TimerInsertUserPlayTime::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    // ORIG 0x08636d0a：统计单例落盘 → 重排当日 04:30 定时器。
    GetInstanceUserPlayTimeStatisticMgr()->InsertUserPlayTime();
    return RegistNextTimer() == 1;
}

// ORIG T 0x08636d44（_ZN23TimerInsertUserPlayTime15RegistNextTimerEv）
bool TimerInsertUserPlayTime::RegistNextTimer()
{
    int curSec = GlobalData::s_systemTime_.getCurSec();
    tm cur;
    localtime_r(reinterpret_cast<time_t*>(&curSec), &cur);
    int prevHour = cur.tm_hour;
    cur.tm_hour = 4;
    cur.tm_min = 0x1e;
    cur.tm_sec = 0;
    int next = static_cast<int>(mktime(&cur));
    if (cur.tm_hour <= prevHour)
    {
        next += 0x15180;
    }
    int delay = next - curSec;
    G_TimerQueue()->InsertTimer(static_cast<TimerEntry::OBJ_TYPE>(2), 0,
                                static_cast<TIMER_MESSAGE>(0x69), delay, 0, 0);
    return 1;
}

TimerCheckForcedDisconnect::TimerCheckForcedDisconnect() {}
TimerCheckForcedDisconnect::~TimerCheckForcedDisconnect() {}
// ---- TimerCheckForcedDisconnect ----
// 报告：docs/class_func_reports/TimerCheckForcedDisconnect/{dispatch_sig,RegistNextTimer}.md

// ORIG T 0x08636e06 (_ZN26TimerCheckForcedDisconnect12dispatch_sigEiij)
int TimerCheckForcedDisconnect::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p2, 0x2);
    if (user == NULL)
        return 0;
    if ((unsigned short)user->get_unique_id() != p3)
        return 0;
    if (user->get_state() != 7)
        return 1;
    PacketGuard packet;
    packet.put_header(0, 0x7b);
    packet.finalize(true);
    user->Send(packet);
    return RegistNextTimer(user) == 1;
}

// ORIG T 0x08636f26 (_ZN26TimerCheckForcedDisconnect15RegistNextTimerEP5CUser)
// 头声明为 static bool；ORIG eax 恒为 1，语义等价。
// TimerDispatcher.h 现签名 `static bool RegistNextTimer(CUser*)` 即可，无需改动。
bool TimerCheckForcedDisconnect::RegistNextTimer(CUser* user)
{
    const unsigned short uid16 =
        static_cast<unsigned short>(user->get_unique_id());
    const unsigned int uid = static_cast<unsigned int>(user->GetUID());
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, uid,
                                (TIMER_MESSAGE)0x6a, 10, uid16, 0);
    return 1;
}

TimerCheckPvPRoomUser::TimerCheckPvPRoomUser() {}
TimerCheckPvPRoomUser::~TimerCheckPvPRoomUser() {}
// ===== TimerCheckPvPRoomUser =====
// dispatch_sig ORIG _ZN21TimerCheckPvPRoomUser12dispatch_sigEiij @0x08636f90 (0xa9B)

int TimerCheckPvPRoomUser::dispatch_sig(int p1, int p2, unsigned int /*p3*/)
{
    PvP_Room* room = G_CGameManager()->GetPvp(p1, (CUser*)0, (int)0x3c);
    if (room == 0)
    {
        return 0;
    }
    if (room->get_timer_key((TIMER_MESSAGE)0x3c) != p2)
    {
        return 0;
    }
    if (room->get_state() != 2)
    {
        return 0;
    }
    // ORIG +0x80: check_recv_pvp_rank()
    room->check_recv_pvp_rank();
    // ORIG +0x90: IsEndPvpBattle(); xor $1; 为真（即返回非 1）时 send_pvp_end
    if ((unsigned char)(room->IsEndPvpBattle() ^ 1) != 0)
    {
        room->send_pvp_end();
    }
    return 1;
}

TimerCheckRecvPvpEndResult::TimerCheckRecvPvpEndResult() {}
TimerCheckRecvPvpEndResult::~TimerCheckRecvPvpEndResult() {}
// ---- TimerCheckRecvPvpEndResult ----
// 报告：docs/class_func_reports/TimerCheckRecvPvpEndResult/dispatch_sig.md

// ORIG T 0x0863703a (_ZN26TimerCheckRecvPvpEndResult12dispatch_sigEiij)
int TimerCheckRecvPvpEndResult::dispatch_sig(int p1, int p2, unsigned int p3)
{
    PvP_Room* room = G_CGameManager()->GetPvp(p2, NULL, 0x3d);
    if (room == NULL)
        return 0;
    if (room->get_timer_key((TIMER_MESSAGE)0x3d) != p3)
        return 0;
    if (room->get_recv_pvp_end_result_count() != 0)
    {
        room->end_pvp_result();
        PacketGuard packet;
        room->make_state_info((char*)&packet);
        G_GameWorld()->send_all(packet);
        room->end_pvp();
        room->battle_reset();
    }
    return 1;
}

TimerCheckHumanCertifyTimeout::TimerCheckHumanCertifyTimeout() {}
TimerCheckHumanCertifyTimeout::~TimerCheckHumanCertifyTimeout() {}
int TimerCheckHumanCertifyTimeout::dispatch_sig(int p1, int p2, unsigned int p3)
{
    unsigned short local_24[10];
    CUser* user = G_CGameManager()->getUser(p1, 2);

    if (user == 0 || user->get_state() < 2)
    {
        return 0;
    }

    if (user->isHumanCertified() && user->isSendMailCertified())
    {
        return 1;
    }

    if ((unsigned int)user->getHumanCertifyTimerKey() > p3)
    {
        return 1;
    }

    unsigned int point = user->m_mcap.getPoint();
    user->m_historyLog.RequestCleanPad(point, '\\x03');

    for (int i = 0; i < 10; ++i)
    {
        local_24[i] = 0;
    }

    if (!user->checkHumanCertify(1, local_24))
    {
        if ((int)user->isSendMailCertified() == 1)
        {
            user->DisConnSig((DISCONN_SIG)0x34, true, 0);
        }
        else
        {
            user->setHumanCertified(true);
            user->SendMail(false);
            user->resetCleanpadFailCnt();
            user->resetHumanCertifyData();
        }
    }

    return 1;
}

TimerUpdateItemGenerateStatistics::TimerUpdateItemGenerateStatistics() {}
TimerUpdateItemGenerateStatistics::~TimerUpdateItemGenerateStatistics() {}
int TimerUpdateItemGenerateStatistics::dispatch_sig(int p1, int p2, unsigned int p3)
{
    WongWork::CItemGeneratorMgr* mgr = G_CDataManager()->getItemGenerator();
    mgr->onTimerEvent(0x6b);
    const int curSec = GlobalData::s_systemTime_.getCurSec();
    registNextTimer(curSec + 0x15180);
    return 1;
}

// ORIG T 0x08637318 _ZN33TimerUpdateItemGenerateStatistics15registNextTimerEl
void TimerUpdateItemGenerateStatistics::registNextTimer(long pointSec)
{
    std::tm timePoint;
    localtime_r(&pointSec, &timePoint);
    timePoint.tm_hour = 0x17;
    timePoint.tm_min = 0;
    timePoint.tm_sec = 0;
    const long base = mktime(&timePoint);
    int candidates[2];
    candidates[0] = static_cast<int>(base - GlobalData::s_systemTime_.getCurSec());
    candidates[1] = 1;
    const int delay = std::max(candidates[1], candidates[0]);
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x6b, delay, 0, 0);
}

TimerUpdatePvPGrade::TimerUpdatePvPGrade() {}
TimerUpdatePvPGrade::~TimerUpdatePvPGrade() {}
int TimerUpdatePvPGrade::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    // ORIG 0x86373c6：makeRequest() → getCurSec() → registNextTimer(sec+0x15180)。
    DB_UpdatePvPGrade::makeRequest();
    const int sec = GlobalData::s_systemTime_.getCurSec();
    registNextTimer(static_cast<long>(sec) + 0x15180);
    return 1;
}

// ORIG T 0x086373f2（objdump 转写，尾界 next symbol 0x86374b5）：
// channel_no!=1 直接返回；否则 localtime_r(&sec) 对齐当日 02:00:00，
// delay=max(target-curSec,1)，InsertTimer(OBJ_TYPE_2,0,0x6c,delay,0,0)。
void TimerUpdatePvPGrade::registNextTimer(long sec)
{
    if (G_CEnvironment()->get_channel_no() != 1)
        return;
    tm timeInfo;
    localtime_r(&sec, &timeInfo);
    timeInfo.tm_hour = 2;
    timeInfo.tm_min = 0;
    timeInfo.tm_sec = 0;
    sec = static_cast<long>(mktime(&timeInfo));   // ORIG mov %eax,0x8(%ebp) 回写参数槽
    const int curSec = GlobalData::s_systemTime_.getCurSec();
    int delay;
    if (sec - curSec > 0)
        delay = static_cast<int>(sec - curSec);
    else
        delay = 1;
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x6c, delay, 0, 0);
}

TimerDetectDisconnect::TimerDetectDisconnect() {}
TimerDetectDisconnect::~TimerDetectDisconnect() {}
int TimerDetectDisconnect::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x86374bc: GetInstanceDisconnectDetecter()->DoDetectingDisconnect() 为真时 registNextTimer(); return 1
    disconnect_detecter::CDisconnectDetecter* detecter =
        disconnect_detecter::GetInstanceDisconnectDetecter();
    if (detecter->DoDetectingDisconnect()) {
        registNextTimer();
    }
    return 1;
}

TimerCompleteLoadAssaultTimeout::TimerCompleteLoadAssaultTimeout() {}
TimerCompleteLoadAssaultTimeout::~TimerCompleteLoadAssaultTimeout() {}
int TimerCompleteLoadAssaultTimeout::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x863752c: GetInstanceAssaultMgr() -> OnTimeoutCompleteLoad(p2,p3), return 1
    pvp_assault::CAssaultMgr* mgr = (pvp_assault::CAssaultMgr*)pvp_assault::GetInstanceAssaultMgr();
    mgr->OnTimeoutCompleteLoad((unsigned int)p2, p3);
    return 1;
}

TimerConnectP2PAssaultTimeout::TimerConnectP2PAssaultTimeout() {}
TimerConnectP2PAssaultTimeout::~TimerConnectP2PAssaultTimeout() {}
int TimerConnectP2PAssaultTimeout::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x863759c: GetInstanceAssaultMgr() -> OnTimeoutConnectP2P(p2,p3), return 1
    pvp_assault::CAssaultMgr* mgr = (pvp_assault::CAssaultMgr*)pvp_assault::GetInstanceAssaultMgr();
    mgr->OnTimeoutConnectP2P((unsigned int)p2, p3);
    return 1;
}

TimerEndAssault::TimerEndAssault() {}
TimerEndAssault::~TimerEndAssault() {}
int TimerEndAssault::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x863760c: GetInstanceAssaultMgr() -> OnTimeoutEndAssault(p2,p3), return 1
    pvp_assault::CAssaultMgr* mgr = (pvp_assault::CAssaultMgr*)pvp_assault::GetInstanceAssaultMgr();
    mgr->OnTimeoutEndAssault((unsigned int)p2, p3);
    return 1;
}

TimerCloseAssault::TimerCloseAssault() {}
TimerCloseAssault::~TimerCloseAssault() {}
int TimerCloseAssault::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x863767c: GetInstanceAssaultMgr() -> OnTimeoutCloseAssault(p2,p3), return 1
    pvp_assault::CAssaultMgr* mgr = (pvp_assault::CAssaultMgr*)pvp_assault::GetInstanceAssaultMgr();
    mgr->OnTimeoutCloseAssault((unsigned int)p2, p3);
    return 1;
}

// ---- pvp_assault 超时定时器注册（ORIG T，四函数同构，仅 TIMER_MESSAGE 不同）----
// objdump 0x863755a/0x86375ca/0x863763a/0x86376aa：
//   G_TimerQueue()->InsertTimer(OBJ_TYPE_2, p1, msg, (int)sec, p2, 0)
void TimerCompleteLoadAssaultTimeout::registNextTimer(long sec, int p1, int p2)
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, p1,
                                (TIMER_MESSAGE)0x6e, static_cast<int>(sec),
                                p2, 0);
}

void TimerConnectP2PAssaultTimeout::registNextTimer(long sec, int p1, int p2)
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, p1,
                                (TIMER_MESSAGE)0x6f, static_cast<int>(sec),
                                p2, 0);
}

void TimerEndAssault::registNextTimer(long sec, int p1, int p2)
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, p1,
                                (TIMER_MESSAGE)0x70, static_cast<int>(sec),
                                p2, 0);
}

void TimerCloseAssault::registNextTimer(long sec, int p1, int p2)
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, p1,
                                (TIMER_MESSAGE)0x71, static_cast<int>(sec),
                                p2, 0);
}

TimerCompleteLoadAfterAssaultTimeout::TimerCompleteLoadAfterAssaultTimeout() {}
TimerCompleteLoadAfterAssaultTimeout::~TimerCompleteLoadAfterAssaultTimeout() {}
int TimerCompleteLoadAfterAssaultTimeout::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x86376ec: GetInstanceAssaultMgr() -> OnTimeoutCompleteLoadAfterAssault(p2,p3), return 1
    pvp_assault::CAssaultMgr* mgr = (pvp_assault::CAssaultMgr*)pvp_assault::GetInstanceAssaultMgr();
    mgr->OnTimeoutCompleteLoadAfterAssault((unsigned int)p2, p3);
    return 1;
}

TimerConnectP2PAfterAssaultTimeout::TimerConnectP2PAfterAssaultTimeout() {}
TimerConnectP2PAfterAssaultTimeout::~TimerConnectP2PAfterAssaultTimeout() {}
int TimerConnectP2PAfterAssaultTimeout::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x863775c: GetInstanceAssaultMgr() -> OnTimeoutConnectP2PAfterAssault(p2,p3), return 1
    pvp_assault::CAssaultMgr* mgr = (pvp_assault::CAssaultMgr*)pvp_assault::GetInstanceAssaultMgr();
    mgr->OnTimeoutConnectP2PAfterAssault((unsigned int)p2, p3);
    return 1;
}

// ORIG T 0x0863778a size 0x42：
//   G_TimerQueue()->InsertTimer(OBJ_TYPE 1, partyIdx, TIMER_MESSAGE 0x72,
//                               sec, key, 0)
// —— CloseAssault 后按队安排 ConnectP2PAfterAssault 超时重载。
void TimerConnectP2PAfterAssaultTimeout::registNextTimer(long sec,
                                                         int p1, int p2)
{
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, p1,
                                (TIMER_MESSAGE)0x72, (int)sec,
                                (unsigned int)p2, 0);
}

TimerDeathTower::TimerDeathTower() {}
TimerDeathTower::~TimerDeathTower() {}
int TimerDeathTower::dispatch_sig(int p1, int p2, unsigned int p3)
{
    WongWork::CDeathTower* tower = G_CGameManager()->getDeathTower(p1);
    if (tower == NULL) {
        return 0;
    }
    tower->onTimer(static_cast<TIMER_MESSAGE>(p3), p2);
    return 1;
}

TimerDeathTowerTopRankerReload::TimerDeathTowerTopRankerReload() {}
TimerDeathTowerTopRankerReload::~TimerDeathTowerTopRankerReload() {}
int TimerDeathTowerTopRankerReload::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    // ORIG 0x8637816：两阶段加载 TopRanker（false 增量 → true 全量），
    // 再把下轮触发排到次日 04:00 + rand(0..299) 分钟。
    DB_DeathTowerLoadTopRanker::makeRequest(false);
    DB_DeathTowerLoadTopRanker::makeRequest(true);
    registNextTimer(static_cast<long>(GlobalData::s_systemTime_.getCurSec()) + 0x15180);
    return 1;
}

TimerScheduleTenSecond::TimerScheduleTenSecond() {}
TimerScheduleTenSecond::~TimerScheduleTenSecond() {}
int TimerScheduleTenSecond::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    // ORIG 0x8637936：遍历 +0x22c 用户 map；get_state()>=2 且 getAntibotKey()>=0
    // 时经 s_antibotChecker_ 虚槽 +0x14（antibot 检查，签名 (void*, seq, 0, accId)）
    // 发起防机器人校验；循环完 registNextTimer(10) 返回 1。
    std::map<unsigned int, CUser*>& users = G_CGameManager()->getUserByAccIdMap();
    for (std::map<unsigned int, CUser*>::iterator it = users.begin();
         it != users.end(); ++it)
    {
        CUser* user = it->second;
        if (user->get_state() >= 2 && user->getAntibotKey() >= 0)
        {
            typedef void (*CheckFn)(void*, unsigned int, int, unsigned int);
            void* checker = GlobalData::s_antibotChecker_;
            CheckFn fn = *(CheckFn*)((*(void***)checker)[0x14 / 4]);
            fn(checker, G_CEnvironment()->getProcessSequence(), 0, user->get_acc_id());
        }
    }
    registNextTimer(10);
    return 1;
}

TimerFinishItemEffect::TimerFinishItemEffect() {}
TimerFinishItemEffect::~TimerFinishItemEffect() {}
// ---- TimerFinishItemEffect ----
// 报告：docs/class_func_reports/TimerFinishItemEffect/{dispatch_sig,_processSepecialEffectContinuousItem}.md
// 类对象仅 vptr；头声明区现缺私有 helper（见 NEEDS_LOCAL_STATIC）。

// ORIG T 0x08637aac (_ZN21TimerFinishItemEffect12dispatch_sigEiij)
int TimerFinishItemEffect::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, 0x3);
    if (user == NULL)
        return 0;
    if (user->get_state() <= 2)          // ORIG setle: state<=2 → return 0
        return 0;
    if (p3 != 0 &&
        static_cast<unsigned int>(user->getCurCharacNo()) != p3)
        return 0;

    const unsigned int curSec =
        static_cast<unsigned int>(GlobalData::s_systemTime_.getCurSec());
    const std::vector<ContinuousItemInfo>* items =
        user->GetCurCharacEffectItemListR();
    if (items == NULL)
        return 0;

    // 循环条件每次迭代重新取 size()（ORIG +0x290 seta 无符号比较）。
    for (unsigned int i = 0; i < static_cast<unsigned int>(items->size()); ++i)
    {
        const ContinuousItemInfo& entry = (*items)[i];
        if (entry.m_itemIdx != p2)       // 未命中该项：i++ 继续
            continue;

        PacketGuard packet;              // ORIG try 区 0x8637bb1..0x8637cfd
        packet.put_header(0, 0xae);
        packet.put_byte(1);
        packet.put_int(p2);
        packet.put_int(0);
        packet.finalize(true);

        if (entry.m_expireTime <= curSec)
        {
            // 已到期：立即结算并返回 Send 结果（ORIG esi=0 直通尾部返回 ebx）。
            _processSepecialEffectContinuousItem(user, p2);
            return user->Send(packet);
        }

        const unsigned int remain = entry.m_expireTime - curSec;
        if (!((remain < 6u) && (0x3cu < remain)))
        {
            // ORIG +0x1d2 重排程：OBJ_TYPE 0 / key=GetUID / msg 0x76 / delay=remain / p2 / characNo
            const unsigned int uid = user->GetUID();
            const unsigned int characNo =
                static_cast<unsigned int>(user->getCurCharacNo());
            G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)0,
                                        static_cast<int>(uid),
                                        (TIMER_MESSAGE)0x76,
                                        static_cast<int>(remain),
                                        p2,
                                        characNo);
            continue;                    // esi=1 → 回到 i++ 尾部继续遍历
        }

        // ORIG +0x227 死分支（remain<6 且 >60 同时成立，恒不可达，按汇编原样保留）。
        _processSepecialEffectContinuousItem(user, p2);
        return user->Send(packet);
    }
    return 1;                            // ORIG 遍历结束 ebx=1
}

// ORIG T 0x08637d64 (_ZN21TimerFinishItemEffect36_processSepecialEffectContinuousItemEP5CUserj)
// ⚠ 私有方法，TimerDispatcher.h 该类内需补声明
//   `void _processSepecialEffectContinuousItem(CUser*, unsigned int);`（NEEDS_LOCAL_STATIC）
void TimerFinishItemEffect::_processSepecialEffectContinuousItem(CUser* user,
                                                                 unsigned int itemIdx)
{
    if (itemIdx != 0x1cd1)
        return;
    user->SetExpAffectItemEffect(false, 0.0f);
}

TimerStartMatching::TimerStartMatching() {}
TimerStartMatching::~TimerStartMatching() {}
// ORIG：PvP_Room+0x6e4 视图（PvP_Room.h 该槽现标 int；ORIG 实为 create_room
// 时 CMatchingSystem::GetMatchingSystem 登记的 IMatch 匹配器指针），
// 与 GmCmds_M.cpp CPvPStartCmd 的 PvpRoomMatcherSlot 同款模式。
struct TimerStartMatchingMatcherSlot
{
    char m_pad[0x6e4];
    IMatch* m_matcher;
};

int TimerStartMatching::dispatch_sig(int p1, int p2, unsigned int p3)
{
    PvP_Room* room = G_CGameManager()->GetPvp(p1, (CUser*)0, 0x79);
    if (room == NULL) {
        return 0;
    }
    IMatch* matcher = ((TimerStartMatchingMatcherSlot*)room)->m_matcher;
    if (matcher == NULL) {
        return 0;
    }
    matcher->OnMatchRoom(room, (int)p3);   // ORIG 虚表 +0x38
    return 1;
}

// ORIG T 0x08637e14
void TimerStartMatching::registNextTimer(long sec, int p1, int p2)
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, p1,
                                (TIMER_MESSAGE)0x79, (int)sec, p2, 0);
}

TimerGlobalEffectItemBegin::TimerGlobalEffectItemBegin() {}
TimerGlobalEffectItemBegin::~TimerGlobalEffectItemBegin() {}
int TimerGlobalEffectItemBegin::dispatch_sig(int p1, int p2, unsigned int p3)
{
    GlobalData::s_globalEffectManager_->beginReservedEffect(p3);
    return 1;
}

TimerGlobalEffectItemTimeout::TimerGlobalEffectItemTimeout() {}
TimerGlobalEffectItemTimeout::~TimerGlobalEffectItemTimeout() {}
int TimerGlobalEffectItemTimeout::dispatch_sig(int p1, int p2, unsigned int p3)
{
    const unsigned int curSec = GlobalData::s_systemTime_.getCurSec();
    GlobalData::s_globalEffectManager_->checkTimeout(p3, curSec);
    return 1;
}

// ORIG T 0x08637ef6 _ZN28TimerGlobalEffectItemTimeout11registTimerEjj
void TimerGlobalEffectItemTimeout::registTimer(unsigned int type, unsigned int timeoutSec)
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x78, static_cast<int>(timeoutSec),
                                static_cast<int>(type), 0);
}

TimerVendingMachineItemNotify::TimerVendingMachineItemNotify() {}
TimerVendingMachineItemNotify::~TimerVendingMachineItemNotify() {}
// ORIG TimerVendingMachineItemNotify::dispatch_sig @ 0x8637f40 (0x134)
int TimerVendingMachineItemNotify::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, 2);   // ORIG 第二实参为常量 2
    if (user != 0)
    {
        PacketGuard packet;
        packet.put_header(0, 0x56);
        packet.put_byte(3);
        packet.put_byte(1);
        packet.put_short(user->get_unique_id() & 0xffff);   // movzwl 零扩展
        packet.put_item_idx(p2);
        packet.put_byte(0);
        packet.put_int(p3);
        packet.finalize(true);
        G_GameWorld()->send_all(packet);
        return 1;
    }
    return 0;
}

TimerConnectMonitorServer::TimerConnectMonitorServer() {}
TimerConnectMonitorServer::~TimerConnectMonitorServer() {}
// ===== TimerConnectMonitorServer =====
// dispatch_sig ORIG _ZN25TimerConnectMonitorServer12dispatch_sigEiij @0x086380bc (0xe3B)
// registNextTimer ORIG _ZN25TimerConnectMonitorServer15registNextTimerElii @0x086381a0 (0x41B)

int TimerConnectMonitorServer::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int p3)
{
    Stream* stream = GlobalData::s_stream_pool->Acquire("TimerDispatcher.cpp", 0xeaa);
    CStreamGuard guard(stream, true);

    // ORIG +0x42: CStreamGuard::operator<<(0x121)
    guard << 0x121;
    // ORIG +0x6d: CStreamGuard::operator<<(-1)
    guard << -1;

    // ORIG +0x78/+0x80: CStreamGuard::GetInBuffer<Packet_Tcp_Server_Connect>
    Packet_Tcp_Server_Connect* pkt = guard.GetInBuffer<Packet_Tcp_Server_Connect>();
    // ORIG +0x90: mov %dl,0xa(%eax) —— 尾参低字节写至缓冲 +0xa
    ((unsigned char*)pkt)[0xa] = (unsigned char)p3;

    // ORIG +0xaa: MsgQueueMgr::put(s_msgq_mgr, 1, guard)
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    // 析构由作用域自然产生（ORIG +0xba ~CStreamGuard）
    return 1;
}

// ORIG T 0x086381a0：static 注册链，与其它 registNextTimer 族同构
void TimerConnectMonitorServer::registNextTimer(long sec, int p1, int p2)
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, p1,
                                (TIMER_MESSAGE)0x7a, static_cast<int>(sec),
                                p2, 0);
}

TimerAssaultSuperPowerState::TimerAssaultSuperPowerState() {}
TimerAssaultSuperPowerState::~TimerAssaultSuperPowerState() {}
int TimerAssaultSuperPowerState::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, 3);
    if (user == NULL) {
        return 0;
    }
    // asm：p3==0 或 getCurCharacNo()==p3 时 al=0 → 执行下方调用并返回 1；
    // 否则 al=1 → 返回 0。
    bool mismatch = false;
    if (p3 != 0 && (unsigned int)user->getCurCharacNo() != p3) {
        mismatch = true;
    }
    if (!mismatch) {
        user->SetCurCharacSuperState(false);
        pvp_assault::CAssaultPlace::SendUserState(user);
        return 1;
    }
    return 0;
}

TimerNPCMoodChange::TimerNPCMoodChange() {}
TimerNPCMoodChange::~TimerNPCMoodChange() {}
int TimerNPCMoodChange::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x86381e2：空函数（纯 ret），恒返 1
    return 1;
}

// ORIG T 0x086381ec (_ZN18TimerNPCMoodChange15registNextTimerEl)：

TimerElevatorDungeon::TimerElevatorDungeon() {}
TimerElevatorDungeon::~TimerElevatorDungeon() {}
int TimerElevatorDungeon::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == NULL) {
        return 0;
    }
    if (!party->check_timer_key((TIMER_MESSAGE)0x28, p2)) {
        return 0;
    }
    // ORIG：CBattle_Field 嵌于 party+0xb24。
    reinterpret_cast<CBattle_Field*>(&party->m_padBattleField1)
        ->check_elevator_timer();
    return 1;
}

TimerRefreshPowerWarProcessInfo::TimerRefreshPowerWarProcessInfo() {}
TimerRefreshPowerWarProcessInfo::~TimerRefreshPowerWarProcessInfo() {}
int TimerRefreshPowerWarProcessInfo::dispatch_sig(int p1, int p2, unsigned int p3)
{
    if (GlobalData::s_power_manager->IsPowerWarEventOn()) {
        GameWorld* world = G_GameWorld();
        world->OnRefreshPowerWarProcessInfo();
        // dm+0xa658 = m_powerParameter.m_refreshProcessInfo(+0x30)；ORIG 按 8 字节
        // long 读取（覆盖 +0x30..+0x37 连续两 int），保持块读语义。
        registNextTimer(*reinterpret_cast<long*>(
            &G_CDataManager()->m_powerParameter.m_refreshProcessInfo));
    }
    return 1;
}

// ORIG T 0x08638316 _ZN31TimerRefreshPowerWarProcessInfo15registNextTimerEl
void TimerRefreshPowerWarProcessInfo::registNextTimer(long sec)
{
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)0, 0,
                                (TIMER_MESSAGE)0x7b, static_cast<int>(sec), 0, 0);
}

// ---- TimerGeneratePowerUp ----
// 报告：docs/class_func_reports/TimerGeneratePowerUp/{dispatch_sig,generatePowerUp,registNextTimer,sendPowerUpInfo}.md
// ctor/dtor 反汇编：ORIG 0x0863cc5a（TimerSig + 2×vector + vtable 0x8ce89e8）/
//   0x0863f156（D1）/ 0x0863f1e0（D0）。dispatcher 槽 +0x1f0（ORIG ctor 0x086321d6）。
// 数据字段（CDataManager.m_powerParameter 相对 +0xa628）：
//   m_powerUpGeneratePeriod(+0x48→dm+0xa670)、m_powerUpRatio(+0x1c→dm+0xa644)、
//   m_powerUpPointMod(+0xa8→dm+0xa6d0)。

TimerGeneratePowerUp::TimerGeneratePowerUp() {}
TimerGeneratePowerUp::~TimerGeneratePowerUp() {}

int TimerGeneratePowerUp::dispatch_sig(int p1, int p2, unsigned int p3)
{
    if (!GlobalData::s_power_manager->IsPowerWarEventOn())
        return 1;

    GlobalData::s_power_manager->SetPowerUpRemainTime(
        G_CDataManager()->m_powerParameter.m_powerUpGeneratePeriod * 0x3c);
    GlobalData::s_power_manager->SetPowerUpTimerPeriod(0);
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x7d, 5, 0, 0);
    registNextTimer(G_CDataManager()->m_powerParameter.m_powerUpGeneratePeriod * 0x3c);
    G_GameWorld()->CancelPowerUp();
    m_team1Users.clear();
    m_team2Users.clear();

    const double ratio =
        static_cast<double>(G_CDataManager()->m_powerParameter.m_powerUpRatio) / 100.0;  // /_DAT_08ce8590(=100.0)
    const int team1Count = GlobalData::s_power_manager->GetPowerUserCount(1);
    const int team2Count = GlobalData::s_power_manager->GetPowerUserCount(2);
    const int team1ChannelPoint = GlobalData::s_power_manager->GetPowerWarPointInChannel(1);
    const int team2ChannelPoint = GlobalData::s_power_manager->GetPowerWarPointInChannel(2);
    const int team1Point = GlobalData::s_power_manager->GetPowerWarPoint((ENUM_POWER_SIDE_TYPE)1);
    const int team2Point = GlobalData::s_power_manager->GetPowerWarPoint((ENUM_POWER_SIDE_TYPE)2);
    const int mod = G_CDataManager()->m_powerParameter.m_powerUpPointMod;  // dm+0xa6d0
    if (mod == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__,
                       0xff4, 0);
        trace("[PowerWar] PowerUpMod is 0");
        sendPowerUpInfo(0, 0);
        return 1;
    }

    double team1Ratio = static_cast<double>(team2Point - team1Point) / mod;  // local_24
    double team2Ratio = static_cast<double>(team1Point - team2Point) / mod;  // local_1c
    if (0.9 < team1Ratio)
        team1Ratio = 0.9;
    if (team1Ratio < -0.9)
        team1Ratio = -0.9;
    if (0.9 < team2Ratio)
        team2Ratio = 0.9;
    if (team2Ratio < -0.9)
        team2Ratio = -0.9;
    if (team1ChannelPoint == 0 || team2ChannelPoint == 0)
    {
        sendPowerUpInfo(0, 0);
        return 1;
    }

    int team1PowerUp = static_cast<int>((team1Ratio + 1.0) *
                                        (static_cast<double>(team2ChannelPoint) / team1ChannelPoint) *
                                        team2Count * ratio);
    int team2PowerUp = static_cast<int>((team2Ratio + 1.0) *
                                        (static_cast<double>(team1ChannelPoint) / team2ChannelPoint) *
                                        team1Count * ratio);
    G_GameWorld()->GetPowerUpDomain(m_team1Users, m_team2Users);
    if (team1PowerUp > static_cast<int>(m_team1Users.size()))
        team1PowerUp = static_cast<int>(m_team1Users.size());
    if (team2PowerUp > static_cast<int>(m_team2Users.size()))
        team2PowerUp = static_cast<int>(m_team2Users.size());
    generatePowerUp(m_team1Users, team1PowerUp);
    generatePowerUp(m_team2Users, team2PowerUp);
    sendPowerUpInfo(team1PowerUp, team2PowerUp);
    return 1;
}

// ORIG T 0x08638960 (_ZN20TimerGeneratePowerUp15generatePowerUpERSt6vectorIP5CUserSaIS2_EEi)
void TimerGeneratePowerUp::generatePowerUp(std::vector<CUser*>& users, int count)
{
    const int total = static_cast<int>(users.size());
    int selected = 0;
    int loopCnt = 0;
    std::vector<CUser*> buf;
    while (count > selected && loopCnt <= 10000)
    {
        CUser* picked = users[rand() % total];
        if (picked->IsPowerUp() != 1)
        {
            picked->SetPowerUp(true);
            buf.push_back(picked);
            ++selected;
        }
        ++loopCnt;
    }
    if (loopCnt > 10000)
    {
        cMyTrace trace(__PRETTY_FUNCTION__,
                       0x104e, 5);
        trace("Generate Power Up Party Failure");
    }
    G_GameWorld()->SendPowerUpList(ENUM_POWER_UP_ACTIVATE_TYPE_ACTIVATE, buf);
}

// ORIG T 0x08638aac (_ZN20TimerGeneratePowerUp15sendPowerUpInfoEii)
void TimerGeneratePowerUp::sendPowerUpInfo(int team1Count, int team2Count)
{
    PacketGuard packet;
    packet.put_header(0, 0xdd);
    packet.put_int(team1Count);
    packet.put_int(team2Count);
    packet.finalize(true);
    G_GameWorld()->send_to_vill(7, packet);
}

// ORIG T 0x08638b5c (_ZN20TimerGeneratePowerUp15registNextTimerEl)
void TimerGeneratePowerUp::registNextTimer(long sec)
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x7c, static_cast<int>(sec), 0, 0);
}

	// ---- TimerGeneratePowerUpNotiTime ----
	// 报告：docs/class_func_reports/TimerGeneratePowerUpNotiTime/{dispatch_sig,registNextTimer,send_noti_time}.md

TimerGeneratePowerUpNotiTime::TimerGeneratePowerUpNotiTime() {}
TimerGeneratePowerUpNotiTime::~TimerGeneratePowerUpNotiTime() {}


// CDataManager+0xa648 为 powerup 通知时间表 std::vector<int>，即
// m_powerParameter.m_vec20（+0xa628+0x20）。
int TimerGeneratePowerUpNotiTime::dispatch_sig(int p1, int p2, unsigned int p3)
{
    std::vector<int>* table = &G_CDataManager()->m_powerParameter.m_vec20;
    const int cnt = static_cast<int>(table->size());
    CPowerManager* pm = GlobalData::s_power_manager;
    int remain = pm->GetPowerUpRemainTime();
    unsigned int period = static_cast<unsigned int>(pm->GetPowerUpTimerPeriod());
    bool active = true;
    if (!pm->IsPowerWarEventOn() || cnt == 0)
        active = false;
    if (active && static_cast<int>(period) < cnt)
    {
        remain -= 5;
        pm->SetPowerUpRemainTime(remain);
        const int notiSec = (*table)[period];
        if (remain <= notiSec)
        {
            send_noti_time(notiSec);
            pm->SetPowerUpTimerPeriod(static_cast<int>(period) + 1);
            if (cnt <= static_cast<int>(period) + 1)
            {
                active = false;
                pm->SetPowerUpTimerPeriod(0);
            }
        }
    }
    if (active)
        registNextTimer(5);
    return 1;
}

// ORIG T 0x0863847e (_ZN28TimerGeneratePowerUpNotiTime15registNextTimerEl)
void TimerGeneratePowerUpNotiTime::registNextTimer(long sec)
{
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x7d,
                                sec, 0, 0);
}

// ORIG T 0x086384c2 (_ZN28TimerGeneratePowerUpNotiTime14send_noti_timeEi)
void TimerGeneratePowerUpNotiTime::send_noti_time(int sec)
{
    PacketGuard packet;
    packet.put_header(0, 0xdc);
    packet.put_int(sec);
    packet.finalize(true);
    G_GameWorld()->send_to_vill(7, packet);
}

TimerCheckPowerWarAbuser::TimerCheckPowerWarAbuser() {}
TimerCheckPowerWarAbuser::~TimerCheckPowerWarAbuser() {}
int TimerCheckPowerWarAbuser::dispatch_sig(int p1, int p2, unsigned int p3)
{
    if (GlobalData::s_power_manager->IsPowerWarEventOn()) {
        GameWorld* world = G_GameWorld();
        world->OnBanAbuserFromPowerWar();
        registNextTimer(0x3c);
    }
    return 1;
}

// ORIG T 0x08638294 _ZN24TimerCheckPowerWarAbuser15registNextTimerEl
void TimerCheckPowerWarAbuser::registNextTimer(long sec)
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x80, static_cast<int>(sec), 0, 0);
}

TimerSetPunishUser::TimerSetPunishUser() {}
TimerSetPunishUser::~TimerSetPunishUser() {}
int TimerSetPunishUser::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, 2);   // ORIG 第二实参为常量 2
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
    CUser* user = G_CGameManager()->getUser(p1, 2);   // ORIG 第二实参为常量 2
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
    CUser* user = G_CGameManager()->getUser(p1, 2);   // ORIG 第二实参为常量 2
    if (user == 0 || user->getCurCharacR() == 0 ||
        static_cast<unsigned int>(user->GetRevivalTime()) != p3)
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
    CUser* user = G_CGameManager()->getUser(p1, 2);   // ORIG 第二实参为常量 2
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
int TimerVillageAttackedPlayTimeout::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == NULL) {
        return 0;
    }
    if (!party->check_timer_key((TIMER_MESSAGE)0x27, p2)) {
        return 0;
    }
    // ORIG：party+0xc4c == CBattle_Field 起点偏移 0x128 = m_villageMonsterId。
    CBattle_Field* field = reinterpret_cast<CBattle_Field*>(&party->m_padBattleField1);
    if ((unsigned short)field->m_villageMonsterId != p3) {
        return 0;
    }
    party->OnFinishVillageMonsterFighting((CUser*)0, true);
    return 1;
}

TimerItemUnlockWaiting::TimerItemUnlockWaiting() {}
TimerItemUnlockWaiting::~TimerItemUnlockWaiting() {}
int TimerItemUnlockWaiting::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, 2);
    if (user == NULL) {
        return 0;
    }
    if (user->getCurCharacR() == NULL) {
        return 0;
    }
    if ((unsigned int)user->getCurCharacNo() != p3) {
        return 0;
    }
    user->OnItemUnlockWaitTimeout(p2, p3);
    return 1;
}

// ===== TimerUpdateCompoundEmblemStatics =====
TimerUpdateCompoundEmblemStatics::TimerUpdateCompoundEmblemStatics() {}
TimerUpdateCompoundEmblemStatics::~TimerUpdateCompoundEmblemStatics() {}
int TimerUpdateCompoundEmblemStatics::dispatch_sig(int p1, int p2, unsigned int p3)
{
    if (getCompoundEmblemStatistic()->getTotalCount() != 0)
    {
        const unsigned int grade6 = static_cast<unsigned int>(getCompoundEmblemStatistic()->getGradeCount(6));
        const unsigned int grade5 = static_cast<unsigned int>(getCompoundEmblemStatistic()->getGradeCount(5));
        const unsigned int grade4 = static_cast<unsigned int>(getCompoundEmblemStatistic()->getGradeCount(4));
        const unsigned int grade3 = static_cast<unsigned int>(getCompoundEmblemStatistic()->getGradeCount(3));
        const unsigned int grade2 = static_cast<unsigned int>(getCompoundEmblemStatistic()->getGradeCount(2));
        const unsigned int grade1 = static_cast<unsigned int>(getCompoundEmblemStatistic()->getGradeCount(1));
        const unsigned int grade0 = static_cast<unsigned int>(getCompoundEmblemStatistic()->getGradeCount(0));
        {
            cMyTrace trace(__PRETTY_FUNCTION__,
                           0x11aa, 0);
            trace("DNF Compound Emblem Statistics (grade 0 : %d)(grade 1 : %d)(grade 2 : %d)(grade 3 : %d)(grade 4 : %d)(grade 5 : %d)(grade 6 : %d)",
                  grade0, grade1, grade2, grade3, grade4, grade5, grade6);
        }
        const unsigned int dbGrade6 = static_cast<unsigned int>(getCompoundEmblemStatistic()->getGradeCount(6));
        const unsigned int dbGrade5 = static_cast<unsigned int>(getCompoundEmblemStatistic()->getGradeCount(5));
        const unsigned int dbGrade4 = static_cast<unsigned int>(getCompoundEmblemStatistic()->getGradeCount(4));
        const unsigned int dbGrade3 = static_cast<unsigned int>(getCompoundEmblemStatistic()->getGradeCount(3));
        const unsigned int dbGrade2 = static_cast<unsigned int>(getCompoundEmblemStatistic()->getGradeCount(2));
        const unsigned int dbGrade1 = static_cast<unsigned int>(getCompoundEmblemStatistic()->getGradeCount(1));
        const unsigned int dbGrade0 = static_cast<unsigned int>(getCompoundEmblemStatistic()->getGradeCount(0));
        DB_UpdateCompoundEmblemStatics::makeRequest(
            *(const int*)((const char*)G_CEnvironment() + 0x1b0),
            dbGrade0, dbGrade1, dbGrade2, dbGrade3, dbGrade4, dbGrade5, dbGrade6);
    }
    registNextTimer();
    getCompoundEmblemStatistic()->clear();
    return 1;
}

// ORIG T _ZN32TimerUpdateCompoundEmblemStatics15registNextTimerEv @0x0863939a
int TimerUpdateCompoundEmblemStatics::registNextTimer()
{
    const time_t now = static_cast<time_t>(GlobalData::s_systemTime_.getCurSec());
    tm t;
    localtime_r(&now, &t);
    const int curHour = t.tm_hour;
    t.tm_hour = 5;
    t.tm_min = 0;
    t.tm_sec = 0;
    t.tm_min = *(const int*)((const char*)G_CEnvironment() + 0x1b0) % 0x1e;
    int target = static_cast<int>(mktime(&t));
    if (t.tm_hour <= curHour)
        target += 0x15180;
    const int delay = target - static_cast<int>(now);
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x89,
                                delay, 0, 0);
    return 1;
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
int Timer_Security_NonClient_CheckResponse::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x86394f0：call _NS_NonClient_CheckResponse(p1,p2,p3) 后 leave/ret（eax 未定）。
    _NS_NonClient_CheckResponse(p1, p2, static_cast<int>(p3));
}

Timer_Midnight::Timer_Midnight() {}
Timer_Midnight::~Timer_Midnight() {}
int Timer_Midnight::dispatch_sig(int p1, int p2, unsigned int p3)
{
    GameWorld* world = G_GameWorld();
    world->ResetMidnight();
    GlobalData::s_onlinePreliminaryTeamMgr->ResetDailyMidnight();
    const bool registered = RegistNextTimer();
    if (!registered) {
        return 0;
    }
    return 1;
}

// ORIG T 0x08639554 _ZN14Timer_Midnight15RegistNextTimerEv
bool Timer_Midnight::RegistNextTimer()
{
    const int nowSec = GlobalData::s_systemTime_.getCurSec();
    const time_t midNow = static_cast<time_t>(nowSec);
    std::tm timePoint;
    std::tm* pt = localtime_r(&midNow, &timePoint);
    const int hourNow = pt->tm_hour;
    const int minNow = pt->tm_min;
    const int secNow = pt->tm_sec;
    cMyTrace trace(__PRETTY_FUNCTION__, 0x1293, 0);
    trace("Reset all user's fatigue!(%02d:%02d:%02d)", hourNow, minNow, secNow);
    pt->tm_hour = 0;
    pt->tm_min = 0;
    pt->tm_sec = 0;
    const long midnight = mktime(pt);
    const int nextAt = static_cast<int>(midnight + 0x15180);
    const int delay = nextAt - nowSec;
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x8d, delay, 0, 0);
    return true;
}

TimerEndPowerWarVictoriousReward::TimerEndPowerWarVictoriousReward() {}
TimerEndPowerWarVictoriousReward::~TimerEndPowerWarVictoriousReward() {}
int TimerEndPowerWarVictoriousReward::dispatch_sig(int p1, int p2, unsigned int p3)
{
    GlobalData::s_event_manager->TriggerEventEnd(0x35);
    return 1;
}

Timer_TenMinuteSchedule::Timer_TenMinuteSchedule() {}
Timer_TenMinuteSchedule::~Timer_TenMinuteSchedule() {}
int Timer_TenMinuteSchedule::dispatch_sig(int /*p1*/, int p2, unsigned int /*p3*/)
{
    // ORIG 0x8639668：检查第二参数（0x10(%ebp)）：==0 → InsertValueStatistic +
    // RegistNextTimer(1)；==1 → InsertCirculationStatistic + RegistNextTimer(0)；
    // 其他值两分支都不执行。RegistNextTimer 恒返回 1，ORIG 返回 !regResult，
    // 即执行分支返回 1、未执行返回 0。
    if (p2 == 0)
    {
        GetInstanceValueStatistic()->InsertValueStatistic();
        RegistNextTimer(1);
        return 1;
    }
    if (p2 == 1)
    {
        GetInstanceCirculationStatistic()->InsertCirculationStatistic();
        RegistNextTimer(0);
        return 1;
    }
    return 0;
}

Timer_UpdateOnlinePreliminaryTeam::Timer_UpdateOnlinePreliminaryTeam() {}
Timer_UpdateOnlinePreliminaryTeam::~Timer_UpdateOnlinePreliminaryTeam() {}
int Timer_UpdateOnlinePreliminaryTeam::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x08639700：FindTeam/RegistNextTimer 均用 p2（0x10(%ebp)），非 p3。
    (void)p1;
    (void)p3;
    online_preliminary::COnlinePreliminaryTeam* team =
        GlobalData::s_onlinePreliminaryTeamMgr->FindTeam(p2);
    if (team == NULL) {
        return 0;
    }
    team->saveData();
    const bool registered = RegistNextTimer(p2);
    if (!registered) {
        return 0;
    }
    return 1;
}

// ORIG T 0x0863977e _ZN33Timer_UpdateOnlinePreliminaryTeam15RegistNextTimerEi
bool Timer_UpdateOnlinePreliminaryTeam::RegistNextTimer(int team)
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x8f, 600, team, 0);
    return true;
}

Timer_SecretShopStatistic::Timer_SecretShopStatistic() {}
Timer_SecretShopStatistic::~Timer_SecretShopStatistic() {}
int Timer_SecretShopStatistic::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x86397ce: secretshop::CSecretShop::SendSecretShopStatistic(s_secret_shop); RegistNextTimer(); return 1
    GlobalData::s_secret_shop->SendSecretShopStatistic();
    RegistNextTimer();
    return 1;
}

// ORIG T 0x086397ee (_ZN25Timer_SecretShopStatistic15RegistNextTimerEv)：
// G_TimerQueue()->InsertTimer(2, 0, 0x90, 0x3c, 0, 0)。
void Timer_SecretShopStatistic::RegistNextTimer()
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x90, 0x3c, 0, 0);
}

Timer_DailySchedule::Timer_DailySchedule() {}
Timer_DailySchedule::~Timer_DailySchedule() {}
int Timer_DailySchedule::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x812c920: CDailyScheduleManager::CheckAndSendToAllUserSchedule(s_DailyScheduleManager); RegistNextTimer(); return 1
    GlobalData::s_DailyScheduleManager->CheckAndSendToAllUserSchedule();
    RegistNextTimer();
    return 1;
}

Timer_ConnectP2PPvPTimeout::Timer_ConnectP2PPvPTimeout() {}
Timer_ConnectP2PPvPTimeout::~Timer_ConnectP2PPvPTimeout() {}
int Timer_ConnectP2PPvPTimeout::dispatch_sig(int p1, int p2, unsigned int p3)
{
    PvP_Room* room = G_CGameManager()->GetPvp(p1, (CUser*)0, 0x3b);
    if (room == NULL) {
        return 0;
    }
    if (!room->CheckTimerKey((TIMER_MESSAGE)0x3b, p2)) {
        return 0;
    }
    room->OnConnectP2PPvPTimeout();
    return 1;
}

Timer_RoutingItemWait::Timer_RoutingItemWait() {}
Timer_RoutingItemWait::~Timer_RoutingItemWait() {}
int Timer_RoutingItemWait::dispatch_sig(int p1, int p2, unsigned int /*p3*/)
{
    // ORIG 0x86398a8：GetParty(p1) → check_timer_key(0x2b,p2) 判真 → ProcessTimeOut。
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
        return 0;
    if (party->check_timer_key((TIMER_MESSAGE)0x2b, p2))
        G_CGameManager()->GetSpecialItemRoutingManager()->ProcessTimeOut(party);
    // GetSpecialItemRoutingManager 返回值域：CGameManager.h 仅前向，见 TimerBatch.md
    return 1;
}

Timer_PcRoomAuthRetry::Timer_PcRoomAuthRetry() {}
Timer_PcRoomAuthRetry::~Timer_PcRoomAuthRetry() {}
// ===== Timer_PcRoomAuthRetry =====
// dispatch_sig ORIG _ZN21Timer_PcRoomAuthRetry12dispatch_sigEiij @0x08639924 (0xacB)
// registNextTimer ORIG _ZN21Timer_PcRoomAuthRetry15registNextTimerEliij @0x086399d0 (0x41B)

int Timer_PcRoomAuthRetry::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, 2);
    if (user == 0)
    {
        return 0;
    }
    // ORIG +0x38/+0x40: get_unique_id() 低 16 位与 p2 比较
    if ((unsigned short)user->get_unique_id() != p2)
    {
        return 0;
    }
    // ORIG +0x57: getCurCharacR() 判空
    if (user->getCurCharacR() == 0)
    {
        return 0;
    }
    // ORIG +0x72: getCurCharacNo() 与 p3 比较
    if (user->getCurCharacNo() != p3)
    {
        return 0;
    }
    // ORIG +0x8e..+0xa5: getPcRoomAuth(); xor $1; 为真即 !=1 时重发登录包
    if ((unsigned char)(user->getPcRoomAuth() ^ 1) != 0)
    {
        user->SendLoginToPCRoomServer();
    }
    return 1;
}

// ORIG T 0x086399d0：static，四参全转发（无 this 使用）
void Timer_PcRoomAuthRetry::registNextTimer(long sec, int p1, int p2,
                                            unsigned int p3)
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, p1,
                                (TIMER_MESSAGE)0x92, static_cast<int>(sec),
                                p2, p3);
}

Timer_DungeonInoutOpenTime::Timer_DungeonInoutOpenTime() {}
Timer_DungeonInoutOpenTime::~Timer_DungeonInoutOpenTime() {}

bool Timer_DungeonInoutOpenTime::checkDungeonInoutTime(int dungeonIdx)
{
    std::map<int, STDungeonInoutTime>& dungeonInout =
        G_CDataManager()->m_serverParameter.m_dungeonInoutc8;
    std::map<int, STDungeonInoutTime>::iterator it = dungeonInout.find(dungeonIdx);
    if (it == dungeonInout.end())
        return false;
    const time_t nowSec = static_cast<time_t>(GlobalData::s_systemTime_.getCurSec());
    struct tm timeBuf;
    struct tm* localTm = localtime_r(&nowSec, &timeBuf);
    const int hour = localTm->tm_hour;
    const int min = localTm->tm_min;
    const STDungeonInoutTime& w = it->second;
    return hour >= w.m_startHour && hour <= w.m_endHour
        && min >= w.m_startMin && min <= w.m_endMin;
}

// Timer_DungeonInoutOpenTime::dispatch_sig ORIG T 0x8639e0a (0x3c5)
int Timer_DungeonInoutOpenTime::dispatch_sig(int p1, int p2, unsigned int p3)
{
    const int curT = static_cast<int>(GlobalData::s_systemTime_.getCurSec());  // ORIG -0x78(%ebp)
    const time_t nowSec = static_cast<time_t>(curT);
    struct tm timeBuf;                                 // ORIG -0xac(%ebp)
    struct tm* localTm = localtime_r(&nowSec, &timeBuf); // ORIG ptr → -0x20(%ebp)
    const int hour = localTm->tm_hour;                 // ORIG -0x18(%ebp)（tm+0x8）
    const int min = localTm->tm_min;                   // ORIG -0x14(%ebp)（tm+0x4）
    if (p3 == 0)
    {
        return 0;
    }

    std::map<int, STDungeonInoutTime>& dungeonInout =
        G_CDataManager()->m_serverParameter.m_dungeonInoutc8;  // ORIG dm+0x130
    std::map<int, STDungeonInoutTime>::iterator it =
        dungeonInout.find((int)p3);                            // key=p3（-0x7c）
    if (it != dungeonInout.end())
    {
        // second 自节点数据 +0x04 起：endHour@+8 / startHour@+4 /
        // startMin@+c / endMin@+10（对照 asm 取值偏移）
        if (it->second.m_endHour < hour)
        {
            localTm->tm_hour = it->second.m_startHour;   // ORIG 写 tm+0x8
            localTm->tm_min = it->second.m_startMin;     // ORIG 写 tm+0x4
            localTm->tm_sec = 0;
            int nextOpenT = mktime(localTm) + 0x15180;   // +86400 秒（次日同刻）
            int delta = nextOpenT - curT;                // ORIG -0xc(%ebp)
            if (delta < 0)
            {
                cMyTrace trace(__PRETTY_FUNCTION__,
                               0x1445, 0);
                trace("ULTIMATE_LOG : TIMER OFF!!");
                return 0;
            }
            G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                        (TIMER_MESSAGE)0x93, delta, 0, p3);
            cMyTrace trace(__PRETTY_FUNCTION__,
                           0x1454, 0);
            trace("ULTIMATE_LOG : NEXT DAY TIMER ON!!");
            return 1;
        }
        localTm->tm_hour = hour;                          // ORIG 写 tm+0x8
        localTm->tm_min = it->second.m_endMin;            // ORIG 写 tm+0x4（v+0x10）
        localTm->tm_sec = 0x3b;
        int checkT = mktime(localTm);                     // ORIG -0x1c(%ebp)
        int delta = checkT - curT;                        // ORIG -0x10(%ebp)
        if (delta < 0)
        {
            delta = 0;
        }
        cMyTrace trace(__PRETTY_FUNCTION__,
                       0x1462, 0);
        trace("DELTA %d , CHECK_T %d , CUR_T %d\\n", delta, checkT, curT);
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                    (TIMER_MESSAGE)0x94, delta, 0, p3);
        cMyTrace traceClose(__PRETTY_FUNCTION__,
                            0x146e, 0);
        traceClose("ULTIMATE_LOG : ULTIMATE DUNGEON CLOSE TIMER ON!!");
        if (!GlobalData::s_power_manager->IsPowerWarEventOn() &&
            !GlobalData::s_villageMonsterMgr->OnEvent())
        {
            // ORIG 0826868c / 082687fc：dm+0x68（m_serverParameter）上的
            // isDungeonOpen(p3)/setDungeonOpen(p3,true)
            if (!G_CDataManager()->m_serverParameter.isDungeonOpen((int)p3))
            {
                G_CDataManager()->m_serverParameter.setDungeonOpen((int)p3, true);
                G_GameWorld()->send_all_dungeon_inout_message((int)p3);
            }
            cMyTrace traceOpen(__PRETTY_FUNCTION__,
                               0x1478, 0);
            traceOpen("ULTIMATE_LOG : ULTIMATE DUNGEON OPEN!!");
        }
    }
    return 1;
}

Timer_DungeonInoutCloseTime::Timer_DungeonInoutCloseTime() {}
Timer_DungeonInoutCloseTime::~Timer_DungeonInoutCloseTime() {}
// ORIG Timer_DungeonInoutCloseTime::dispatch_sig @ 0x863a1d0 (0x25f)
int Timer_DungeonInoutCloseTime::dispatch_sig(int p1, int p2, unsigned int p3)
{
    (void)p1; (void)p2;
    time_t now = static_cast<time_t>(GlobalData::s_systemTime_.getCurSec());
    struct tm tmBuf;
    struct tm* ptm = localtime_r(&now, &tmBuf);
    const int curHour = ptm->tm_hour;
    const int curMin = ptm->tm_min;
    if (p3 == 0)
        return 0;

    CDataManager* dataMgr = G_CDataManager();
    std::map<int, STDungeonInoutTime>::iterator it =
        dataMgr->m_serverParameter.m_dungeonInoutc8.find(p3);
    if (it != dataMgr->m_serverParameter.m_dungeonInoutc8.end())
    {
        ptm->tm_hour = curHour + 1;
        ptm->tm_min = it->second.m_endMin;   // 迭代器 value +0xc
        ptm->tm_sec = 0;
        const time_t closeSec = mktime(ptm);
        const int delay = static_cast<int>(closeSec - now);

        if (delay < 0)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x14a3, 0);
            trace("ULTIMATE_LOG : TIMER OFF!!");
            return 0;
        }
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x93,
                                    delay, 0, static_cast<unsigned int>(p3));
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x14b1, 0);
            trace("ULTIMATE_LOG : ULTIMATE DUNGEON OPEN TIMER ON!!");
        }

        // !(IsPowerWarEventOn() || s_villageMonsterMgr->OnEvent())
        if (!GlobalData::s_power_manager->IsPowerWarEventOn() &&
            !GlobalData::s_villageMonsterMgr->OnEvent())
        {
            G_CDataManager()->m_serverParameter.setDungeonOpen(p3, false);
            G_GameWorld()->send_all_dungeon_inout_message(p3);
            {
                cMyTrace trace(__PRETTY_FUNCTION__, 0x14b8, 0);
                trace("ULTIMATE_LOG : ULTIMATE DUNGEON CLOSE!!");
            }
        }
    }
    return 1;
}

Timer_FairPvPRematch::Timer_FairPvPRematch() {}
Timer_FairPvPRematch::~Timer_FairPvPRematch() {}
// ===== Timer_FairPvPRematch =====
// dispatch_sig ORIG _ZN20Timer_FairPvPRematch12dispatch_sigEiij @0x0863a430 (0x97B)

int Timer_FairPvPRematch::dispatch_sig(int p1, int p2, unsigned int /*p3*/)
{
    PvP_Room* room = G_CGameManager()->GetPvp(p1, (CUser*)0, (int)0x3e);
    if (room == 0)
    {
        return 0;
    }
    if (room->get_timer_key((TIMER_MESSAGE)0x3e) != p2)
    {
        return 0;
    }
    // ORIG +0x61: matcher = *(void**)(room + 0x6e4)（匹配系统指针，ORIG 布局该槽为带虚表对象）
    void* matcher = *(void**)((char*)room + 0x6e4);
    if (matcher != 0)
    {
        // ORIG +0x8e: vtable[0x64/4=slot25](matcher, room)
        typedef void (*RematchSlotFn)(void* self, PvP_Room* room);
        RematchSlotFn fn = ((RematchSlotFn*)*(void***)matcher)[0x64 / 4];
        fn(matcher, room);
    }
    return 1;
}

Timer_ReturnUserTimeout::Timer_ReturnUserTimeout() {}
Timer_ReturnUserTimeout::~Timer_ReturnUserTimeout() {}
// ORIG Timer_ReturnUserTimeout::dispatch_sig @ 0x863a4c8 (0x23e)
int Timer_ReturnUserTimeout::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, 2);
    if (user == 0)
        return 0;

    if ((unsigned short)user->get_unique_id() != p2)
        return 0;

    user->ResetReturnUser();

    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x12d);
    packet.put_int(0);
    packet.finalize(true);
    user->Send(packet);

    packet.clear();
    packet.put_header(0, 2);
    packet.put_byte(0);
    packet.put_short(1);
    user->make_basic_info(reinterpret_cast<char*>(&packet), '\0');
    packet.finalize(true);

    CParty* party = static_cast<CParty*>(user->GetParty());
    if (user->get_state() == 5)
    {
        if (party == 0)
            user->Send(packet);
        else
            party->send_to_party(packet);
    }
    else
    {
        G_GameWorld()->send_all(packet, user);
    }

    if (party != 0)
    {
        if (!(party->IsReturnUserParty() && party->IsEventCharacParty()))
        {
            G_GameWorld()->send_party_info_to_all(party, 1);
        }
    }
    return 1;
}

// ORIG _ZN23Timer_ReturnUserTimeout15registNextTimerEiij @0x0863a706：
//   now = CSystemTime::getCurSec(&GlobalData::s_systemTime_);
//   G_TimerQueue()->InsertTimer(OBJ_TYPE_2, p1, (TIMER_MESSAGE)0x95,
//                               (int)(p3 - now), p2, 0)
void Timer_ReturnUserTimeout::registNextTimer(int p1, int p2, unsigned int p3)
{
    const int now = GlobalData::s_systemTime_.getCurSec();
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, p1,
                                (TIMER_MESSAGE)0x95, static_cast<int>(p3 - now),
                                p2, 0);
}

Timer_HadesHeartBeat::Timer_HadesHeartBeat() {}
Timer_HadesHeartBeat::~Timer_HadesHeartBeat() {}
int Timer_HadesHeartBeat::dispatch_sig(int p1, int p2, unsigned int p3)
{
    if (GlobalData::s_hades_proxy->GetSocketState() == 0) {
        int channelNo = G_CEnvironment()->get_channel_no();
        unsigned short udpPort =
            static_cast<unsigned short>(G_CEnvironment()->get_udp_port());
        GlobalData::s_hades_proxy->SendHeartBeatPacket(udpPort, channelNo);
        registNextTimer();
    }
    return 1;
}

// ORIG T 0x0863a7c2 _ZN20Timer_HadesHeartBeat15registNextTimerEv
void Timer_HadesHeartBeat::registNextTimer()
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x96, 0x3c, 0, 0);
}

Timer_TOD_Send_ClearReward::Timer_TOD_Send_ClearReward() {}
Timer_TOD_Send_ClearReward::~Timer_TOD_Send_ClearReward() {}
int Timer_TOD_Send_ClearReward::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == NULL) {
        return 0;
    }
    if (!party->check_timer_key((TIMER_MESSAGE)p3, p2)) {
        return 0;
    }
    if (!party->OnTODRewardTimer((TIMER_MESSAGE)p3)) {
        return 0;
    }
    return 1;
}

Timer_OneADayItemShop::Timer_OneADayItemShop() {}
Timer_OneADayItemShop::~Timer_OneADayItemShop() {}
int Timer_OneADayItemShop::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x863a916: G_CDataManager()->m_itemShop->updateOneADayItemList(); RegistNextTimer(); return 1
    CItemShop* itemShop = G_CDataManager()->m_itemShop;   // +0x8cbc
    itemShop->updateOneADayItemList();
    RegistNextTimer();
    return 1;
}

Timer_Schedule_MidNight::Timer_Schedule_MidNight() {}
Timer_Schedule_MidNight::~Timer_Schedule_MidNight() {}
int Timer_Schedule_MidNight::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x863a982: g_GoldControlDateFlag++; RegistNextTimer(); return 1
    ++GlobalData::g_GoldControlDateFlag;
    RegistNextTimer();
    return 1;
}

Timer_Send_Ontime_Reward::Timer_Send_Ontime_Reward() {}
Timer_Send_Ontime_Reward::~Timer_Send_Ontime_Reward() {}
// ===== Timer_Send_Ontime_Reward =====
// dispatch_sig ORIG _ZN24Timer_Send_Ontime_Reward12dispatch_sigEiij @0x0863aa90 (0xbcB)
// RegistNextTimer ORIG _ZN24Timer_Send_Ontime_Reward15RegistNextTimerEv @0x0863ab4c (0x45B)

int Timer_Send_Ontime_Reward::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    COnTimeEvent* ev =
        (COnTimeEvent*)GlobalData::s_event_manager->GetRepeatEvent(0x33);
    bool ok;
    if (ev == 0 || ev->IsEventing(0) == false)
    {
        // ORIG +0x49: mov $0x0,%eax（bVar1 = false）
        ok = false;
    }
    else
    {
        ok = true;
    }
    if (!ok)
    {
        cMyTrace trace(__PRETTY_FUNCTION__,
                       0x1638, 5);
        trace("[ON_TIME_EVENT] Timer_Send_Ontime_Reward() error");
        return 0;
    }
    // ORIG +0x58: process_send_reward_to_user()
    ev->process_send_reward_to_user();
    // ORIG +0x63: get_ontime_reward_user_count()
    if (ev->get_ontime_reward_user_count() != 0)
    {
        RegistNextTimer();
    }
    return 1;
}

// ORIG T 0x0863ab4c：static，全部常量实参
void Timer_Send_Ontime_Reward::RegistNextTimer()
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x9b, 10, 0, 0);
}

Timer_EventManagement::Timer_EventManagement() {}
Timer_EventManagement::~Timer_EventManagement() {}
int Timer_EventManagement::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 读 CEnvironment +0x37c int（字段未具名，仓库既有 reinterpret_cast 风格同 APSystemOnlyOrig.cpp:237）
    const int dayValue = G_CEnvironment()->m_serverEnvir.m_fatigueTime;
    EventClassify::CEventScriptMng* mng =
        static_cast<EventClassify::CEventScriptMng*>(G_CDataManager()->get_event_script_mng());
    mng->update_time_values(dayValue);
    RegistNextTimer(0x3c);
    return 1;
}

Timer_QuickPartyWaitingPartyMatching::Timer_QuickPartyWaitingPartyMatching() {}
Timer_QuickPartyWaitingPartyMatching::~Timer_QuickPartyWaitingPartyMatching() {}
// ORIG Timer_QuickPartyWaitingPartyMatching::dispatch_sig @ 0x863ac18 (0x17e)
int Timer_QuickPartyWaitingPartyMatching::dispatch_sig(int p1, int p2, unsigned int p3)
{
    QuickParty::CQuickParty* quickParty =
        G_CGameManager()->GetQuickParty(p1);
    if (quickParty == 0)
        return 0;

    if (!quickParty->check_timer_key((TIMER_MESSAGE)p3, p2))
        return 0;

    const int quickPartyCount = quickParty->get_quick_party_count();
    // ORIG：count < 2 && first_matching_cnt == count（movsbl 符号扩展后比较）
    if ((quickPartyCount < 2) &&
        (static_cast<int>(quickParty->get_first_matching_cnt()) == quickPartyCount))
    {
        return 0;
    }

    if (quickParty->get_timer_key((TIMER_MESSAGE)p3) == 1)
    {
        if (!cancel_quick_party(quickParty))
            return 0;
    }

    // ORIG：!(cnt < count) && !(is_change_quick_party_count()) 时取消
    {
        bool cancelNow;
        const int firstCnt = static_cast<int>(quickParty->get_first_matching_cnt());
        if (firstCnt < quickPartyCount)
        {
            cancelNow = false;
        }
        else
        {
            cancelNow = !quickParty->is_change_quick_party_count();
        }
        if (cancelNow)
        {
            if (!cancel_quick_party(quickParty))
                return 0;
        }
    }

    QuickParty::STQuickPartyPoolMap_Key poolKey;   // ORIG ctor W @0x082344ca
    poolKey.m_dungeonIndex = quickParty->get_dungeon_index();
    poolKey.m_dungeonDiff  = static_cast<unsigned char>(quickParty->get_dungeon_diff());

    CGameManager* game = G_CGameManager();
    QuickParty::CQuickPartySystemManager* mgr = game->GetQuickPartySystemManager();
    mgr->start_game(quickParty, poolKey);
    return 1;
}

// ORIG T 0x863ad96（cancel_quick_party.md 全文转写；成员函数，需 TimerDispatcher.h 补声明）
bool Timer_QuickPartyWaitingPartyMatching::cancel_quick_party(
    QuickParty::CQuickParty* quickParty)
{
    CParty* party = quickParty->get_party_object();
    if (party == 0)
    {
        return false;
    }
    QuickParty::CQuickPartySystemManager* mgr =
        G_CGameManager()->GetQuickPartySystemManager();
    if (!mgr->cancel_quick_party(party, true, static_cast<CUser*>(0)))
    {
        return true;
    }
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x17f);
    packet.finalize(true);
    party->send_to_party(packet);
    return false;
}

Timer_QuickPartyWaitingStartGame::Timer_QuickPartyWaitingStartGame() {}
Timer_QuickPartyWaitingStartGame::~Timer_QuickPartyWaitingStartGame() {}
// ORIG Timer_QuickPartyWaitingStartGame::dispatch_sig @ 0x863ae82 (0x238)
int Timer_QuickPartyWaitingStartGame::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
        return 0;

    if (!party->check_timer_key((TIMER_MESSAGE)p3, p2))
        return 0;

    party->send_quick_party_matching_result(false, (CUser*)0);
    party->set_quick_party(false);

    CUserCharacInfo* managerInfo = party->getManager();
    const char* managerName = managerInfo->getCurCharacName();
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x16a8, 0);
        trace("QUICK_PARTY_LOG : PARTY DESTROY AND LEAVE DUNGEON !! : MANAGER (%s)", managerName);
    }

    for (int i = 0; i < 4; ++i)   // asm: cmp $3 / setle（i <= 3）
    {
        if (!party->checkValidUser(i))
            continue;

        CUser* user = party->get_user(i);
        const int ret = party->leave_user(user, (ENUM_PARTY_INFO_TYPE)2);
        if (ret != 1)
        {
            G_GameWorld()->out_from_dungeon(user);
            PacketGuard packet;
            packet.clear();
            packet.put_header(0, 2);
            packet.put_byte(0);
            packet.put_short(1);
            user->make_basic_info(reinterpret_cast<char*>(&packet), '\0');
            packet.finalize(true);
            G_GameWorld()->send_all(packet, user);
        }
    }
    return 1;
}

Timer_AssaultGhostTimer::Timer_AssaultGhostTimer() {}
Timer_AssaultGhostTimer::~Timer_AssaultGhostTimer() {}
int Timer_AssaultGhostTimer::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, 2);

    if (user == 0)
    {
        return 0;
    }

    if (user->getCurCharacNo() != (int)p3)
    {
        return 0;
    }

    if (user->IsCurCharacGhost())
    {
        user->SetCurCharacGhost(false);
        user->SetCurCharacChaosDieTime(0);
        user->SetCurCharacChaosResponTime(0);
        user->m_characterView.enableSaveCharacView();

        PacketGuard packet;
        packet.clear();
        packet.put_header(0, 3);
        packet.put_byte(1);
        packet.put_short(user->get_unique_id() & 0xffff);
        packet.put_byte(0);
        packet.finalize(true);

        G_GameWorld()->send_all(packet, user);
    }

    return 1;
}

Timer_GenerateCaptchaData::Timer_GenerateCaptchaData() {}
Timer_GenerateCaptchaData::~Timer_GenerateCaptchaData() {}
int Timer_GenerateCaptchaData::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x863b29c: GlobalData::GenerateCaptchaData(); RegistNextTimer(3); return 1
    GlobalData::GenerateCaptchaData();
    RegistNextTimer(3);
    return 1;
}
void Timer_GenerateCaptchaData::RegistNextTimer(int sec)
{
    // ORIG 0x863b2ba：G_TimerQueue()->InsertTimer((OBJ_TYPE)2, 0, (TIMER_MESSAGE)0xa1,
    // sec, 0, 0)。
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0xa1,
                                sec, 0, 0);
}

// ===== Timer_TournamentDugeonClearStateNotiReward =====
Timer_TournamentDugeonClearStateNotiReward::Timer_TournamentDugeonClearStateNotiReward() {}
Timer_TournamentDugeonClearStateNotiReward::~Timer_TournamentDugeonClearStateNotiReward() {}
int Timer_TournamentDugeonClearStateNotiReward::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)0x2e, p3))
        return 0;
    if (party->GetTournamentDungeonClearState() == 'x01')
    {
        PacketGuard packet;
        packet.clear();
        packet.put_header(1, 0x1ca);
        packet.put_byte(1);
        for (int i = 0; i <= 3; ++i)
        {
            CUser* user = party->get_user(i);
            if (user == 0)
            {
                packet.put_byte(-1);
            }
            else
            {
                unsigned short fatigue = user->getCurCharacTotalFatigue();
                unsigned short maxFatigue = user->getCurCharacTotalMaxFatigue();
                if (fatigue == maxFatigue)
                    packet.put_byte(0);
                else
                    packet.put_byte(1);
            }
        }
        packet.finalize(true);
        party->send_to_party(packet);
        party->SetTournamentDungeonClearState('x02');
        party->InsertTournamentDungeonTimer((TIMER_MESSAGE)0x2f);
    }
    return 1;
}

// ===== Timer_TournametDungeonClearStateSelect =====
Timer_TournametDungeonClearStateSelect::Timer_TournametDungeonClearStateSelect() {}
Timer_TournametDungeonClearStateSelect::~Timer_TournametDungeonClearStateSelect() {}
int Timer_TournametDungeonClearStateSelect::dispatch_sig(int p1, int p2, unsigned int p3)
{
    (void)p3;   // ORIG 0x0863b68c：check_timer_key 用 p2（0x10(%ebp)），非 p3
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)0x2f, p2))
        return 0;
    if (party->GetTournamentDungeonClearState() == 2)
    {
        PacketGuard packet;
        packet.clear();
        packet.put_header(1, 0x1cb);
        packet.put_byte(1);
        CTournamentDungeonReward& reward = party->m_tournamentReward;  // ORIG party+0x17bc
        std::vector<int> members;
        party->GetValidPartyMember(members);
        const bool victory = party->isTournamentVictory();
        if (!reward.isCardSelectComplete((ENUM_TOURNAMENT_REWARD_CARD_TYPE)2, members))
            reward.ForceSelectCard(members, victory);
        reward.MakeSelectPacket(&packet);
        packet.finalize(true);
        party->send_to_party(packet);
        party->SetTournamentDungeonClearState(3);
        party->TournamentDungeonClear(victory);
    }
    return 1;
}

Timer_TowerOnTime::Timer_TowerOnTime() {}
Timer_TowerOnTime::~Timer_TowerOnTime() {}
int Timer_TowerOnTime::dispatch_sig(int p1, int p2, unsigned int p3)
{
    WongWork::CBossTower* tower = G_CGameManager()->getBossTower(p1);
    if (tower == NULL) {
        return 0;
    }
    tower->_onTimerProcess(static_cast<TIMER_MESSAGE>(p3), p2);
    return 1;
}

Timer_NoticeEmergencyPatchMessage::Timer_NoticeEmergencyPatchMessage()
{
    m_remainingNotiCount = 0xa;   // ORIG W 0x0863bc2e：movl $0xa,0x4(%eax)
}
Timer_NoticeEmergencyPatchMessage::~Timer_NoticeEmergencyPatchMessage() {}
// ORIG T 0x0863b6c4 sz 0x1ba（_ZN33Timer_NoticeEmergencyPatchMessage12dispatch_sigEiij）
// 报告 docs/class_func_reports/Timer_NoticeEmergencyPatchMessage/dispatch_sig.md 全文已读：
// PacketGuard；put_header(0,0xc)；put_byte(0)；put_short(0)；put_byte(0)；
// local_43c[0x100] rep stos 清零；sprintf("%d분뒤 서버점검이 있습니다. %s",
//   this+0x04, GameserverDemonManagerInstance()->GetNoticeMessage())；
// len=strlen；put_int(len)；put_str(local_43c,len)；finalize(true)；
// G_GameWorld()->send_all；this+0x04 -= 1；
// ==0 → GetInstanceShutdowManager()->SendShutdownSignal() +
//   cMyTrace(PRETTY,0x1849,0)("Emergency Patch Message Received")；
// !=0 → RegistNextTimer()；return 1。
int Timer_NoticeEmergencyPatchMessage::dispatch_sig(int /*p1*/, int /*p2*/,
                                                    unsigned int /*p3*/)
{
    char local_43c[0x100] = { '\0' };   // ORIG +0x6b..0x7f：rep stos 清零 0x100
    PacketGuard packet;
    packet.put_header(0, 0xc);
    packet.put_byte(0);
    packet.put_short(0);
    packet.put_byte(0);
    char const* notice = GameserverDemonManagerInstance()->GetNoticeMessage();
    sprintf(local_43c, "%d분뒤 서버점검이 있습니다. %s", m_remainingNotiCount,
            notice);
    unsigned int len = strlen(local_43c);
    packet.put_int((int)len);
    packet.put_str(local_43c, (int)len);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
    m_remainingNotiCount -= 1;
    if (m_remainingNotiCount == 0)
    {
        GetInstanceShutdowManager()->SendShutdownSignal();
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x1849, 0);
        trace("Emergency Patch Message Received");
    }
    else
    {
        RegistNextTimer();
    }
    return 1;
}
// ORIG T 0x0863b87e sz 0x45（_ZN33Timer_NoticeEmergencyPatchMessage15RegistNextTimerEv）：
// G_TimerQueue()->InsertTimer(OBJ_TYPE_2, 0, 0xa2, 0x3c, 0, 0)。
void Timer_NoticeEmergencyPatchMessage::RegistNextTimer()
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0xa2,
                                0x3c, 0, 0);
}

advancealtar::Timer_MainTick::Timer_MainTick() {}
advancealtar::Timer_MainTick::~Timer_MainTick() {}
int advancealtar::Timer_MainTick::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x812f798: G_CGameManager()->onTimeAdvanceAltar(); RegistNextTimerGage(); return 1
    G_CGameManager()->onTimeAdvanceAltar();
    RegistNextTimerGage();
    return 1;
}

advancealtar::Timer_StageControl::Timer_StageControl() {}
advancealtar::Timer_StageControl::~Timer_StageControl() {}
int advancealtar::Timer_StageControl::dispatch_sig(int p1, int p2, unsigned int p3)
{
    advancealtar::StageControl* stage = G_CGameManager()->getAdvanceAltar(p1);
    if (stage == NULL) {
        return 0;
    }
    stage->onTimer(static_cast<TIMER_MESSAGE>(p2),
                   static_cast<advancealtar::StageEndType::T>(p3));
    return 1;
}

// ORIG T 0x0812f850 _ZN12advancealtar18Timer_StageControl21RegistTimerCheckFinshEi
void advancealtar::Timer_StageControl::RegistTimerCheckFinsh(int stageIdx)
{
    G_TimerQueue()->InsertTimerInMilisecond(
        (TimerEntry::OBJ_TYPE)8, stageIdx, (TIMER_MESSAGE)0xa7, kCheckFinishMs, 0xa7, 0);
}

// ORIG T 0x0812f89c _ZN12advancealtar18Timer_StageControl22RegistTimerClearRewardEiNS_12StageEndType1TE
void advancealtar::Timer_StageControl::RegistTimerClearReward(int stageIdx,
                                                             advancealtar::StageEndType::T endType)
{
    G_TimerQueue()->InsertTimerInMilisecond(
        (TimerEntry::OBJ_TYPE)8, stageIdx, (TIMER_MESSAGE)0xa8, kClearRewardMs, 0xa8, endType);
}

// ORIG T 0x0812f8ec _ZN12advancealtar18Timer_StageControl15RegistTimerEPLPEiNS_12StageEndType1TE
void advancealtar::Timer_StageControl::RegistTimerEPLP(int stageIdx,
                                                       advancealtar::StageEndType::T endType)
{
    long ms = kFailEPLPMs;      // ORIG 先置 0x3e8；endType==3 分支同值再赋 0x3e8（语义无差异）
    if (endType == static_cast<advancealtar::StageEndType::T>(3)) {
        ms = kClearEPLPMs;
    }
    G_TimerQueue()->InsertTimerInMilisecond(
        (TimerEntry::OBJ_TYPE)8, stageIdx, (TIMER_MESSAGE)0xa9, ms, 0xa9, endType);
}

// ORIG T 0x0812f956 _ZN12advancealtar18Timer_StageControl28RegistTimerEPLPReturnVillageEi
void advancealtar::Timer_StageControl::RegistTimerEPLPReturnVillage(int stageIdx)
{
    G_TimerQueue()->InsertTimerInMilisecond(
        (TimerEntry::OBJ_TYPE)8, stageIdx, (TIMER_MESSAGE)0xaa, kEPLPReturnVillageMs, 0xaa, 0);
}

Timer_DungeonStatisticEntrance_Hour::Timer_DungeonStatisticEntrance_Hour() {}
Timer_DungeonStatisticEntrance_Hour::~Timer_DungeonStatisticEntrance_Hour() {}
int Timer_DungeonStatisticEntrance_Hour::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x863b9da: GetInstanceDungeonEntranceLog()->InsertDungeonEntrance_Hour(); RegistNextTimer(); return 1
    CDungeonEntranceLog* log = (CDungeonEntranceLog*)GetInstanceDungeonEntranceLog();
    log->InsertDungeonEntrance_Hour();
    RegistNextTimer();
    return 1;
}

Timer_RefreshServerState::Timer_RefreshServerState() {}
Timer_RefreshServerState::~Timer_RefreshServerState() {}
int Timer_RefreshServerState::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x863b90e: ARAD::Arad_ServerStateManager::checkContinus(s_serverStateManager_); return 1
    GlobalData::s_serverStateManager_->checkContinus();
    return 1;
}

Timer_StayTimeEvent::Timer_StayTimeEvent() {}
Timer_StayTimeEvent::~Timer_StayTimeEvent() {}
// ===== Timer_StayTimeEvent =====
// dispatch_sig ORIG _ZN19Timer_StayTimeEvent12dispatch_sigEiij @0x0863b928 (0xb2B)

int Timer_StayTimeEvent::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    CEventStayTime* ev =
        (CEventStayTime*)GlobalData::s_event_manager->GetRepeatEvent(0x9f);
    bool ok;
    if (ev != 0 && ev->IsEventing(0) != false)
    {
        ok = true;
    }
    else
    {
        ok = false;
    }
    if (!ok)
    {
        return 1;
    }
    // ORIG +0x5a: time(NULL)
    const time_t now = time((time_t*)0);
    // ORIG +0x67: getRewardStayTime()
    const unsigned int rewardStayTime = ev->getRewardStayTime();
    // ORIG +0x7a: 内联 ctor —— m_0=rewardStayTime, m_4=now（局部 8 字节）
    CEventStayTime::RewardGoGoFighter visitor(rewardStayTime, now);
    // ORIG +0x97: GameWorld::ProcessAllUsers<CEventStayTime::RewardGoGoFighter>(visitor)
    G_GameWorld()->ProcessAllUsers(visitor);
    // ORIG +0xa2: CEventStayTime::RegistNextTimer()
    ev->RegistNextTimer();
    return 1;
}

Timer_CreateDnfDbLoad::Timer_CreateDnfDbLoad() {}
Timer_CreateDnfDbLoad::~Timer_CreateDnfDbLoad() {}
int Timer_CreateDnfDbLoad::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    // ORIG 0x863ba46：GetRepeatEvent(0xa0)（CEventCreateDnf）判空 + IsEventing(0)
    // 为真时 RegistNextTimer()；随后无条件 DBSelectCreateDnfEventInfo::makeRequest()。
    CEventCreateDnf* event =
        static_cast<CEventCreateDnf*>(GlobalData::s_event_manager->GetRepeatEvent(0xa0));
    if (event != 0 && event->IsEventing(0))
    {
        event->RegistNextTimer();
    }
    DBSelectCreateDnfEventInfo::makeRequest();
    return 1;
}

// ORIG T 0x0815ba94（_ZN26DBSelectCreateDnfEventInfo11makeRequestEv）
// 报告：docs/class_func_reports/DBSelectCreateDnfEventInfo/makeRequest.md：
// 事件未在进行中（或 receiver 空）时发 Taiwan DbThread 0x9 选择请求。
void DBSelectCreateDnfEventInfo::makeRequest()
{
    CEventCreateDnf* event =
        static_cast<CEventCreateDnf*>(GlobalData::s_event_manager->GetRepeatEvent(0xa0));
    bool skip = true;
    if (event != 0)
    {
        skip = event->IsEventing(0);
    }
    if (!skip)
    {
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire(
                "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", 0x1e4),
            true);
        Taiwan::internal_stream(guard, static_cast<TaiwanInternalPack::T>(9), -1);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
}


// 类内已初始化（TimerDispatcher.h）；此处仅保留类外定义以维持 ORIG R 符号。
const unsigned int advancealtar::Timer_MainTick::kMainTickMs = 1000;  // ORIG R 0x8b6c5e4（头文件无类内初始化）
const unsigned int advancealtar::Timer_StageControl::kFailEPLPMs;   // ORIG R 0x8b6c5f4
const unsigned int advancealtar::Timer_StageControl::kClearEPLPMs;  // ORIG R 0x8b6c5f0
const unsigned int advancealtar::Timer_StageControl::kCheckFinishMs;// ORIG R 0x8b6c5e8
const unsigned int advancealtar::Timer_StageControl::kClearRewardMs;// ORIG R 0x8b6c5ec
const unsigned int advancealtar::Timer_StageControl::kEPLPReturnVillageMs; // ORIG R 0x8b6c5f8

// ---------------------------------------------------------------------------
// LinkClosureB-2 追加：三个 RegistNextTimer（报告全文已读，逐指令转写）
// ---------------------------------------------------------------------------

// ORIG T 0x086340b4 sz 0x4f（_ZN18TimerStatisticsLog15RegistNextTimerEv）：
// G_TimerQueue()->InsertTimer(OBJ_TYPE_2, 0, 0x52, 0x708, 0, 0)；返回 1。
bool TimerStatisticsLog::RegistNextTimer()
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x52,
                                0x708, 0, 0);
    return 1;
}

// ORIG T 0x0863a9a2 sz 0xed（_ZN23Timer_Schedule_MidNight15RegistNextTimerEv）：
// cur=getCurSec()；localtime_r(&cur+0x15180,&tm) → tm_hour/min/sec=0；
// next=mktime(&tm)；delta=next-getCurSec()；
// cMyTrace(__PRETTY_FUNCTION__,0x1626,0)
//   ("RegistNextTimer Schedule_MidNight delta %d\n", delta)；
// G_TimerQueue()->InsertTimer(OBJ_TYPE_2, 0, 0x9a, delta, 0, 0)。
void Timer_Schedule_MidNight::RegistNextTimer()
{
    struct tm v;
    time_t sec = (time_t)GlobalData::s_systemTime_.getCurSec();
    sec += 0x15180;
    struct tm* pt = localtime_r(&sec, &v);
    pt->tm_hour = 0;
    pt->tm_min = 0;
    pt->tm_sec = 0;
    int next = (int)mktime(pt);
    int delta = next - GlobalData::s_systemTime_.getCurSec();
    cMyTrace trace(__PRETTY_FUNCTION__,
                   0x1626, 0);
    trace("RegistNextTimer Schedule_MidNight delta %d\n", delta);
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x9a,
                                delta, 0, 0);
}

// ORIG T 0x086396d2 sz 0x4f（_ZN23Timer_TenMinuteSchedule15RegistNextTimerEi）：
// G_TimerQueue()->InsertTimer(OBJ_TYPE_2, 0, 0x8e, 600, param_1, 0)；返回 1。
bool Timer_TenMinuteSchedule::RegistNextTimer(int type)
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x8e,
                                0x258, type, 0);
    return 1;
}

// ---------------------------------------------------------------------------
// LinkClosureB-2 追加：_NS_NonClient_CheckResponse（无独立报告，ORIG objdump
// 0x08274e4e sz 0xe3 逐指令转写，标注【推断】仅限语义命名）
// ---------------------------------------------------------------------------
// ORIG _Z27_NS_NonClient_CheckResponseiii：
//   user=G_CGameManager()->getUser(slot,2)；user==0 → 0；
//   (uint16)user->get_unique_id() != param_2 → 0（ORIG movzwl 16 位比较，
//     值域内与 int 比较等价）；
//   getCurCharacR()==0 → 0；getCurCharacNo()!=param_3 → 0；
//   !GetNonClientFlag()（ORIG xor $1 bool 取反）→
//     getHackAnalyzer()->addServerHackCnt(user, ENUM_HACKTYPE(0x33a), 1, 0, 0)；
//   return 1。
int _NS_NonClient_CheckResponse(int slot, int uniqueId, int characNo)
{
    CUser* user = G_CGameManager()->getUser(slot, 2);
    if (user == 0)
    {
        return 0;
    }
    if (user->get_unique_id() != uniqueId)
    {
        return 0;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0;
    }
    if (user->getCurCharacNo() != characNo)
    {
        return 0;
    }
    if (!user->GetNonClientFlag())
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x33a, 1, 0, 0);
    }
    return 1;
}
