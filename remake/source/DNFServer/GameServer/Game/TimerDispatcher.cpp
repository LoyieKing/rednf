#include "TimerDispatcher.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
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
// §6.2 超大函数（1661 insns）验收：调用点差集 MISSING=0；EXTRA 仅 +2 operator
// delete / +2 _Unwind_Resume（new 表达式 EH 清理路径，N 类『new-expression
// landing-pad』编译器生成差，源码侧无合法解豁免）；gap 1661→1679(-18，全部为
// 上述 EH 清理块) ⇒ 判语义完成、byte 层 N 封顶（N83）。
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
    // ORIG 0x086329f6：非空分支为 fall-through（先 reload 再 jmp），空走 mov $0。
    if (m_dispatchers[msg] != 0)
    {
        return m_dispatchers[msg];
    }
    return 0;
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
// CParty +0xb24 内嵌 CBattle_Field 子对象局部视图（与 GmCmds_D.cpp/
// DisPatcherCore.cpp 同款；CParty.h 以 pad 占位，Main 裁定 TU 局部视图）。
struct CPartyBattleFieldView
{
    char m_pad[0xb24];
    CBattle_Field m_battleField;   // +0xb24
};
// stub 体待后续批次按 docs/class_func_reports 还原；地址=该函数 ORIG 起点。

TimerCheckConn::TimerCheckConn() {}
TimerCheckConn::~TimerCheckConn() {}
int TimerCheckConn::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x8632bca：getUser(p1, 2)（increId 形参被 getUser 忽略，常量 2 为占位）。
    CUser* user = G_CGameManager()->getUser(p1, 2);
    // ORIG 0x8632bff：movzwl %ax 后与 p2 作 int 比较（unique_id 低 16 位）。
    if (user == 0 || static_cast<unsigned short>(user->get_unique_id()) != p2)
    {
        return 0;
    }
    std::string accName(user->get_acc_name());
    char ip[0x10] = {0};
    user->m_network.GetPeerIP2(ip, 0x10);
    if (G_CGameManager()->IsNoGameGuardApplingUser(accName, ip))
    {
        cMyTrace tr(__PRETTY_FUNCTION__, 0x3a4, 0);
        tr("Try To Login By No Game Guard : %s", accName.c_str());
        return 1;
    }
    if (user->get_state() != 0)
    {
        if (user->get_connect_count() <= 2)
        {
            if (user->get_connect_count() > 1)
            {
                cMyTrace tr2(__PRETTY_FUNCTION__, 0x3bc, 0);
                tr2("User %s - no response %dth ping.", user->getCurCharacName(),
                    static_cast<int>(user->get_connect_count()));
                user->SetResumeChecksumTime(GlobalData::s_systemTime_.getCurSec() +
                                            user->GetCheckConnInterval());
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
        }
        else
        {
            // ORIG 0x8632ec4：连续 3 次无响应 → 日志 + 断线。
            LogManager::logFormat(1, __PRETTY_FUNCTION__, "TimerDispatcher.cpp", 0x40a,
                                  "User %s - no response during 3 times ping",
                                  user->getCurCharacName());
            user->DisConnSig(static_cast<DISCONN_SIG>(8), false, 0);
        }
    }
    return 1;
}

TimerDBUpdate::TimerDBUpdate() {}
TimerDBUpdate::~TimerDBUpdate() {}
int TimerDBUpdate::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x8631f0c：getUser(p1, 3)（第二实参为常量 3，increId 占位）。
    CUser* user = G_CGameManager()->getUser(p1, 3);
    if (user == 0)
    {
        return 0;
    }
    // UpdateData/GetUID 为 CUser 成员（CUser 继承 CUserCharacInfo），保持 CUser*。
    const int elapsedHours =
        (GlobalData::s_systemTime_.getCurSec() - user->GetLoginTick()) / 0xe10;
    if (elapsedHours == 1)
    {
        APSystem::CUserProc::ClearActionAndSendtoUser(
            user, (APSystem::_ActionGroupIndex)0x23, 0, ENUM_PACKETCLASS_0);
    }
    else if (elapsedHours > 0xb && elapsedHours < 0xd)
    {
        APSystem::CUserProc::ClearActionAndSendtoUser(
            user, (APSystem::_ActionGroupIndex)0x24, 0, ENUM_PACKETCLASS_0);
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
    CUser* user = G_CGameManager()->getUser(p1, 4);   // ORIG 0x08633c90 第2参固定 4（p2 仅用于 loginTick 比对）
    if (user == 0 || user->GetLoginTick() != p2 ||
        (p3 != 0 && static_cast<unsigned int>(user->get_charac_no(-1)) != p3))
    {
        return 0;
    }
    user->recover_stamina();
    user->IncCurCharacLuckPointUpdateTimeCount();
    const int updateCount = user->GetCurCharacLuckPointUpdateTimeCount();
    // P48 no-CSE：ORIG 每处 use 各自 fresh G_CDataManager()（m_84 / m_80 共 2 次）。
    if (G_CDataManager()->m_serverParameter.m_84 <= updateCount)
    {
        user->IncCurCharacLuckPoint(G_CDataManager()->m_serverParameter.m_80);
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
    // ORIG 0x0863375d `mov 0x37c(%eax),%ebx` 单段直落 callee-saved 跨 update 调用；
    // 绑定版消 int 槽（guard1/guard2 槽归位 -0x14/-0x1c），装载恒两段
    // N96/P0：绑定形已废，自然局部（ORIG 寄存器直落残差记 N18a/N76）。
    int dailyResetHour = G_CEnvironment()->m_serverEnvir.m_fatigueTime;
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
    G_GameWorld()->ResetEntireReliablePerson();   // ORIG 0x8633806：resetDailyData 之后立即调用
    GetInstanceDungeonEntranceLog()->InsertDungeonEntrance();
    GetInstanceCommonStatisticsMgr()->InsertDatabase();

    // ORIG：三个统计域上报（CLevelDungeonPlayStatistic@+0x0、CDungeonExpStatistic@+0x18、
    // CExpertJobStatistic 单例），与上面两个 InsertDatabase 同批落库。
    ((CLevelDungeonPlayStatistic*)GlobalData::s_statisticMgr)->InsertDatabase();
    ((CDungeonExpStatistic*)((char*)GlobalData::s_statisticMgr + 0x18))->InsertDatabase();
    GetInstanceExpertJobStatistic()->UpdateExpertJob();

    advancealtar::HistoryLog::updateStatistics();   // 已声明，定义未链接 → UNLINKED_CALLEE

    // ORIG 0x0863386c-0x086338fd：Acquire 结果 %eax 直落 guard ctor 0x4(%esp)
    // （技巧36）；guard1 槽 -0x14、guard2 槽 -0x1c 紧邻、dtor+catch 各随块尾
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("TimerDispatcher.cpp", 0x512), true);
        // ORIG 0x08633888-0x086338b9：每写经显式 operator*() 调用（跨版本代理链
        // 形，同 CUserOnlyOrig LoadMileage 模板；guard<<x 直调会丢 op* 段）
        guard.operator*()->operator<<((int)0x4e);
        guard.operator*()->operator<<((int)-1);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q /*2*/, guard);
    }

    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("TimerDispatcher.cpp", 0x51c), true);
        guard.operator*()->operator<<((int)0x50);
        guard.operator*()->operator<<((int)-1);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q /*4*/, guard);
    }

    DB_LoadAutoMarketConditionsControl::makeRequest(false);   // 定义 W9_EventSmall.cpp:30
    DB_CreateCharac::makeRequestResetLimitCreateMid();        // 定义 DB_CreateCharac.cpp
    G_GameWorld()->ResetOneADayItemShop();

    // ORIG 0x086339d9-0x086339e7：bool 返回 byte 槽 -0x9 物化 + `movzbl; xor $0x1;
    // test; je TRUE岛(mov$1)`——false fall-through（技巧1/22 同族）
    bool ok = RegistNextTimer();
    if (ok != 1)
        return 0;
    return 1;
}

// ORIG T 0x08633a04 (_ZN17TimerFatigueReset15RegistNextTimerEv)
// ORIG 非静态成员但不使用 this（调用点 0x086339d4 传 this）；返回 bool。
bool TimerFatigueReset::RegistNextTimer()
{
    // ORIG 0x08633a17-0x08633a1c：`call getCurSec; mov %eax,-0x4c` 单段直存，
    // curSec 槽与 tmv(-0x78..-0x49) 高端相邻——试先声明取得更外槽位
    time_t curSec = GlobalData::s_systemTime_.getCurSec();   // -0x4c
    tm timeInfo;                                             // local_7c -0x78

    tm* pt = localtime_r(&curSec, &timeInfo);                // ORIG sret 形 pt 槽 -0x28

    const int hour = pt->tm_hour;                            // -0x24（08633a33 直读）
    // ORIG 0x08633a40/0x45/0x47：三字段各 `mov -0x28,%eax` 重读 + 单段直落
    // N96/P0：绑定形已废（跨 call 保活残差记 N76/N82附）。
    int sec = pt->tm_sec;
    int min = pt->tm_min;
    int hr = pt->tm_hour;

    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x572, 0);
        // ORIG 0x08633a71-0x08633a79：参数装载 edi→0x10/min→0xc/hr→0x8（右→左）
        trace("Reset all user's fatigue!(%02d:%02d:%02d)", hr, min, sec);
    }

    // env+0x37c 写回 tm_hour；min/sec 清零（ORIG 0x8(%eax)/0x4(%eax)/(%eax) 槽序）。
    pt->tm_hour = G_CEnvironment()->m_serverEnvir.m_fatigueTime;   // env+0x37c
    pt->tm_min = 0;
    pt->tm_sec = 0;

    int nextMidnight = static_cast<int>(mktime(pt));         // -0x20
    if (G_CEnvironment()->m_serverEnvir.m_fatigueTime <= hour)
        nextMidnight += 0x15180;                             // ORIG addl 原地 -0x20

    const int delta = nextMidnight - static_cast<int>(curSec);  // -0x1c

    {
        cMyTrace trace2(__PRETTY_FUNCTION__, 0x587, 0);
        trace2("DELTA %d , CHECK_T %d , CUR_T %d\n", delta, nextMidnight, static_cast<int>(curSec));
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
bool TimerAvatarExpire::RegistTimer(CUser* user)   // ORIG 单出口 mov $0x1,%eax（恒 true）
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
    return true;
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
    // ORIG 0x08633f3d-0x08633f54：Acquire 结果 %eax 直落 guard ctor 0x4(%esp)
    // （技巧36 删 s 中转槽）
    // ORIG 0x08633f25-0x08633fce：guard 生命周期止于 put 后（dtor+catch 块夹在
    // getter 段之前，ORIG 0x08633fad catch）⇒ 显式 {} 作用域块（技巧12）
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("TimerDispatcher.cpp", 0x62f), true);
        // ORIG 0x08633f59-0x08633f6f：显式 operator*() 代理链（同 FatigueReset 模板）
        guard.operator*()->operator<<((int)0xcd);
        guard.operator*()->operator<<((int)-1);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    }

    // ORIG 0x08633fe0/0x08633ff0/0x086334000：前三 getter 结果槽 -0x44/-0x40/-0x3c
    //（4.1 声明序布局；4.4 按生命周期落 -0x28..近端，槽序不可翻=N 族残差）
    int cacheLoadFailOverflowCount = exchange_server::GetInstanceExchangeServerHitRate()->GetCacheLoadFailOverflowCount();
    int cacheLoadFailCount = exchange_server::GetInstanceExchangeServerHitRate()->GetCacheLoadFailCount();
    int cacheLoadOverflowCount = exchange_server::GetInstanceExchangeServerHitRate()->GetCacheLoadOverflowCount();
    // ORIG 0x08634010/0x0863401f/0x0863402e：后三 getter 结果直落
    // %edi/%esi/%ebx 跨 ctor 存活；绑定版残差=装载时多一条重排 mov（probe 实测
    // N96/P0：绑定形已废（三 callee 结果寄存器缓存残差记 N76/N82附）。
    int cacheLoadCount =
        exchange_server::GetInstanceExchangeServerHitRate()->GetCacheLoadCount();
    int dbLoadOverflowCount =
        exchange_server::GetInstanceExchangeServerHitRate()->GetDBLoadOverflowCount();
    int dbLoadCount =
        exchange_server::GetInstanceExchangeServerHitRate()->GetDBLoadCount();

    cMyTrace trace(__PRETTY_FUNCTION__, 0x637, 0);
    trace("EXCHANGE_SERVER_HIT_RATE : db count(%u), overflow db count(%u), cache count(%u), overflow cache count(%u), cache fail count(%u), overflow cache fail count(%u)",
          dbLoadCount, dbLoadOverflowCount, cacheLoadCount, cacheLoadOverflowCount,
          cacheLoadFailCount, cacheLoadFailOverflowCount);

    // ORIG 0x08634092-0x0863409b：`mov %al,-0x19; movzbl -0x19,%eax; xor $0x1`——
    // 具名 bool 局部 byte 槽 + 重读后 ==1 判定（技巧22 邻域）
    bool ok = TimerStatisticsLog::RegistNextTimer();
    // ORIG 0x08634099 `xor $0x1; test; je TRUE岛` ——false 为 fall-through 体
    if (ok != 1)
        return 0;
    return 1;
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
    // ORIG 0x08634412..+0x84：|| 合并条件 —— gcc4.4 对 || 生成短路跳转 +
    // 汇合点 mov$1/jmp/mov$0 物化（探针 x4 p1 实证）；独立两 if 各自 sete，不合 ORIG。
    if (party->get_dungeon_clear_state() == 0 ||
        party->get_dungeon_clear_state() == 5)
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
    // [b23S3C] P48 去缓存：get_user 每调用点重取×3；fatigue/max 结果寄存器直比；
    // 判定链拆早返岛（fresh .o 134/134 零差）。
    // ORIG 0x086345f2：GetParty(p1) + check_timer_key(0x12, p2)（用 p2 非 p3）。
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)0x12, p2))
        return 0;
    if (party->get_dungeon_clear_state() == 0x03)
    {
        PacketGuard packet;
        packet.put_header(1, 0x49);
        packet.put_byte(1);
        for (int i = 0; i <= 3; ++i)
        {
            // ORIG 0x08634696：get_user ×3（每次调用点重取，P183 去缓存）。
            if (party->get_user(i) == 0)
                packet.put_short(-1);
            else
            {
                if (party->get_user(i)->getCurCharacTotalFatigue() ==
                    party->get_user(i)->getCurCharacTotalMaxFatigue())
                    packet.put_short(0);
                else
                    packet.put_short(1);
            }
        }
        packet.finalize(true);
        party->send_to_party(packet);
        party->set_dungeon_clear_state(0x04);
        party->insert_time_dungeon_score_scroll_msg(0x1e);
        party->insert_time_card_select_msg(0x04);
    }
    return 1;
}

TimerClearDungeonCardSelectRight::TimerClearDungeonCardSelectRight() {}
TimerClearDungeonCardSelectRight::~TimerClearDungeonCardSelectRight() {}
int TimerClearDungeonCardSelectRight::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x086347e2：GetParty(p1) + check_timer_key(0x13, p2)。
    CParty* party = G_CGameManager()->GetParty(p1);
    // 拆分独立早退（ORIG 0x086347e3：null 段 cmpl;jne 7f0+mov0 岛；
    // check 段 xor$1;test;je 818 前跳 + 811 mov0 岛）。
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)0x13, p2))
        return 0;
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
    // ORIG 0x0863485a：三段独立早退（jne 岛 + setne 物化 + xor$1），非 || 链。
    if (user == 0)
        return 0;
    if (static_cast<unsigned int>(user->get_acc_id()) != p3)
        return 0;
    if (!user->IsPremiumUser())
        return 0;
    WongWork::CHandlePremium::handlePremiumTimeout(user);   // ORIG 0x863485a
    return 1;
}

TimerPremiumServiceUpdate::TimerPremiumServiceUpdate() {}
TimerPremiumServiceUpdate::~TimerPremiumServiceUpdate() {}
int TimerPremiumServiceUpdate::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, 6);   // ORIG 第二实参为常量 6
    // ORIG 0x086348d4：三段独立早退。
    if (user == 0)
        return 0;
    if (static_cast<unsigned int>(user->get_acc_id()) != p3)
        return 0;
    if (!user->IsPremiumUser())
        return 0;
    user->GetPremiumInfoW()->ReCalcAdvantage();
    user->GetPremiumInfoW()->RecalcAdditionalInfo(user);
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
    int nextAt = static_cast<int>(mktime(&midnight));   // ORIG mov %eax,-0xc(%ebp)
    if (midnight.tm_hour >= 0)                          // ORIG mov -0x38(%ebp) test js
        nextAt += 0x15180;
    register int delay = nextAt - static_cast<int>(now);   // ORIG %ebx 直落 0x10(%esp)
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, p1, (TIMER_MESSAGE)5,
                                delay, p2, p3);
    return 1;
}

TimerPremiumServiceStart::TimerPremiumServiceStart() {}
TimerPremiumServiceStart::~TimerPremiumServiceStart() {}
int TimerPremiumServiceStart::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, 6);   // ORIG 第二实参为常量 6
    if (user == 0)
        return 0;
    if (static_cast<unsigned int>(user->get_acc_id()) != p3)
        return 0;
    if (!user->GetPremiumInfoW()->startPremium(p2, false))
    {
        return 0;
    }
    if (user->get_state() > 2)
    {
        // ORIG 通过 CUser::GetPremiumInfo()（const）取 CUserPremium*，再查指定类型信息。
        WongWork::SUserPremiumInfo* info =
            user->GetPremiumInfo()->GetPremiumInfo(p2);
        // ORIG 0x08634af4/0x08634b3b：两处正向早退 return 1（mov $1 岛内联）。
        if (info->m_type == 0)
            return 1;
        WongWork::CHandlePremium::handleNotifyPremiumInfo(
            user, info->m_type, info->m_end - info->m_start);
        if (g_SPremiumInfo[info->m_type].m_kind == 2)
            return 1;
        WongWork::CHandlePremium::handlePremiumEffect(
            user, static_cast<ENUM_PREMIUM_TYPE>(info->m_type));
    }
    return 1;
}

TimerCheckPaidPCRoomTerm::TimerCheckPaidPCRoomTerm() {}
TimerCheckPaidPCRoomTerm::~TimerCheckPaidPCRoomTerm() {}
int TimerCheckPaidPCRoomTerm::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, 6);   // ORIG 第二实参为常量 6
    // ORIG 报告 TimerCheckPaidPCRoomTerm/dispatch_sig.md：两段独立早退；
    // !isAffectedPremium()（bool 声明 ⇒ xor $0x1）先走 notify/return 0，
    // logout/login/return 1 位于尾部 je 目标块。
    if (user == 0)
        return 0;
    if (static_cast<unsigned int>(user->get_acc_id()) != p3)
        return 0;
    if (!user->isAffectedPremium(static_cast<ENUM_PREMIUM_TYPE>(p2)))
    {
        WongWork::CHandlePremium::handleNotifyPremiumInfo(user, p2, 0);
        return 0;
    }
    user->SendLogoutToPCRoomServer();
    user->SendLoginToPCRoomServer();
    return 1;
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
    // 拆分早退（ORIG 0x08634da5：cmpl;jne 前跳+mov0 岛；0x08634dc7 xor$1;test;je
    // 前跳 STATE + mov0 岛；ChangeState 后 mov$1 直落 EXIT）。
    WarRoom* room = G_CGameManager()->GetWarRoom(p1);
    if (room == 0)
        return 0;
    if (!room->CheckTimerKey((TIMER_MESSAGE)0x42, p2))
        return 0;
    room->ChangeState(WARROOM_STATE_3);
    return 1;
}

TimerWarRoomBattlePauseCount::TimerWarRoomBattlePauseCount() {}
TimerWarRoomBattlePauseCount::~TimerWarRoomBattlePauseCount() {}
int TimerWarRoomBattlePauseCount::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x08634df6：GetWarRoom(p1) + CheckTimerKey(0x43, p2)。
    // 拆分早退（同 ReadyLockCount，ORIG 0x08634e15/0x08634e37）。
    WarRoom* room = G_CGameManager()->GetWarRoom(p1);
    if (room == 0)
        return 0;
    if (!room->CheckTimerKey((TIMER_MESSAGE)0x43, p2))
        return 0;
    room->ChangeState(WARROOM_STATE_3);
    return 1;
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
    // 拆分独立早退（ORIG 0x08634ffx：null 段 jne 前跳岛 + check 段 xor$1;test;je 岛）。
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)0x16, p2))
        return 0;
    party->m_battleField.BloodSpawnTimerManager(
        false, true);   // ORIG：party+0xb24 即 CBattle_Field 子对象
    return 1;
}

TimerBloodPhaseRegister0::TimerBloodPhaseRegister0() {}
TimerBloodPhaseRegister0::~TimerBloodPhaseRegister0() {}
int TimerBloodPhaseRegister0::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)0x17, p2))
        return 0;
    short nextRound =
        reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnCnt[0];
    short* spawnTime =
        reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnTime;
    reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.BloodPhaseSpawn(
        nextRound, *spawnTime, 0x17, 0);
    return 1;
}

TimerBloodPhaseRegister1::TimerBloodPhaseRegister1() {}
TimerBloodPhaseRegister1::~TimerBloodPhaseRegister1() {}
int TimerBloodPhaseRegister1::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)0x18, p2))
        return 0;
    short nextRound =
        reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnCnt[1];
    short* spawnTime =
        &reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnTime[1];
    reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.BloodPhaseSpawn(
        nextRound, *spawnTime, 0x18, 1);
    return 1;
}

TimerBloodPhaseRegister2::TimerBloodPhaseRegister2() {}
TimerBloodPhaseRegister2::~TimerBloodPhaseRegister2() {}
int TimerBloodPhaseRegister2::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)0x19, p2))
        return 0;
    short nextRound =
        reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnCnt[2];
    short* spawnTime =
        &reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnTime[2];
    reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.BloodPhaseSpawn(
        nextRound, *spawnTime, 0x19, 2);
    return 1;
}

TimerBloodPhaseRegister3::TimerBloodPhaseRegister3() {}
TimerBloodPhaseRegister3::~TimerBloodPhaseRegister3() {}
int TimerBloodPhaseRegister3::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)0x1a, p2))
        return 0;
    short nextRound =
        reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnCnt[3];
    short* spawnTime =
        &reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnTime[3];
    reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.BloodPhaseSpawn(
        nextRound, *spawnTime, 0x1a, 3);
    return 1;
}

TimerBloodPhaseRegister4::TimerBloodPhaseRegister4() {}
TimerBloodPhaseRegister4::~TimerBloodPhaseRegister4() {}
int TimerBloodPhaseRegister4::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)0x1b, p2))
        return 0;
    short nextRound =
        reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnCnt[4];
    short* spawnTime =
        &reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnTime[4];
    reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.BloodPhaseSpawn(
        nextRound, *spawnTime, 0x1b, 4);
    return 1;
}

TimerBloodPhaseRegister5::TimerBloodPhaseRegister5() {}
TimerBloodPhaseRegister5::~TimerBloodPhaseRegister5() {}
int TimerBloodPhaseRegister5::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)0x1c, p2))
        return 0;
    short nextRound =
        reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnCnt[5];
    short* spawnTime =
        &reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnTime[5];
    reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.BloodPhaseSpawn(
        nextRound, *spawnTime, 0x1c, 5);
    return 1;
}

TimerBloodPhaseRegister6::TimerBloodPhaseRegister6() {}
TimerBloodPhaseRegister6::~TimerBloodPhaseRegister6() {}
int TimerBloodPhaseRegister6::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)0x1d, p2))
        return 0;
    short nextRound =
        reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnCnt[6];
    short* spawnTime =
        &reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnTime[6];
    reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.BloodPhaseSpawn(
        nextRound, *spawnTime, 0x1d, 6);
    return 1;
}

TimerBloodPhaseRegister7::TimerBloodPhaseRegister7() {}
TimerBloodPhaseRegister7::~TimerBloodPhaseRegister7() {}
int TimerBloodPhaseRegister7::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)0x1e, p2))
        return 0;
    short nextRound =
        reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnCnt[7];
    short* spawnTime =
        &reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnTime[7];
    reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.BloodPhaseSpawn(
        nextRound, *spawnTime, 0x1e, 7);
    return 1;
}

TimerBloodPhaseRegister8::TimerBloodPhaseRegister8() {}
TimerBloodPhaseRegister8::~TimerBloodPhaseRegister8() {}
int TimerBloodPhaseRegister8::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)0x1f, p2))
        return 0;
    short nextRound =
        reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnCnt[8];
    short* spawnTime =
        &reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnTime[8];
    reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.BloodPhaseSpawn(
        nextRound, *spawnTime, 0x1f, 8);
    return 1;
}

TimerBloodPhaseRegister9::TimerBloodPhaseRegister9() {}
TimerBloodPhaseRegister9::~TimerBloodPhaseRegister9() {}
int TimerBloodPhaseRegister9::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)0x20, p2))
        return 0;
    short nextRound =
        reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnCnt[9];
    short* spawnTime =
        &reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnTime[9];
    reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.BloodPhaseSpawn(
        nextRound, *spawnTime, 0x20, 9);
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
    party->m_battleField.BloodCrazyMonsterSpawnPacket(
        );   // ORIG：party+0xb24 子对象
    return 1;
}

TimerBloodDungeonClearReward::TimerBloodDungeonClearReward() {}
TimerBloodDungeonClearReward::~TimerBloodDungeonClearReward() {}
int TimerBloodDungeonClearReward::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x08638cd0：GetParty(p1)；check_timer_key 以 p3 为 msg、p2 为 param。
    CParty* party = G_CGameManager()->GetParty(p1);
    // 拆分三段独立早退（ORIG 0x08638cd5 jne 岛 / 0x08638cf7 xor$1;test;je /
    // 0x08638d17 同形，je→8d25 mov$1 尾岛）。
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)p3, p2))
        return 0;
    if (!party->onBloodRewardTimer((TIMER_MESSAGE)p3))
        return 0;
    return 1;
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
    if (party->m_battleField.m_pDungeon == 0 ||
        party->m_battleField.m_pDungeon->get_blood_dungeon_type() != 2)
    {
        return 0;
    }
    CBattle_Field* field = &party->m_battleField;   // ORIG：party+0xb24 子对象
    if (field->getUltimateDifficylty() != 0)
    {
        return 0;
    }
    const int diff = field->getNextRoundRandomDifficulty();
    field->setUltimateDifficulty(static_cast<char>(diff));
    // P48 no-CSE：ORIG 两次 fresh getBloodRoundOrder()（比较用 + send 实参用）
    if (static_cast<short>(field->m_bloodRound.getBloodRoundOrder()) != field->getBloodMaxRound() - 1)
    {
        field->sendUltimateRandomDifficulty(
            static_cast<short>(field->m_bloodRound.getBloodRoundOrder()), diff);
    }
    field->startUltimateRound();
    return 1;
}
TimerEventChristmas::TimerEventChristmas() {}
TimerEventChristmas::~TimerEventChristmas() {}
int TimerEventChristmas::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 三方法为静态语义（调用点无 this 压栈，直接操作全局 0x943c138），
    // 静态成员直调（ORIG 8635710/863571b/8635726 无 temp 对象槽，帧 0x18）。
    CServerEvent::SubExpRate(p2);
    CServerEvent::TurnOffEvent(p3);
    // ORIG：结果不落实名槽，直接 movswl %ax,%edx 寄存器扩展后入参。
    Inter_MonitorServerEventStart::SendEventNotiPacket(false,
                                                        static_cast<short>(p3),
                                                        static_cast<short>(CServerEvent::GetEventInterval(p3)), 0);
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
    // ORIG 0x086358c0：CalcNextTime 结果 `mov %eax,%ebx` 驻寄存器，
    // N96/P0：asm volatile 同禁。自然两段式（ORIG 寄存器原地 sub 直落残差
    // 记 N18a 族，槽化 store/load 为 4.4 常态）。
    int delay = CalcNextTime();
    delay -= static_cast<int>(GlobalData::s_systemTime_.getCurSec());
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
    // 第一段：LOGDB_Q(4)，值 0x7b / -1 —— ORIG 0x08635a60 段：Acquire 免槽直落
    // guard ctor、显式 operator*() 代理链、guard1 槽 -0x10 + {} 块界 dtor/catch
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("TimerDispatcher.cpp", 0x97b), true);
        guard.operator*()->operator<<((int)0x7b);
        guard.operator*()->operator<<((int)-1);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    }

    // 第二段：DB_Q(2)，值 0x7c / -1（ORIG guard2 槽 -0x18 与 guard1 相邻）
    {
        CStreamGuard guard2(GlobalData::s_stream_pool->Acquire("TimerDispatcher.cpp", 0x981), true);
        guard2.operator*()->operator<<((int)0x7c);
        guard2.operator*()->operator<<((int)-1);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard2);
    }

    RegistNextTimer();
    return 1;
}

// ORIG T 0x8635bdc（RegistNextTimer.md 全文转写；声明已在 TimerDispatcher.h，
// 签名 static void；ORIG 恒返回 eax=1，void 化由现有头口径决定）
// ORIG T 0x08635bdc：InsertTimer(2,0,0x59,0x708,0,0)；返回 1。
int TimerQueryCounter::RegistNextTimer()
{
    // ORIG：delay 先物化到局部槽 -0xc(%ebp)（movl $0x708,-0xc），再两步入栈。
    int delay = 0x708;
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x59,
                                delay, 0, 0);
    return 1;
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
int TimerNatType::RegistNextTimer()
{
    // ORIG 先把 delay 常量暂存 -0xc(%ebp) 再装入实参槽（gcc4.1 形态），
    // 用局部变量复现该栈槽暂存（同 TimerFatigueReset 先例）。
    int delay = 0x15180;
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x5a, delay, 0, 0);
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

// ORIG T 0x08635cd0 (_ZN16TimerNatTypeUser15RegistNextTimerEv)：
// G_TimerQueue()->InsertTimer(2, 0, 0x5b, 0xe10, 0, 0)；返回 1。
int TimerNatTypeUser::RegistNextTimer()
{
    // ORIG 先把 delay 常量暂存 -0xc(%ebp) 再装入实参槽（gcc4.1 形态），
    // 用非 const 局部变量复现该栈槽暂存（const 会被 4.4 折叠为立即数）。
    int delay = 0xe10;
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x5b, delay, 0, 0);
    return 1;
}

TimerCheckMailTimeOut::TimerCheckMailTimeOut() {}
TimerCheckMailTimeOut::~TimerCheckMailTimeOut() {}
int TimerCheckMailTimeOut::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x08635d0f：单出口 result(%ebx) + 四段判错，各 setXX %al 物化
    // （0x08635d9x `cmp $0x1,%eax; setle` / `cmp 0x10,%eax; setne` / `cmp 0x14,%eax; setne`）
    // → `test %al,%al; je` 前跳；result=1 在 PacketGuard dtor(0x08635e64) 之前。
    // N96/P0：绑定形已废，自然具名局部（setXX 物化链残差记 N85/setcc-fold）。
    int result;
    bool bad;
    CUser* user = G_CGameManager()->getUser(p1, 7);   // ORIG 第二实参为常量 7
    if (user == 0) {
        result = 0;
    } else {
        bad = user->get_state() <= 1;
        if (bad) {
            result = 0;
        } else {
            bad = user->GetLoginTick() != p2;
            if (bad) {
                result = 0;
            } else {
                bad = static_cast<unsigned int>(user->getCurCharacNo()) != p3;
                if (bad) {
                    result = 0;
                } else {
                    PacketGuard packet;
                    packet.put_header(0, 0x62);
                    int countIndex = packet.get_index();
                    packet.put_int(0);
                    int expired = static_cast<int>(WongWork::CMailBoxHelper::CheckMailTimeout(user, &packet));
                    if (expired != 0)
                    {
                        packet.put_int(countIndex, expired);
                        packet.finalize(true);
                        user->Send(packet);
                    }
                    WongWork::CMailBoxHelper::SetPeriodCheckTimer(user);
                    result = 1;
                }
            }
        }
    }
    return result;
}

TimerCreature::TimerCreature() {}
TimerCreature::~TimerCreature() {}
int TimerCreature::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x08635e96：getUser 第二实参为常量 1（movl $0x1,0x8(%esp)）；
    // 三段独立早退（jne 岛 + 两条 cmp/setne 物化，ORIG +0x29/+0x46/+0x62）。
    CUser* user = G_CGameManager()->getUser(p1, 1);
    if (user == 0)
        return 0;
    if (user->get_unique_id() != p2)
        return 0;
    if (user->getCurCharacNo() != p3)
        return 0;
    // CheckDiedCreature 是 user_creature::CCreatureMgr 成员（ORIG 0x08339acc）：
    // user->getCurCharacInvenW()->GetCreatureMgrW() 取管理器实例。
    // ORIG +0x99：CheckDiedCreature 结果物化 -0x11(%ebp)（4.1 无死码消除，具名复现）。
    // 技巧21：died 先声明占远槽 -0x11、saveInventory 后声明占近槽 -0x9（ORIG
    // 8635f11/8635f2f 槽号）；`x ^ 1` 具名 char 槽条件出 movzbl+xor$1+test+je
    // （ORIG 8635f32，cmpb 融合形被 ^ 表达式阻断）。
    bool died;
    // ORIG 0x08635eeb/0x08635f08：isSaveInven 先调（byte 槽 -0x9），
    // CheckDiedCreature 后调（-0x11，4.1 两 byte 槽间留隙）
    const char saveInventory = user->isSaveInven();
    died = user->getCurCharacInvenW()->GetCreatureMgrW()->CheckDiedCreature();
    (void)died;
    // ORIG 8635f32 `movzbl -0x9,%eax; xor $1,%eax; test %al; je`：
    // N96/P0：绑定形全域禁用后无合法源形可阻 fold-const（探针 /tmp/l7_m.cpp
    // m1/m3 恒折 `cmpb $1,-0x9; je`；register-hint n1 形溢入 %ebx 改帧）——
    // setcc-fold 负例在册，取最短 gap 折叠形，残差 3 行封顶。
    if ((unsigned char)(saveInventory ^ 1))
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
    // ORIG 0x08635f50：name 临时跨调用驻留 %ebx（register），且每个使用点
    // 各自重新调用 get_charac_name_by_charac_no（真/假分支+首 trace 共 3 次）。
    register const char* name = user->get_charac_name_by_charac_no(p3);
    cMyTrace(__PRETTY_FUNCTION__, 0xa52, 0)("TimerForbidChat %s", name);
    name = user->get_charac_name_by_charac_no(p3);
    // ORIG：`!EnableChat` 前置（xor$1/test），Failed 块 fall-through、Enabled 后置。
    if (!G_GameWorld()->EnableChat(name))
    {
        name = user->get_charac_name_by_charac_no(p3);
        cMyTrace(__PRETTY_FUNCTION__, 0xa54, 0)("Failed to enable to chat for: %s", name);
    }
    else
    {
        name = user->get_charac_name_by_charac_no(p3);
        cMyTrace(__PRETTY_FUNCTION__, 0xa56, 0)("Enabled to chat for: %s", name);
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
    // 与 TimerForbidChat 同构（ORIG 0x086360a8）：register name→%ebx、
    // 各点重新调用、!EnableMove 前置、Failed fall-through。
    register const char* name = user->get_charac_name_by_charac_no(p3);
    cMyTrace(__PRETTY_FUNCTION__, 0xa64, 0)("TimerForbidMove %s", name);
    name = user->get_charac_name_by_charac_no(p3);
    if (!G_GameWorld()->EnableMove(name))
    {
        name = user->get_charac_name_by_charac_no(p3);
        cMyTrace(__PRETTY_FUNCTION__, 0xa66, 0)("Failed to enable to move for: %s", name);
    }
    else
    {
        name = user->get_charac_name_by_charac_no(p3);
        cMyTrace(__PRETTY_FUNCTION__, 0xa68, 0)("Enabled to move for: %s", name);
    }
    return 1;
}

TimerScheduleFiveMinPerUser::TimerScheduleFiveMinPerUser() {}
TimerScheduleFiveMinPerUser::~TimerScheduleFiveMinPerUser() {}
int TimerScheduleFiveMinPerUser::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, 3);   // ORIG 第二实参为常量 3
    // ORIG 0x086341d2：三段独立早退；第三段 && 经 (bool) 物化（+0x57..+0x7b 岛形）。
    if (user == 0)
        return 0;
    if (user->GetLoginTick() != p2)
        return 0;
    if ((bool)(p3 != 0 && static_cast<unsigned int>(user->getCurCharacNo()) != p3))
        return 0;
    user->WorkPerFiveMin();
    // ORIG 0x0863425f..：右到左求值，RCR 依次落 %edi(no)/%esi(tick)/%ebx(uid)
    // （4.4 自由分配先选 %ebx，asm 绑名复现）。
    register unsigned int characNo = user->getCurCharacNo();
    register unsigned int tick = user->GetLoginTick();
    register int uid = user->GetUID();
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, uid,
                                (TIMER_MESSAGE)0x5f, 300, tick, characNo);
    return 1;
}

TimerScheduleTenMinPerUser::TimerScheduleTenMinPerUser() {}
TimerScheduleTenMinPerUser::~TimerScheduleTenMinPerUser() {}
int TimerScheduleTenMinPerUser::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, 3);   // ORIG 第二实参为常量 3
    // ORIG 0x08634106（报告 TimerScheduleTenMinPerUser/dispatch_sig.md）：同 Twin 形。
    if (user == 0)
        return 0;
    if (user->GetLoginTick() != p2)
        return 0;
    if ((bool)(p3 != 0 && static_cast<unsigned int>(user->getCurCharacNo()) != p3))
        return 0;
    user->WorkPerTenMin();
    // 同 FiveMin 孪生（ORIG 0x08634106）。
    register unsigned int characNo = user->getCurCharacNo();
    register unsigned int tick = user->GetLoginTick();
    register int uid = user->GetUID();
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, uid,
                                (TIMER_MESSAGE)0x60, 600, tick, characNo);
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
        // ORIG 0x08636246：Acquire 结果 %eax 直落 guard ctor 0x4(%esp)（无 stream 槽）。
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("TimerDispatcher.cpp",
                                                              0xa7f), true);
        // ORIG 0x08636262..：每项 `*guard`（operator* deEv 0x080c8c46）后 lsEi
        // （0x0863626d/288/29e/2c7 各一次 lea+call deEv），非链式折叠。
        (*guard)->operator<<(0x9b);
        (*guard)->operator<<(-1);
        (*guard)->operator<<(remaining);
        (*guard)->operator<<(G_CEnvironment()->get_channel_no());
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    }
    // ORIG 0x0863631c：`if(!RegistNextTimer()) return 0; return 1;`
    // bool 结果落槽 -0x9(%ebp)（0x08636327 mov %al,-0x9），movzbl+xor$1 尾岛。
    const bool ok = RegistNextTimer();
    if (!ok)
        return 0;
    return 1;
}

// ORIG T 0x08636348 (_ZN17TimerQueueSizeLog15RegistNextTimerEv)：
// G_TimerQueue()->InsertTimer(2, 0, 0x61, 0x3c, 0, 0)；返回 1。
bool TimerQueueSizeLog::RegistNextTimer()
{
    // ORIG：delay 物化到局部槽 -0xc(%ebp) 再两步入栈。
    int delay = 0x3c;
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x61, delay, 0, 0);
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
    // ORIG 0x0863674a-0x08636757：Acquire 免槽直落 ctor；guard 全程作用域
    // （dtor 在 updateDataIntoDB/Regist 之后，catch @0fc）；写流走显式
    // operator*() 代理链；GetInBuffer 经 operator->()（ORIG 0x086367a2 ptEv call）
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("TimerDispatcher.cpp", 0xb1a), true);
    guard.operator*()->operator<<((int)0xaf);
    guard.operator*()->operator<<((int)-1);
    SIG_DUNGEON_DATA_STATISTIC* stat =
        guard.operator->()->GetInBuffer<SIG_DUNGEON_DATA_STATISTIC>();
    memset(stat, 0, 0x80);
    *(int*)stat = 3;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    GetInstanceVendingMachineStatistic()->updateDataIntoDB();
    // ORIG 0x0863680a-0x08636824：Regist byte 槽 -0x9 物化 + `xor $0x1; test; je`
    // 双岛直存 %ebx 跨 dtor（绑定消 bool byte 槽），单出口 return %ebx
    bool ok = RegistNextTimer();
    int ret;   // N96/P0：绑定形已废
    if (ok != 1)
        ret = false;
    else
        ret = true;
    return ret;
}

// ORIG T 0x08636856 (_ZN25TimerDungeonDataStatistic15RegistNextTimerEv)
// ORIG 非 static（调用点传 this）返回 bool（ORIG T 0x08636856）
bool TimerDungeonDataStatistic::RegistNextTimer()
{
    // ORIG 0x086368xx：curSec 单 time_t 槽（免 int→long 拷贝）；localtime_r 返回
    // pt 落槽 -0x18，各字段经 `mov -0x18,%eax` 重读 + 指针 disp 读写（技巧37 反用：
    // 具名指针槽正是 ORIG 形）
    time_t curSec = GlobalData::s_systemTime_.getCurSec();
    tm t;
    tm* pt = localtime_r(&curSec, &t);
    const int curHour = pt->tm_hour;
    pt->tm_hour = 5;
    pt->tm_min = 0;
    pt->tm_sec = 0;
    // ORIG 0x086368a7-0x086368e1：chn 全程 %ecx、x=chn/30（0x88888889+sar4 魔数）
    // 存 %ebx（sign 修正后）、2x=%edx；`shl $4; sub %edx` =30x；
    // tm_min = chn-30x = chn%30（P196：旧 /15*45 形在 chn≥15 分叉系误判）。
    // N96/P0：绑定形已废（槽序归位残差记 N82/P112）。
    int chn = G_CEnvironment()->m_serverEnvir.m_channelNo; // +0x1b0
    int x = chn / 30;
    int x2 = x * 2;
    pt->tm_min = chn - (x2 * 16 - x2);
    int target = static_cast<int>(mktime(pt));
    if (pt->tm_hour <= curHour)
        target += 0x15180;
    const int delay = target - static_cast<int>(curSec);
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
    // N96/P0：uchar callee 无槽折叠形（probe /tmp/l7_m.cpp：4.4 恒折
    // cmpb$1 直判，xor 寄存器物化属 setcc-fold 负例）。
    if ((unsigned char)(user->IsRecvEvent() ^ 1))
    {
        // ORIG 0x086369e6：DisConnSig(0x1d, false, 0) 断线
        user->DisConnSig((DISCONN_SIG)0x1d, false, 0);
        char ip[20];
        ip[0] = '\0';
        ip[1] = '\0';
        ip[2] = '\0';
        ip[3] = '\0';
        ip[4] = '\0';
        ip[5] = '\0';
        ip[6] = '\0';
        ip[7] = '\0';
        ip[8] = '\0';
        ip[9] = '\0';
        ip[10] = '\0';
        ip[11] = '\0';
        ip[12] = '\0';
        ip[13] = '\0';
        ip[14] = '\0';
        ip[15] = '\0';
        ip[16] = '\0';
        ip[17] = '\0';
        ip[18] = '\0';
        ip[19] = '\0';
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
    std::vector<stLightServerNotice_t>& notices =
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
    // ORIG 0x08636b50：先 put_int(len) 再 put_str
    packet.put_int(len);
    packet.put_str(notices[p3].m_message.c_str(), len);
    packet.finalize(true);
    G_GameWorld()->send_all(guard);
    const bool done = RegistNextTimer(static_cast<int>(p3));
    // N96/P0：绑定形已废；具名 bool done 槽 + 自然 if(!done)（4.4 恒折
    // cmpb 直判，ORIG movzbl+xor 寄存器物化属 setcc-fold 负例在册）。
    int ret;
    if (!done)
        ret = 0;
    else
        ret = 1;
    return ret;
}

// ORIG T 0x08636c14（_ZN22TimerLightServerNotice15RegistNextTimerEi）
bool TimerLightServerNotice::RegistNextTimer(int idx)
{
    // ORIG：localtime_r 返回值 pt 具名存槽 -0x18，字段读写全经 pt 指针
    // （`mov -0x18,%eax; mov %edx,0x8(%eax)` 形），非 cur 折叠；curSec 槽 -0x1c。
    // G_CDataManager()+0xa4 的 vector operator[] 按 P48 每用点重取（tm_hour/tm_min
    // 各自独立 operator[] + G_CDataManager 调用），禁缓存 notice 引用。
    time_t curSec = static_cast<time_t>(GlobalData::s_systemTime_.getCurSec());
    tm cur;
    tm* pt = localtime_r(&curSec, &cur);
    int prevHour = pt->tm_hour;
    // P48：两处各自主调用 G_CDataManager()+0xa4 operator[]（ORIG 双调用）。
    pt->tm_hour = G_CDataManager()->m_serverParameter.m_light3c[idx].m_hour;
    pt->tm_min = G_CDataManager()->m_serverParameter.m_light3c[idx].m_min;
    pt->tm_sec = 0;
    int next = static_cast<int>(mktime(pt));
    if (pt->tm_hour <= prevHour)
    {
        next += 0x15180;
    }
    int delay = static_cast<int>(next - curSec);
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
    // ORIG +0x1e..：bool 结果落 -0x9 槽，reload+xor$1（Midnight 同形，探针 t5）。
    const bool registered = RegistNextTimer();
    if (!registered)
        return 0;
    return 1;
}

// ORIG T 0x08636d44（_ZN23TimerInsertUserPlayTime15RegistNextTimerEv）
bool TimerInsertUserPlayTime::RegistNextTimer()
{
    // ORIG 0x08636：curSec 落槽 -0x1c（time_t 直传 &）；localtime_r 返回值具名 pt
    // 槽 -0x18；prevHour -0x14 / next -0x10；全部字段读写经 pt 指针
    // （`mov -0x18,%eax; movl $0x4,0x8(%eax)` 形，ORIG 0x08636xxx），非 cur 折叠。
    time_t curSec = static_cast<time_t>(GlobalData::s_systemTime_.getCurSec());
    tm cur;
    tm* pt = localtime_r(&curSec, &cur);
    int prevHour = pt->tm_hour;
    pt->tm_hour = 4;
    pt->tm_min = 0x1e;
    pt->tm_sec = 0;
    int next = static_cast<int>(mktime(pt));
    if (pt->tm_hour <= prevHour)
    {
        next += 0x15180;
    }
    int delay = static_cast<int>(next - curSec);
    G_TimerQueue()->InsertTimer(static_cast<TimerEntry::OBJ_TYPE>(2), 0,
                                static_cast<TIMER_MESSAGE>(0x69), delay, 0, 0);
    return 1;
}

TimerCheckForcedDisconnect::TimerCheckForcedDisconnect() {}
TimerCheckForcedDisconnect::~TimerCheckForcedDisconnect() {}
// ---- TimerCheckForcedDisconnect ----
// 报告：docs/class_func_reports/TimerCheckForcedDisconnect/{dispatch_sig,RegistNextTimer}.md

// ORIG T 0x08636e06 (_ZN26TimerCheckForcedDisconnect12dispatch_sigEiij)
// 参数语义（实测 2026-08-30）：getUser(p1, 0x2)，比较 unique_id!=p2；
// NEW 曾误用 p2/p3，已修正。
int TimerCheckForcedDisconnect::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x08636e06：结果驻留 %ebx；四段 else-if 链（jne 岛 mov$0/%1）；
    // RegistNextTimer 为成员调用（+0xca mov 0x8(%ebp)=this；+0xd5 bool 落 -0x9）。
    register int result;
    CUser* user = G_CGameManager()->getUser(p1, 0x2);
    if (user == 0)
    {
        result = 0;
    }
    else if (user->get_unique_id() != p2)
    {
        result = 0;
    }
    else if (user->get_state() != 7)
    {
        result = 1;
    }
    else
    {
        PacketGuard packet;
        packet.put_header(0, 0x7b);
        packet.finalize(true);
        user->Send(packet);
        const bool registered = RegistNextTimer(user);
        if (!registered)
            result = 0;
        else
            result = 1;
    }
    return result;
}

// ORIG T 0x08636f26 (_ZN26TimerCheckForcedDisconnect15RegistNextTimerEP5CUser)
// 头声明为 static bool；ORIG eax 恒为 1，语义等价。
// TimerDispatcher.h 现签名 `static bool RegistNextTimer(CUser*)` 即可，无需改动。
bool TimerCheckForcedDisconnect::RegistNextTimer(CUser* user)
{
    // ORIG 0x08636f26：delay 先落 -0xc 槽（movl $0xa,-0xc）；两次 call 结果
    // 直落 %esi/%ebx（register 形态，探针 x10 r1）。
    int delay = 10;
    register int uid16 = static_cast<unsigned short>(user->get_unique_id());   // ORIG movzwl %ax,%esi 定义处零扩展
    register unsigned int uid = user->GetUID();
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, uid,
                                (TIMER_MESSAGE)0x6a, delay, uid16, 0);
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
    // ORIG +0x90: !IsEndPvpBattle()（bool 声明 ⇒ xor $0x1,%eax；PvPR 同步 .h/.cpp）
    if (!room->IsEndPvpBattle())
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
    // ORIG 0x0863703a：GetPvp 首参=0xc(%ebp)=p1；timer key 比较=0x10(%ebp)=p2
    // （旧版误用 p2/p3，Ghidra 参数标注偏移不可信，以 asm 为准）。
    PvP_Room* room = G_CGameManager()->GetPvp(p1, NULL, 0x3d);
    if (room == NULL)
        return 0;
    if (room->get_timer_key((TIMER_MESSAGE)0x3d) != p2)
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

    if ((unsigned int)user->getHumanCertifyTimerKey() > (unsigned int)p2)
    {
        return 1;
    }

    unsigned int point = user->m_mcap.getPoint();
    user->m_historyLog.RequestCleanPad(point, '\x03');

    for (int i = 0; i < 10; ++i)
    {
        local_24[i] = 0;
    }

    // P168a①：bool callee 一律裸形（技巧1：`!boolcall()` 直产 xor$1;test;je）
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
    // ORIG 0x08637170：两级链式（%eax 直落 (%esp)，无具名槽），帧 0x18；
    // getCurSec 结果寄存器内 add $0x15180 后直传 registNextTimer。
    G_CDataManager()->getItemGenerator()->onTimerEvent(0x6b);
    registNextTimer(GlobalData::s_systemTime_.getCurSec() + 0x15180);
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
    registNextTimer(GlobalData::s_systemTime_.getCurSec() + 0x15180);
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
    // ORIG 0x086373f2：getCurSec 两独立重调（P48）；delay 无槽经 %ebx 跨
    // G_TimerQueue call（P179 register 形，禁栈槽——否则 tm 帧偏移+4 全错位）。
    register int delay =
        (sec - (long)GlobalData::s_systemTime_.getCurSec() > 0)
            ? (int)(sec - (long)GlobalData::s_systemTime_.getCurSec())
            : 1;
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x6c, delay, 0, 0);
}

TimerDetectDisconnect::TimerDetectDisconnect() {}
TimerDetectDisconnect::~TimerDetectDisconnect() {}
int TimerDetectDisconnect::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x86374bc: GetInstanceDisconnectDetecter()->DoDetectingDisconnect() 为真时 registNextTimer(); return 1
    if (disconnect_detecter::GetInstanceDisconnectDetecter()->DoDetectingDisconnect()) {
        registNextTimer();
    }
    return 1;
}

TimerCompleteLoadAssaultTimeout::TimerCompleteLoadAssaultTimeout() {}
TimerCompleteLoadAssaultTimeout::~TimerCompleteLoadAssaultTimeout() {}
int TimerCompleteLoadAssaultTimeout::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x863752c: GetInstanceAssaultMgr() -> OnTimeoutCompleteLoad(p1,p2), return 1
    // ORIG：p1 先物化到 callee-saved %ebx（跨 GetInstance 调用存活），mgr 不落槽。
    register unsigned int partyIdx = (unsigned int)p1;
    ((pvp_assault::CAssaultMgr*)pvp_assault::GetInstanceAssaultMgr())
        ->OnTimeoutCompleteLoad(partyIdx, p2);
    return 1;
}

TimerConnectP2PAssaultTimeout::TimerConnectP2PAssaultTimeout() {}
TimerConnectP2PAssaultTimeout::~TimerConnectP2PAssaultTimeout() {}
int TimerConnectP2PAssaultTimeout::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x863759c: GetInstanceAssaultMgr() -> OnTimeoutConnectP2P(p1,p2), return 1
    register unsigned int partyIdx = (unsigned int)p1;
    ((pvp_assault::CAssaultMgr*)pvp_assault::GetInstanceAssaultMgr())
        ->OnTimeoutConnectP2P(partyIdx, p2);
    return 1;
}

TimerEndAssault::TimerEndAssault() {}
TimerEndAssault::~TimerEndAssault() {}
int TimerEndAssault::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x863760c: GetInstanceAssaultMgr() -> OnTimeoutEndAssault(p1,p2), return 1
    register unsigned int partyIdx = (unsigned int)p1;
    ((pvp_assault::CAssaultMgr*)pvp_assault::GetInstanceAssaultMgr())
        ->OnTimeoutEndAssault(partyIdx, p2);
    return 1;
}

TimerCloseAssault::TimerCloseAssault() {}
TimerCloseAssault::~TimerCloseAssault() {}
int TimerCloseAssault::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x863767c: GetInstanceAssaultMgr() -> OnTimeoutCloseAssault(p1,p2), return 1
    register unsigned int partyIdx = (unsigned int)p1;
    ((pvp_assault::CAssaultMgr*)pvp_assault::GetInstanceAssaultMgr())
        ->OnTimeoutCloseAssault(partyIdx, p2);
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
    // ORIG 0x86376ec: GetInstanceAssaultMgr() -> OnTimeoutCompleteLoadAfterAssault(p1,p2), return 1
    register unsigned int partyIdx = (unsigned int)p1;
    ((pvp_assault::CAssaultMgr*)pvp_assault::GetInstanceAssaultMgr())
        ->OnTimeoutCompleteLoadAfterAssault(partyIdx, p2);
    return 1;
}

TimerConnectP2PAfterAssaultTimeout::TimerConnectP2PAfterAssaultTimeout() {}
TimerConnectP2PAfterAssaultTimeout::~TimerConnectP2PAfterAssaultTimeout() {}
int TimerConnectP2PAfterAssaultTimeout::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x863775c: GetInstanceAssaultMgr() -> OnTimeoutConnectP2PAfterAssault(p1,p2), return 1
    register unsigned int partyIdx = (unsigned int)p1;
    ((pvp_assault::CAssaultMgr*)pvp_assault::GetInstanceAssaultMgr())
        ->OnTimeoutConnectP2PAfterAssault(partyIdx, p2);
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
    // ORIG 0x08637d6b：mov 0x10(%ebp),%eax; cmp $0x1cd1,%eax; jne 尾。
    // N96/P0：绑定形已废，直读形参数（4.4 对形参比较恒折 mem cmp，残差
    // 记 N18b/P85 跨版本装载时刻）。
    if (itemIdx == 0x1cd1)
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
    // ORIG 0x08638xxx：room 单槽 -0xc，matcher 每次从 room+0x6e4 直读（三次
    // `mov -0xc,%eax; mov 0x6e4(%eax),%eax`，无具名槽）；虚表槽 +0x38，
    // 实参为 p2（0x10(%ebp)）非 p3。
    if (((TimerStartMatchingMatcherSlot*)room)->m_matcher == 0)
        return 0;
    ((TimerStartMatchingMatcherSlot*)room)->m_matcher->OnMatchRoom(room, p2);
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
    // ORIG 0x08637e56：实参为 0x10(%ebp)=p2（先前误用 p3，已修正）。
    GlobalData::s_globalEffectManager_->beginReservedEffect(p2);
    return 1;
}

TimerGlobalEffectItemTimeout::TimerGlobalEffectItemTimeout() {}
TimerGlobalEffectItemTimeout::~TimerGlobalEffectItemTimeout() {}
int TimerGlobalEffectItemTimeout::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x08637ec2：getCurSec 结果直落 %ecx 跨后续求值（无名变量槽）。
    // 且第一实参为 0x10(%ebp)=p2（Ghidra 标 param_3 不准，asm 实证）。
    GlobalData::s_globalEffectManager_->checkTimeout(
        p2, GlobalData::s_systemTime_.getCurSec());
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
    // ORIG 0x08637f40：结果驻留 %ebx；true 块 `result=1` 先于 dtor，
    // false 块 mov $0 紧跟 cmp（无提前初始化）。
    // 4.4 布局：`if (user == 0)` 写守卫（jne 跳过 inline mov$0 岛）复现 ORIG；
    // `if (user != 0)` 则 je 跳后置岛。
    register int result;
    if (user == 0)
    {
        result = 0;
    }
    else
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
        result = 1;
    }
    return result;
}

TimerConnectMonitorServer::TimerConnectMonitorServer() {}
TimerConnectMonitorServer::~TimerConnectMonitorServer() {}
// ===== TimerConnectMonitorServer =====
// dispatch_sig ORIG _ZN25TimerConnectMonitorServer12dispatch_sigEiij @0x086380bc (0xe3B)
// registNextTimer ORIG _ZN25TimerConnectMonitorServer15registNextTimerElii @0x086381a0 (0x41B)

int TimerConnectMonitorServer::dispatch_sig(int /*p1*/, int p2, unsigned int /*p3*/)
{
    // ORIG 0x086380dc：Acquire 结果 %eax 直落 guard ctor 0x4(%esp)（无 stream 槽）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("TimerDispatcher.cpp",
                                                          0xeaa), true);

    // ORIG 0x086380f8/0x08638113：`*guard`（deEv 0x080c8c46）+ lsEi 两级。
    (*guard)->operator<<(0x121);
    (*guard)->operator<<(-1);

    // ORIG 0x0863812e/0x08638139：`guard->`（ptcEv 0x080c8c4e）+ GetInBuffer。
    Packet_Tcp_Server_Connect* pkt = guard->GetInBuffer<Packet_Tcp_Server_Connect>();
    // ORIG 0x08638144：值源为 p2（0x10(%ebp)），具名 char 字段恢复
    // `mov %dl,0xa(%eax)` disp 直存（0x0863814c）。
    pkt->m_serverFlag = static_cast<char>(p2);

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
    // ORIG 0x08634180：短路 && 物化 %al 双岛 merge（mov$1/mov$0 + test%al）。
    // N96/P0：绑定形已废；短路直判为 4.4 常态（4.1 merge 岛形属跨版本 bool
    // ABI 负例在册）。
    if (p3 != 0 &&
        static_cast<unsigned int>(user->getCurCharacNo()) != p3)
        return 0;
    user->SetCurCharacSuperState(false);
    pvp_assault::CAssaultPlace::SendUserState(user);
    return 1;
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
    reinterpret_cast<CBattle_Field*>(&party->m_battleField)
        ->check_elevator_timer();
    return 1;
}

TimerRefreshPowerWarProcessInfo::TimerRefreshPowerWarProcessInfo() {}
TimerRefreshPowerWarProcessInfo::~TimerRefreshPowerWarProcessInfo() {}
int TimerRefreshPowerWarProcessInfo::dispatch_sig(int p1, int p2, unsigned int p3)
{
    if (GlobalData::s_power_manager->IsPowerWarEventOn()) {
        // ORIG 0x086382xx：G_GameWorld 结果 %eax 直落 (%esp) 实参（无槽）。
        G_GameWorld()->OnRefreshPowerWarProcessInfo();
        // dm+0xa658 = m_powerParameter.m_refreshProcessInfo(+0x30)；ORIG 按 8 字节
        // long 读取（覆盖 +0x30..+0x37 连续两 int），保持块读语义。
        // ORIG 0x086382f0：`mov 0xa658(%eax),%eax` 单级字段读（int 值语义，
        // 4B 等宽；reinterpret long* 取址形会反产 add+mov 两级）。
        registNextTimer(G_CDataManager()->m_powerParameter.m_refreshProcessInfo);
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
        // N96/P0：uchar callee 无槽折叠形（xor 寄存器物化属 setcc-fold 负例）。
        if ((unsigned char)(picked->IsPowerUp() ^ 1))
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
    // ORIG 0x0863835a：size/GetPowerUpRemainTime/GetPowerUpTimerPeriod 均由 fresh
    // G_CDataManager()/s_power_manager 直取（P48 去缓存 pm/table）。两 bool 不可合并：
    // bVar1 门内块（IsPowerWarEventOn && cnt!=0，register 态即用），bVar2（槽 -0xd）
    // 门尾 registNextTimer，仅内块 cnt<=period+1 清 0——不受事件门影响（原实现误用
    // 单 active 使事件关时漏 registNextTimer，语义 bug）。
    const int cnt =
        static_cast<int>(G_CDataManager()->m_powerParameter.m_vec20.size());
    int remain = GlobalData::s_power_manager->GetPowerUpRemainTime();
    unsigned int period = static_cast<unsigned int>(
        GlobalData::s_power_manager->GetPowerUpTimerPeriod());
    bool bVar2 = true;
    if ((GlobalData::s_power_manager->IsPowerWarEventOn() && cnt != 0) &&
        static_cast<int>(period) < cnt)
    {
        remain -= 5;
        GlobalData::s_power_manager->SetPowerUpRemainTime(remain);
        const int notiSec =
            G_CDataManager()->m_powerParameter.m_vec20[static_cast<int>(period)];
        if (remain <= notiSec)
        {
            send_noti_time(notiSec);
            GlobalData::s_power_manager->SetPowerUpTimerPeriod(
                static_cast<int>(period) + 1);
            if (cnt <= static_cast<int>(period) + 1)
            {
                bVar2 = false;
                GlobalData::s_power_manager->SetPowerUpTimerPeriod(0);
            }
        }
    }
    if (bVar2)
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
        G_GameWorld()->OnBanAbuserFromPowerWar();
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
    // ORIG 0x08638ba0：独立早退（jne 岛）；比较对象 p2（0x10(%ebp)）；
    // get_unique_id unsigned short ⇒ movzwl %ax 直比，无 &0xffff。
    if (user == 0)
        return 0;
    if (user->get_unique_id() != p2)
        return 0;
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
    // ORIG 0x08638c48：两段独立早退；比较对象是 p2（0x10(%ebp)）非 p3；
    // get_unique_id 返回 unsigned short ⇒ movzwl %ax 后直比，无 &0xffff。
    if (user == 0)
        return 0;
    if (user->get_unique_id() != p2)
        return 0;
    user->SetTradePunishType(4);
    return 1;
}

TimerRevivalCharac::TimerRevivalCharac() {}
TimerRevivalCharac::~TimerRevivalCharac() {}
int TimerRevivalCharac::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x08638e78：单出口 result(%ebx) 链，三段 if/else-if；
    // 0x08638ed8 GetRevivalTime 比较对象为 p2（0x10(%ebp)），非 p3；
    // PacketGuard 于 else 块内构造+dtor（0x08638f39 call D1）后 result=1。
    // 技巧2：result 驻 %ebx（ORIG 0x08638ea5 `mov $0x0,%ebx`×3 / 0x08638f61
    // `mov %ebx,%eax` 单出口），PacketGuard 回 -0x18、帧 0x20。
    int result;   // N96/P0：绑定形已废（单出口 %ebx 驻留残差记 N82附）
    CUser* user = G_CGameManager()->getUser(p1, 2);   // ORIG 第二实参为常量 2
    if (user == 0) {
        result = 0;
    } else if (user->getCurCharacR() == 0) {
        result = 0;
    } else if (user->GetRevivalTime() != p2) {
        result = 0;
    } else {
        // ORIG 0x08638f34：result=1 在 PacketGuard dtor(0x08638f39) 之前（else 块内）。
        PacketGuard packet;
        packet.put_header(0, 0xe4);
        packet.finalize(true);
        user->Send(packet);
        result = 1;
    }
    return result;
}

TimerRequestExit::TimerRequestExit() {}
TimerRequestExit::~TimerRequestExit() {}
int TimerRequestExit::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, 2);   // ORIG 第二实参为常量 2
    // ORIG 0x08638f6a：三段独立早退；characR 判空 sete 物化（c1 探针形）；
    // characNo 比较对象 p2（0x10(%ebp)），混型走 mov edx;cmp 形态（c3 探针形）。
    if (user == 0)
        return 0;
    if (user->getCurCharacR() == 0)
        return 0;
    // ORIG 0x08638f3b：`mov 0x10(%ebp),%edx; cmp %edx,%eax`（call 结果 vs arg 两步形，
    // 4.4 恒折 `cmp $mem,%eax`（本轮复验 register asm("edx") 反产提前装载+拷贝链，
    // N18b/N22 定案）。
    if (user->getCurCharacNo() != p2)
        return 0;
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
    // ORIG 0x086391a3：`mov -0xc,%eax; movzwl 0xc4c(%eax),%eax; movzwl %ax,%eax;
    // cmp 0x14(%ebp),%eax` 单级折叠（view 嵌套成员链式，无 field 具名槽）。
    if (static_cast<unsigned short>(
            reinterpret_cast<CPartyBattleFieldView*>(party)
                ->m_battleField.m_villageMonsterId) != p3) {
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
    G_GameWorld()->ResetMidnight();
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
    // ORIG 0x08639554：curSec time_t 单槽 -0x3c（免 int→long 拷贝）；pt 槽 -0x28
    // 各点 reload；hourNow 槽 -0x24；sec/min/hour 直落 edi/esi/ebx（绑定，N18a
    // 两段残差）跨 cMyTrace ctor 存活
    time_t nowSec = GlobalData::s_systemTime_.getCurSec();
    std::tm timePoint;
    std::tm* pt = localtime_r(&nowSec, &timePoint);
    const int hourNow = pt->tm_hour;
    // N96/P0：绑定形已废（跨 call 保活残差记 N76/N82附）。
    int secNow = pt->tm_sec;
    int minNow = pt->tm_min;
    int hrNow = pt->tm_hour;
    cMyTrace trace(__PRETTY_FUNCTION__, 0x1293, 0);
    trace("Reset all user's fatigue!(%02d:%02d:%02d)", hrNow, minNow, secNow);
    pt->tm_hour = 0;
    pt->tm_min = 0;
    pt->tm_sec = 0;
    const long midnight = mktime(pt);
    const int nextAt = static_cast<int>(midnight + 0x15180);
    const int delay = nextAt - static_cast<int>(nowSec);
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
    // 其他值两分支都不执行。ORIG 形：bool ok=false（movb $0,-0x9）先行初始化，
    // 分支内 ok = RegistNextTimer(n)（mov %al,-0x9），尾部 reload+xor$1 物化返回。
    bool ok = false;
    if (p2 == 0)
    {
        GetInstanceValueStatistic()->InsertValueStatistic();
        ok = RegistNextTimer(1);
    }
    else if (p2 == 1)
    {
        GetInstanceCirculationStatistic()->InsertCirculationStatistic();
        ok = RegistNextTimer(0);
    }
    if (!ok)
        return 0;
    return 1;
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
    // ORIG 0x08639722：if/else 双分支各赋 result（三处 mov $N,%eax 汇聚单出口）。
    // 修正：ORIG 实为三段早退（team 空块置于函数尾），char 中间量落槽 -0x9，
    // `c == 1` 于 0/1 值取 xor $1/test/je 形态。
    // ORIG 块序：registered-false 岛(8639769) → team-null 岛(8639770) →
    // return-1 尾岛(8639777 fall-through leave)。
    // 技巧23 三 goto 钉岛序：rFalse → null → return1 尾岛 fall-through。
    if (team != 0)
    {
        team->saveData();
        const bool registered = RegistNextTimer(p2);
        if (!registered)
            goto RET0A;
        goto RET1;
    }
    goto RET0B;
    // [N 封顶] 三岛块序与 ORIG 逐岛对齐（rFalse→null→ret1）；4.4 对 je/jmp 目标
    // 岛前发 1 字节 nop（跨版本 jump-threading/空岛填充），共享 RET0 形更劣化块序。
RET0A:
    return 0;
RET0B:
    return 0;
RET1:
    return 1;
}

// ORIG T 0x0863977e _ZN33Timer_UpdateOnlinePreliminaryTeam15RegistNextTimerEi
bool Timer_UpdateOnlinePreliminaryTeam::RegistNextTimer(int team)
{
    // ORIG 0x0863977e：movl $0x258,-0xc(%ebp) 具名槽（帧 0x38），0x10(%esp) 二级装载。
    int nSec = 0x258;
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x8f, nSec, team, 0);
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
    // ORIG 0x086398a8：null 早退（jne 岛）+ `!check` 早退（xor$1;test;je，
    // 0x086398c9）；mgr 具名槽 -0xc（party=-0x10 后使用，0x086398d8 mov %eax,-0xc）。
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)0x2b, p2))
        return 0;
    CSpecialItemRoutingManager* mgr = G_CGameManager()->GetSpecialItemRoutingManager();
    mgr->ProcessTimeOut(party);
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
    // ORIG +0x8e..+0xa5: !getPcRoomAuth()（bool 声明 ⇒ xor $0x1,%eax 形态）
    if (!user->getPcRoomAuth())
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
        && min >= w.m_startMin && min < w.m_endMin;
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

    // ORIG：find/end 各自写完整表达式（两次 G_CDataManagerv 调用，不缓存）
    std::map<int, STDungeonInoutTime>::iterator it =
        G_CDataManager()->m_serverParameter.m_dungeonInoutc8.find((int)p3);
    if (it != G_CDataManager()->m_serverParameter.m_dungeonInoutc8.end())
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
            // isDungeonOpen(0x2aff)/setDungeonOpen(0x2aff,true)；send 用 p3
            if (!G_CDataManager()->m_serverParameter.isDungeonOpen(0x2aff))
            {
                G_CDataManager()->m_serverParameter.setDungeonOpen(0x2aff, true);
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

int Timer_FairPvPRematch::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // [b26N7] 尾部虚调用 callee 寄存器 ORIG vfn→%ecx/参数 eax,edx vs 4.4 恒
    // vfn→%eax（N7 探针实证），ndiff 12 封顶。
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
    // ORIG 0x0863cxxx：room 单槽 -0xc，matcher 三次从 +0x6e4 直读（view 链式，
    // 无具名槽）；虚表 +0x64 = IMatch::OnRematchTimeout(room)。
    // ORIG 0x0863a499：正向 `if (matcher != 0){call}`（je 863a4c0 跳 mov$1，
    // null 与 call 后都汇聚 return 1）。
    if (((TimerStartMatchingMatcherSlot*)room)->m_matcher != 0)
        ((TimerStartMatchingMatcherSlot*)room)->m_matcher->OnRematchTimeout(room);
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
        // ORIG 0x0863a631：party!=0 走 fall-through（send_to_party），==0 je 跳尾部 Send 岛。
        if (party != 0)
            party->send_to_party(packet);
        else
            user->Send(packet);
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
    // ORIG 0x0863ae10：getCurSec 结果驻 %eax（无 now 槽），call 后再装载 p3
    // `mov 0x10(%ebp),%edx; mov %edx,%ecx; sub %eax,%ecx; mov %ecx,%eax` 三步拷贝链，
    // delta 落具名槽
    // N96/P0：绑定形已废，两段式具名局部（P197③ 双槽形）。
    unsigned int cur;
    cur = GlobalData::s_systemTime_.getCurSec();
    int delta;
    delta = static_cast<int>(p3) - static_cast<int>(cur);
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, p1,
                                (TIMER_MESSAGE)0x95, delta,
                                p2, 0);
}

Timer_HadesHeartBeat::Timer_HadesHeartBeat() {}
Timer_HadesHeartBeat::~Timer_HadesHeartBeat() {}
int Timer_HadesHeartBeat::dispatch_sig(int p1, int p2, unsigned int p3)
{
    if (GlobalData::s_hades_proxy->GetSocketState() == 0) {
        // ORIG 0x0863a760：channelNo 直落 %ebx（push %ebx 帧）、udp_port 参数位
        // movzwl %ax 免槽（探针 x7 实证 register + 实参内联 cast 形态）。
        register int channelNo = G_CEnvironment()->get_channel_no();
        GlobalData::s_hades_proxy->SendHeartBeatPacket(
            (unsigned short)G_CEnvironment()->get_udp_port(), channelNo);
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
    G_CDataManager()->m_itemShop->updateOneADayItemList();   // m_itemShop @+0x8cbc
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
    // ORIG 0x0863ab92：mov 0x37c(%eax),%ebx（register 直落 %ebx）；
    // mng 无具名槽（链式）。
    register int dayValue = G_CEnvironment()->m_serverEnvir.m_fatigueTime;
    static_cast<EventClassify::CEventScriptMng*>(G_CDataManager()->get_event_script_mng())
        ->update_time_values(dayValue);
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

    // ORIG 0x0863a??（报告 Timer_AssaultGhostTimer/dispatch_sig.md）：
    // getCurCharacNo(unsigned) != p3 同型 ⇒ cmp 0x14(%ebp),%eax 直比。
    if (user->getCurCharacNo() != p3)
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
    // [b23S3C] check_timer_key 第二实参为 p2（非 p3，ORIG 0x0863b318）；P48 去缓存
    // get_user 每调用点重取×3（fresh .o 133/133 零差）。
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)0x2e, p2))  // ORIG 0x0863b318：用 p2（0x10(%ebp)），非 p3
        return 0;
    if (party->GetTournamentDungeonClearState() == 0x01)
    {
        PacketGuard packet;
        packet.clear();
        packet.put_header(1, 0x1ca);
        packet.put_byte(1);
        for (int i = 0; i <= 3; ++i)
        {
            if (party->get_user(i) == 0)
            {
                packet.put_byte(-1);
            }
            else
            {
                // ORIG 0x0863b40a：get_user ×3（每次调用点重取，P183 去缓存）；
                // 结果寄存器直比（sete）。
                if (party->get_user(i)->getCurCharacTotalFatigue() ==
                    party->get_user(i)->getCurCharacTotalMaxFatigue())
                    packet.put_byte(0);
                else
                    packet.put_byte(1);
            }
        }
        packet.finalize(true);
        party->send_to_party(packet);
        party->SetTournamentDungeonClearState(0x02);
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
        CTournamentDungeonReward& reward =
            *static_cast<CTournamentDungeonReward*>(   // ORIG 0x0863b71b：mov party; add $0x17bc,%eax 单级
                (void*)((unsigned long)party + 0x17bcUL));
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
    char local_43c[0x400];              // ORIG rep stos ecx=0x100（dword）→ 清 0x400 字节
    memset(local_43c, 0, sizeof(local_43c));
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
    ((CDungeonEntranceLog*)GetInstanceDungeonEntranceLog())->InsertDungeonEntrance_Hour();
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
    // ORIG 0x084293xx：无 skip 槽 —— `event != 0`（cmpl;je merge/xor）+ `!IsEventing`
    // （虚表 +0x34 调用后 xor$1;test;je 直落 guard 块）。
    if (event != 0 && !event->IsEventing(0))
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
    // ORIG：delay 物化到局部槽 -0xc(%ebp) 再两步入栈。
    int delay = 0x708;
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x52,
                                delay, 0, 0);
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
    // ORIG 0x0863a9b4：getCurSec 结果寄存器内 add 后单落槽 -0x28(%ebp)。
    time_t sec = (time_t)(GlobalData::s_systemTime_.getCurSec() + 0x15180);
    struct tm v;                                 // ORIG v@-0x24（比 sec 近端）
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
    // ORIG 0x086396db：movl $0x258,-0xc(%ebp) 具名槽（帧 0x38），非 const 防常量传播，
    // 0x10(%esp)=mov -0xc(%ebp),%edx 装载。
    int nSec = 0x258;
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x8e,
                                nSec, type, 0);
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
