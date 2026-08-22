#include "LogManager.h"
// ============================================================================
// df_game_r PvP_Room（G2-3 PvP 链，PvP 对战房间聚合根，池化 600）还原
// （2026-08-17）。
// 逐函数对照 docs/class_func_reports/PvP_Room.md（92 节）与 ORIG 反汇编实现；
// 布局依据 PvP_Room.h（0x6e8），子对象走 PvP_deps.h/PvP_deps.cpp。
// 目标：编译产物与 ORIG 逐操作数一致（AE 口径：分支/调用目标→<T>、
// 大地址→<A>、去 @plt、剥尾部 nop）。
// 跨类缺失方法（CUser/GameWorld/统计/脚本等）以 asm-label extern 发调用，
// 链接桩由主 agent / 后续批次提供（PvP_deps.cpp 已定义其中大部分）。
// ============================================================================

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#include "PvP_Room.h"
#include "CSystemTime.h"
#include "InterfacePacketBuf.h"
#include "TimerQueue.h"
#include "GameWorld.h"
#include "CDataManager.h"

// ============================================================================
// PacketGuard（ORIG 0xc 字节：InterfacePacketBuf + int + char；
//      构造/析构由 GameStubs 提供单例定义 _ZN11PacketGuardC1/D1Ev）
// ============================================================================
class PacketGuard : public InterfacePacketBuf
{
public:
    PacketGuard();
    ~PacketGuard();

private:
    int m_nField4;
    char m_cField8;
};

// ============================================================================
// CSwitchLog（PvP 日志；_ZN10CSwitchLogC1EPKciii / clEPKcz 由 GameStubs 提供）
// ============================================================================
class CSwitchLog
{
public:
    CSwitchLog(const char* name, int line, int a, int b);
    void operator()(const char* fmt, ...);

private:
    char m_pad[0x10];
};

// ============================================================================
// PvP 全局数据（ORIG 定义；从 GameStubs.cpp 迁移）：
//   PVP_NORMAL_BATTLE_TIME 0x93f4ee0 / PVP_DM_BATTLE_TIME 0x93f4ee8，各 [2] int
//   PVPMAP_DEATHMATCH_MODE 0x94f70e0 / PVPMAP_ONLY_NORMAL_EXPOSURE 0x94f7108
//   PVPMAP_ONLY_TOURNAMENT_RANDOM 0x94f711c，各 0x14 char（零初始化 BSS）
// ============================================================================
int PVP_NORMAL_BATTLE_TIME[2] = {0, 0};
int PVP_DM_BATTLE_TIME[2] = {0, 0};
char PVPMAP_DEATHMATCH_MODE[0x14] = {0};
char PVPMAP_ONLY_NORMAL_EXPOSURE[0x14] = {0};
char PVPMAP_ONLY_TOURNAMENT_RANDOM[0x14] = {0};

// ============================================================================
// 自由函数
// ============================================================================
// ORIG 0x85d4690：查 PvP map；onlyDeathMatch = info.m_playable；
// mapNo∈[0,0x13] 时返回 PVPMAP_DEATHMATCH_MODE[mapNo]
bool IsDeathMatchMap(int mapNo, bool& onlyDeathMatch)
{
    PvPMapInfo info;
    if (!CMapList::GetPvpMapInfo(mapNo, info))
        return false;
    onlyDeathMatch = info.m_playable != 0;
    if (mapNo > 0x13 || mapNo < 0)
        return false;
    return PVPMAP_DEATHMATCH_MODE[mapNo] != 0;
}
extern "C" int sub_GetMatchingType() asm("_Z15GetMatchingTypev");
// ORIG 0x855c8f4：按频道类型返回匹配类型（9→2, 10→3, 14→4, 15→5, 其余→0）
int GetMatchingType()
{
    int type = 0;
    switch (G_GameWorld()->GetChannelType())
    {
    case 9:  type = 2; break;
    case 10: type = 3; break;
    case 14: type = 4; break;
    case 15: type = 5; break;
    default: type = 0;
    }
    return type;
}
extern "C" TimerQueue* sub_G_TimerQueue() asm("_Z12G_TimerQueuev");
extern "C" void* sub_GetInstanceCommonStatisticsMgr() asm("_Z30GetInstanceCommonStatisticsMgrv");
extern "C" void* sub_G_CGameManager() asm("_Z14G_CGameManagerv");
extern "C" void sub_CGameManager_PutPvp(void* mgr, void* room) asm("_ZN12CGameManager6PutPvpEP8PvP_Room");
extern "C" void sub_CGameManager_CheckOutPvp(void* mgr, void* user, bool flag)
    asm("_ZN12CGameManager11CheckOutPvpEP5CUserb");
extern "C" void sub_GameWorld_out_from_pvp(void* world, void* user)
    asm("_ZN9GameWorld12out_from_pvpEP5CUser");
extern "C" int sub_GameWorld_IsFreePvPChannel(void* world) asm("_ZNK9GameWorld16IsFreePvPChannelEv");
extern "C" int sub_GameWorld_IsSchoolPvPChannel(void* world)
    asm("_ZNK9GameWorld18IsSchoolPvPChannelEv");
extern "C" int sub_GameWorld_IsWinPointPvPChannel(void* world)
    asm("_ZNK9GameWorld20IsWinPointPvPChannelEv");
extern "C" void sub_GameWorld_send_AllBasicInfo(void* world, void* user)
    asm("_ZN9GameWorld17send_AllBasicInfoEP5CUser");
extern "C" void sub_Statistics_PVPPlay2(int a, int b, int c) asm("_ZN10Statistics8PVPPlay2Eiii");
extern "C" void sub_CCommonStatisticsMgr_IncreaseQuantity(void* self, int container, int v)
    asm("_ZN20CCommonStatisticsMgr16IncreaseQuantityENS_24ENUM_STATISTIC_CONTAINEREi");
extern "C" void sub_CEventManager_GetRepeatEvent(void* self, int v)
    asm("_ZN13CEventManager14GetRepeatEventEi");
extern "C" void* sub_CEventManager_GetRepeatEvent_p(void* self, int v)
    asm("_ZN13CEventManager14GetRepeatEventEi");
extern "C" void sub_CFairPvPScore_GetRemainBonusCount(void* self)
    asm("_ZN8fair_pvp13CFairPvPScore19GetRemainBonusCountEv");
extern "C" void sub_CFairPvPScore_GetLastTrainingSealCount(void* self)
    asm("_ZN8fair_pvp13CFairPvPScore24GetLastTrainingSealCountEv");
extern "C" void sub_CFairPvPScore_GetLastTrainingSealBonusCount(void* self)
    asm("_ZN8fair_pvp13CFairPvPScore29GetLastTrainingSealBonusCountEv");
extern "C" int sub_CFairPvPScore_GetRemainBonusCount_i(void* self)
    asm("_ZN8fair_pvp13CFairPvPScore19GetRemainBonusCountEv");
extern "C" int sub_CFairPvPScore_GetLastTrainingSealCount_i(void* self)
    asm("_ZN8fair_pvp13CFairPvPScore24GetLastTrainingSealCountEv");
extern "C" int sub_CFairPvPScore_GetLastTrainingSealBonusCount_i(void* self)
    asm("_ZN8fair_pvp13CFairPvPScore29GetLastTrainingSealBonusCountEv");
extern "C" int sub_CFairPvPScore_GetGiveItem(void* self) asm("_ZNK8fair_pvp13CFairPvPScore11GetGiveItemEv");
extern "C" int sub_RefPvpGrade_GetPvpGrade(void* self, int v) asm("_ZNK11RefPvpGrade11GetPvpGradeEi");
extern "C" int sub_RefPvpGrade_GetPvpCurrRankPoint(void* self, int v)
    asm("_ZNK11RefPvpGrade19GetPvpCurrRankPointEi");
extern "C" int sub_RefPvpGrade_GetPvpNextRankPoint(void* self, int v)
    asm("_ZNK11RefPvpGrade19GetPvpNextRankPointEi");
extern "C" int sub_ServerParameterScript_get_pvp_item_give(void* self, int v)
    asm("_ZN21ServerParameterScript17get_pvp_item_giveEi");
extern "C" int sub_CMapList_GetPVPMapCount(void* self) asm("_ZNK8CMapList14GetPVPMapCountEv");
extern "C" void sub_CDataManager_GetMaxGradePvPChannel(void* self)
    asm("_ZN12CDataManager21GetMaxGradePvPChannelEv");
extern "C" void sub_CPvPLiveEventData_IncreasePlayCount(void* self, void* user)
    asm("_ZN17CPvPLiveEventData17IncreasePlayCountEP5CUser");
extern "C" void sub_CUserProc_ClearActionAndSendtoUser(void* user, int a, int b, int c)
    asm("_ZN8APSystem9CUserProc24ClearActionAndSendtoUserEP5CUserNS_17_ActionGroupIndexEi16ENUM_PACKETCLASS");
extern "C" void sub_CDisconnectDetecter_RegisterUser(void* self, void* user)
    asm("_ZN19disconnect_detecter19CDisconnectDetecter12RegisterUserEP5CUser");
extern "C" void sub_CDisconnectDetecter_UnregisterUser(void* self, void* user)
    asm("_ZN19disconnect_detecter19CDisconnectDetecter14UnregisterUserEP5CUser");
extern "C" void* sub_CDisconnectDetecter_GetInstance() asm("_ZN19disconnect_detecter29GetInstanceDisconnectDetecterEv");
extern "C" void sub_CHackAnalyzer_reportHackInfo(void* self) asm("_ZN8WongWork13CHackAnalyzer14reportHackInfoEv");
extern "C" void sub_CHackAnalyzer_beginCollectHackInfo(void* self, void* user)
    asm("_ZN8WongWork13CHackAnalyzer20beginCollectHackInfoEP5CUser");
extern "C" void sub_CHackAnalyzer_setLastMonsterDeadTime(void* self, long long v)
    asm("_ZN8WongWork13CHackAnalyzer22setLastMonsterDeadTimeEl");
extern "C" void sub_CStatisticServerProxy_SendPacket(void* self, char* buf, int len)
    asm("_ZN21CStatisticServerProxy10SendPacketEPci");
extern "C" void* sub_CServerProxyMgr_GetServerProxy(void* self, int group)
    asm("_ZN15CServerProxyMgrI21CStatisticServerProxyE14GetServerProxyE17ENUM_SERVER_GROUP");
extern "C" void sub_Packet_Server_Match_data_C1(void* self) asm("_ZN24Packet_Server_Match_dataC1Ev");
extern "C" void sub_MissionClearCondition_Parameter_C1(void* self, int mode)
    asm("_ZN31MissionClearCondition_ParameterC1E15PVP_BATTLE_MODE");
extern "C" void sub_CMissionList_Charac_Update_Win_event(void* self, void* user,
                                                         const void* param, bool& out)
    asm("_ZN19CMissionList_Charac16Update_Win_eventER5CUserRK31MissionClearCondition_ParameterRb");
extern "C" void sub_CMissionList_Charac_Update_Kill_event(void* self, void* user,
                                                          const void* param)
    asm("_ZN19CMissionList_Charac17Update_Kill_eventER5CUserRK31MissionClearCondition_Parameter");
extern "C" void sub_CMissionList_Charac_Update_Revenge_event(void* self, void* user,
                                                             int mode, bool a)
    asm("_ZN19CMissionList_Charac20Update_Revenge_eventER5CUser15PVP_BATTLE_MODEb");
extern "C" void sub_CMissionList_Charac_MakeMissionList_forOldUser(void* self, const void* user)
    asm("_ZN19CMissionList_Charac26MakeMissionList_forOldUserERK5CUser");
extern "C" void sub_CMissionList_Charac_setIssue(void* self, bool a, bool b)
    asm("_ZN19CMissionList_Charac8setIssueEbb");
extern "C" short sub_CMissionList_Charac_getIssue_mode(void* self, bool a)
    asm("_ZNK19CMissionList_Charac13getIssue_modeEb");
extern "C" int sub_PvpResultType_GetTotalScore(const void* self)
    asm("_ZNK13PvpResultType13GetTotalScoreEv");
extern "C" int sub_CNetwork_get_inner_ip(void* self) asm("_ZN8CNetworkILi4096ELi450000EE12get_inner_ipEv");
extern "C" int sub_CNetwork_get_ip(void* self) asm("_ZN8CNetworkILi4096ELi450000EE6get_ipEv");
extern "C" int sub_CNetwork_get_port(void* self) asm("_ZN8CNetworkILi4096ELi450000EE8get_portEv");
extern "C" int sub_CNetwork_get_mtu(void* self) asm("_ZN8CNetworkILi4096ELi450000EE7get_mtuEv");
extern "C" int sub_CNetwork_get_nat_type(void* self) asm("_ZN8CNetworkILi4096ELi450000EE12get_nat_typeEv");
extern "C" int sub_CDataManager_GetMaxGradePvPChannel_i(void* self)
    asm("_ZN12CDataManager21GetMaxGradePvPChannelEv");

// ---- GlobalData 静态成员（GlobalData.h 与 CUserCharacInfo.h 的 namespace
//      GlobalData 冲突，不能直接 include；以 asm-label extern 引用定义）----
extern "C" CSystemTime GlobalData_s_systemTime_ asm("_ZN10GlobalData13s_systemTime_E");
extern "C" void* GlobalData_s_event_manager asm("_ZN10GlobalData15s_event_managerE");
extern "C" void* GlobalData_s_statistic_proxy_mgr asm("_ZN10GlobalData21s_statistic_proxy_mgrE");
extern "C" void* GlobalData_s_ref_pvp_grade asm("_ZN10GlobalData15s_ref_pvp_gradeE");

// ---- CUser 缺失方法（CUser.h 已声明的走成员调用；其余 asm extern）----
extern "C" void sub_CUser_LogHistory(void* self, const char* fmt, ...)
    asm("_ZN5CUser10LogHistoryEPKcz");
extern "C" void sub_CUser_SetPvpIndex(void* self, short idx) asm("_ZN5CUser11SetPvpIndexEs");
extern "C" void sub_CUser_gain_exp_sp(void* self, int a, int& b, int& c,
                                      int reason, int d, bool e)
    asm("_ZN5CUser11gain_exp_spEiRiS0_13eExpAddReasonib");
extern "C" int sub_CUser_gain_exp_sp_i(void* self, int a, int& b, int& c,
                                       int reason, int d, bool e)
    asm("_ZN5CUser11gain_exp_spEiRiS0_13eExpAddReasonib");
extern "C" void sub_CUser_CheckFatigue(void* self) asm("_ZN5CUser12CheckFatigueEv");
extern "C" void sub_CUser_IsEquipAvatar(void* self) asm("_ZN5CUser13IsEquipAvatarEv");
extern "C" void sub_CUser_add_pvp_result(void* self, bool a, unsigned* b)
    asm("_ZN5CUser14add_pvp_resultEbPj");
extern "C" void sub_CUser_update_pvp_rank(void* self, const void* result)
    asm("_ZN5CUser15update_pvp_rankERK13PvpResultType");
extern "C" void sub_CUser_update_pvp_point(void* self, int point) asm("_ZN5CUser16update_pvp_pointEi");
extern "C" void sub_CUser_add_pvp_play_info(void* self, unsigned a, unsigned b)
    asm("_ZN5CUser17add_pvp_play_infoEjj");
extern "C" void sub_CUser_add_guild_point_item(void* self) asm("_ZN5CUser20add_guild_point_itemEv");
extern "C" void sub_CUser_add_guild_pvp_result(void* self, int v) asm("_ZN5CUser20add_guild_pvp_resultEi");
extern "C" void sub_CUser_update_old_pvp_point(void* self) asm("_ZN5CUser20update_old_pvp_pointEv");
extern "C" void sub_CUser_find_pvp_masterid_walkingout_me(void* self, unsigned v)
    asm("_ZN5CUser31find_pvp_masterid_walkingout_meEj");
extern "C" int sub_CUser_find_pvp_masterid_walkingout_me_i(void* self, unsigned v)
    asm("_ZN5CUser31find_pvp_masterid_walkingout_meEj");
extern "C" void sub_CUser_insert_pvp_masterid_walkingout_me(void* self, unsigned v)
    asm("_ZN5CUser33insert_pvp_masterid_walkingout_meEj");
extern "C" int sub_CUser_has_within_Mission(void* self) asm("_ZNK5CUser18has_within_MissionEv");
extern "C" void sub_CUser_acceptable_within_mission(void* self)
    asm("_ZNK5CUser25acceptable_within_missionEv");

// ---- 数组访问宏（ORIG 使用 (i+0xc)*4 等原始偏移形态；成员写法会生成
//      直接缩放偏移 mov 0xc(%eax,%edx,4)，与 ORIG 的 add+缩放 形态不同）----
#define P_SEATS(i)   (m_seats[i])
#define P_TEAMS(i)   (m_teams[i])
#define P_SEATSTATE(i) (m_seatState[i])
#define P_WIN(i)     (m_winCheck[i])
#define P_TEAMWIN(i) (m_teamWin[i])
#define P_USERS(i)   (m_users[i])
// ============================================================================
// PvP_Room 实现
// ============================================================================

// ----------------------------------------------------------------------------
// 构造 / 析构
// ----------------------------------------------------------------------------

PvP_Room::PvP_Room()
{
    int local_14 = 0;
    std::fill<int*, int>(m_field22c, m_field22c + 15, local_14);
    for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
    {
        P_SEATS(local_10) = 0;
    }
    m_pMatchingSystem = new CMatchingSystem;
    m_matching = (int)m_pMatchingSystem->GetMatchingSystem(ENUM_PVP_MATCHING_TYPE_0);
    m_field61c = 0;
}

PvP_Room::~PvP_Room()
{
    if (m_pMatchingSystem)
    {
        delete m_pMatchingSystem;
    }
}

// ----------------------------------------------------------------------------
// 基础访问器（报告缺失，按 ORIG 直接读取）
// ----------------------------------------------------------------------------

void PvP_Room::SetIDX(int idx)
{
    m_index = idx;
    reset();
}

int PvP_Room::get_index()
{
    return m_index;
}

int PvP_Room::get_state()
{
    return m_state;
}

void PvP_Room::set_state(int state)
{
    m_state = state;
}

int PvP_Room::get_pvp_battle_mode()
{
    return m_pvpBattleMode;
}

int PvP_Room::GetManager() const
{
    return (int)m_seats[7];
}

int PvP_Room::GetManagerSeat() const
{
    return m_managerSeat;
}

bool PvP_Room::isRematchable() const
{
    return (unsigned char)m_field61c != 0;
}

void PvP_Room::setReamtchable(bool flag)
{
    m_field61c = flag ? 1 : 0;
}

int PvP_Room::GetRequestedMapNo()
{
    return (int)m_field5c4;
}

void PvP_Room::SetRequestedMapNo(short mapNo)
{
    m_field5c4 = mapNo;
}

int PvP_Room::GetTeamCount(int team)
{
    int local_f8 = 0;
    for (int local_fc = 0; local_fc < 8; local_fc = local_fc + 1)
    {
        if (P_TEAMS(local_fc) == team)
        {
            local_f8 = local_f8 + 1;
        }
    }
    return local_f8;
}

void PvP_Room::lock()
{
}

void PvP_Room::unlock()
{
}

unsigned int PvP_Room::gen_timer_key(TIMER_MESSAGE msg)
{
    return ++*(int*)((char*)&m_election + 0x40 + (int)msg * 4);
}

unsigned int PvP_Room::get_timer_key(TIMER_MESSAGE msg)
{
    return *(int*)((char*)&m_election + 0x40 + (int)msg * 4);
}

bool PvP_Room::CheckTimerKey(TIMER_MESSAGE msg, int key)
{
    return *(int*)((char*)&m_election + 0x40 + (int)msg * 4) == key;
}
int PvP_Room::IsExistPassword()
{
    return (unsigned char)m_field604;
}

int PvP_Room::IsEndPvpBattle()
{
    return (unsigned char)m_field617;
}

int PvP_Room::IsInsertTimerRecvPvpRank()
{
    return (unsigned char)m_field616;
}

// ----------------------------------------------------------------------------
// 积分计算
// ----------------------------------------------------------------------------

int PvP_Room::CalculateVictoryPointOfSinglePlay(int rank, bool win, int score)
{
    int local_10;
    if (win)
    {
        local_10 = score + 1;
    }
    else if (rank == 1)
    {
        local_10 = (score + 1) / 2;
    }
    else if (rank == 2)
    {
        local_10 = (int)(((unsigned)(score + 1 >> 0x1f) >> 0x1e) + score + 1) >> 2;
    }
    else if (rank == 3)
    {
        local_10 = (int)(((unsigned)(score + 1 >> 0x1f) >> 0x1d) + score + 1) >> 3;
    }
    else if (rank == 4)
    {
        local_10 = (int)(((unsigned)(score + 1 >> 0x1f) >> 0x1c) + score + 1) >> 4;
    }
    else if (rank == 5)
    {
        local_10 = (int)(((unsigned)(score + 1 >> 0x1f) >> 0x1b) + score + 1) >> 5;
    }
    else if (rank == 6)
    {
        local_10 = (int)(((unsigned)(score + 1 >> 0x1f) >> 0x1a) + score + 1) >> 6;
    }
    else if (rank == 7)
    {
        local_10 = (int)(((unsigned)(score + 1 >> 0x1f) >> 0x19) + score + 1) >> 7;
    }
    else if (rank == 0)
    {
        local_10 = 1;
    }
    else
    {
        cMyTrace local_20("int PvP_Room::CalculateVictoryPointOfSinglePlay(int, bool, int)",
                          0xb50, 5);
        local_20("invalid rank %d", rank);
    }
    return local_10;
}

int PvP_Room::CalculateVictoryPointOfTeamPlay(int rank, bool win, int score)
{
    if ((win) && (rank == 0))
    {
        score = score + 2;
    }
    else if (!win)
    {
        score = 1;
    }
    return score;
}

int PvP_Room::CalcuateVictoryPoint(int* rank, int rankCount)
{
    if ((m_pvpBattleMode == 1) || (m_pvpBattleMode == 4))
    {
        for (int local_2c = 0; local_2c < 8; local_2c = local_2c + 1)
        {
            if (P_SEATS(local_2c) != 0)
            {
                bool bVar1;
                char cVar2;
                if (P_SEATS(local_2c) == 0)
                {
                    bVar1 = false;
                }
                else
                {
                    cVar2 = (char)IsPvpObserver(local_2c);
                    bVar1 = cVar2 != 0;
                }
                if (!bVar1)
                {
                    int local_28 = 0;
                    for (; local_28 < rankCount && rank[local_28] != local_2c;
                         local_28 = local_28 + 1)
                    {
                    }
                    if (local_28 == rankCount)
                    {
                        cMyTrace local_54("void PvP_Room::CalcuateVictoryPoint(int*, int)", 0xb7c, 5);
                        local_54("1 PvP_Room::CalcuateVictoryPoint Error, No Rank User Found i<%d> RankCount<%d>",
                                 local_2c, rankCount);
                        goto LAB_085da583;
                    }
                    int local_34 = local_28;
                    bool local_2d = m_userTable.GetWinTeam() == local_2c;
                    CUser* user = P_SEATS(local_2c);
                    *(int*)((char*)user + 0x796fc) =
                        CalculateVictoryPointOfSinglePlay(local_34, local_2d, m_fieldBc);
                }
            }
        }
    }
    else
    {
        for (int local_24 = 0; local_24 < 8; local_24 = local_24 + 1)
        {
            if (P_SEATS(local_24) != 0)
            {
                bool bVar1;
                char cVar2;
                if (P_SEATS(local_24) == 0)
                {
                    bVar1 = false;
                }
                else
                {
                    cVar2 = (char)IsPvpObserver(local_24);
                    bVar1 = cVar2 != 0;
                }
                if (!bVar1)
                {
                    int local_20 = GetTeamIndex(P_SEATS(local_24));
                    int local_1c = 0;
                    for (; local_1c < rankCount && rank[local_1c] != local_24;
                         local_1c = local_1c + 1)
                    {
                    }
                    if (local_1c == rankCount)
                    {
                        cMyTrace local_44("void PvP_Room::CalcuateVictoryPoint(int*, int)", 0xb97, 5);
                        local_44("1 PvP_Room::CalcuateVictoryPoint Error, No Rank User Found i<%d> RankCount<%d>",
                                 local_24, rankCount);
                        goto LAB_085da583;
                    }
                    int local_18 = local_1c;
                    int local_34 = local_1c;
                    int local_14 = local_1c;
                    while (local_14 = local_14 + -1, -1 < local_14)
                    {
                        int local_10 = rank[local_14];
                        bool bVar3;
                        if (P_SEATS(local_10) == 0)
                        {
                            bVar3 = false;
                        }
                        else
                        {
                            cVar2 = (char)IsPvpObserver(local_10);
                            bVar3 = cVar2 != 0;
                        }
                        if ((!bVar3) &&
                            (GetTeamIndex(P_SEATS(local_10)) != local_20))
                        {
                            local_34 = local_34 + -1;
                        }
                    }
                    bool local_2d = m_userTable.GetWinTeam() == local_20;
                    CUser* user = P_SEATS(local_24);
                    *(int*)((char*)user + 0x796fc) =
                        CalculateVictoryPointOfTeamPlay(local_34, local_2d, m_fieldBc / 2);
                }
            }
        }
    }
LAB_085da583:
    ;
}

// ----------------------------------------------------------------------------
// 房间流程
// ----------------------------------------------------------------------------

int PvP_Room::CheckPassword(const char* password, int len)
{
    if (m_passwordLen != len)
    {
        return 0;
    }
    return memcmp(m_password, password, m_passwordLen) == 0;
}

int PvP_Room::IsPvpObserver(int seat)
{
    if (seat <= 7)
    {
        if ((P_TEAMS(seat) == 3) || (P_TEAMS(seat) == 4))
        {
            return 1;
        }
        return 0;
    }
    return 0;
}

int PvP_Room::check_joinable(CUser* user)
{
    int local_10;
    if (m_state == 1)
    {
        local_10 = (*(int (**)(void*, void*, void*))(*(char**)m_matching + 0x70))(
            (void*)m_matching, this, user);
        if (local_10 != 0)
        {
            return local_10;
        }
        for (int local_f4 = 0; local_f4 < 8; local_f4 = local_f4 + 1)
        {
            if ((P_SEATS(local_f4) == 0) && (P_TEAMS(local_f4) == 0xff))
            {
                return 0;
            }
        }
        local_10 = 4;
    }
    else
    {
        local_10 = 4;
    }
    return local_10;
}

int PvP_Room::IsQuickJoinablePVPRoom(CUser* user)
{
    if (check_joinable(user) == 0)
    {
        unsigned characNo = m_seats[7]->getCurCharacNo();
        if (sub_CUser_find_pvp_masterid_walkingout_me_i(user, characNo) != 1)
        {
            return 1;
        }
    }
    return 0;
}

int PvP_Room::get_team(int seat)
{
    if ((seat < 0) || (7 < seat))
    {
        return 0xfe;
    }
    return P_TEAMS(seat);
}

int PvP_Room::get_manager_team()
{
    if ((m_managerSeat < 0) || (7 < m_managerSeat))
    {
        return 0xfe;
    }
    return P_TEAMS(m_managerSeat);
}

int PvP_Room::get_user_seat(CUser* user)
{
    int local_8 = 0;
    while (true)
    {
        if (7 < local_8)
        {
            return -1;
        }
        if (P_SEATS(local_8) == user)
        {
            break;
        }
        local_8 = local_8 + 1;
    }
    return local_8;
}

int PvP_Room::get_user_seat(int seat)
{
    int r;
    if ((seat >= 0) && (seat <= 7))
    {
        r = (int)P_SEATS(seat);
    }
    else
    {
        r = 0;
    }
    return r;
}

int PvP_Room::GetTeamIndex(CUser* user)
{
    unsigned int local_8 = 0;
    while ((int)local_8 < 8 &&
           (P_SEATS(local_8) == 0 || P_SEATS(local_8) != user))
    {
        local_8 = local_8 + 1;
    }
    if (local_8 == 8)
    {
        local_8 = 0xffffffff;
    }
    else if ((m_pvpBattleMode != 1) && (m_pvpBattleMode != 4))
    {
        local_8 = P_TEAMWIN(local_8);
    }
    return local_8;
}

int PvP_Room::get_player_count()
{
    int local_14 = 0;
    int local_10 = 0;
    do
    {
        if (7 < local_10)
        {
            return local_14;
        }
        bool bVar1;
        if (P_SEATS(local_10) == 0)
        {
            bVar1 = false;
        }
        else
        {
            char cVar2 = (char)IsPvpObserver(local_10);
            if (cVar2 == 1)
            {
                bVar1 = false;
            }
            else
            {
                bVar1 = true;
            }
        }
        if (bVar1)
        {
            local_14 = local_14 + 1;
        }
        local_10 = local_10 + 1;
    } while (true);
}

int PvP_Room::get_pvp_team_cnt()
{
    int local_14;
    if (((m_pvpBattleMode == 2) || (m_pvpBattleMode == 3)) || (m_pvpBattleMode == 5))
    {
        int local_18 = 0;
        local_14 = 0;
        for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
        {
            bool bVar1;
            char cVar2;
            if (P_SEATS(local_10) == 0)
            {
                bVar1 = false;
            }
            else
            {
                cVar2 = (char)IsPvpObserver(local_10);
                if (cVar2 == 1)
                {
                    bVar1 = false;
                }
                else
                {
                    bVar1 = true;
                }
            }
            if (bVar1)
            {
                if (P_TEAMS(local_10) == 1)
                {
                    local_18 = local_18 + 1;
                }
                else if (P_TEAMS(local_10) == 2)
                {
                    local_14 = local_14 + 1;
                }
            }
        }
        if (local_14 != local_18)
        {
            local_14 = -1;
        }
    }
    else
    {
        local_14 = -1;
    }
    return local_14;
}

int PvP_Room::check_team_pvp()
{
    if (((m_pvpBattleMode == 2) || (m_pvpBattleMode == 3)) || (m_pvpBattleMode == 5))
    {
        if (G_GameWorld()->GetChannelType() == 7)
        {
            return 1;
        }
        int local_18 = 0;
        int local_14 = 0;
        for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
        {
            bool bVar1;
            char cVar2;
            if (P_SEATS(local_10) == 0)
            {
                bVar1 = false;
            }
            else
            {
                cVar2 = (char)IsPvpObserver(local_10);
                if (cVar2 == 1)
                {
                    bVar1 = false;
                }
                else
                {
                    bVar1 = true;
                }
            }
            if (bVar1)
            {
                if (P_TEAMS(local_10) == 1)
                {
                    local_18 = local_18 + 1;
                }
                else if (P_TEAMS(local_10) == 2)
                {
                    local_14 = local_14 + 1;
                }
            }
        }
        if (local_14 == local_18)
        {
            return 1;
        }
        return 0;
    }
    return 1;
}

int PvP_Room::get_waiter_count() const
{
    int local_c = 0;
    for (int local_8 = 0; local_8 < 8; local_8 = local_8 + 1)
    {
        if (P_SEATS(local_8) != 0)
        {
            local_c = local_c + 1;
        }
    }
    return local_c;
}

int PvP_Room::get_remain_seat() const
{
    return 8 - get_waiter_count();
}

int PvP_Room::count_pvp_user()
{
    int local_c = 0;
    for (int local_8 = 0; local_8 < 8; local_8 = local_8 + 1)
    {
        if (P_SEATS(local_8) != 0)
        {
            local_c = local_c + 1;
        }
    }
    return local_c;
}

int PvP_Room::check_authority(CUser* user, int seat)
{
    if (m_seats[7] == user)
    {
        return 1;
    }
    if (P_SEATS(seat) == user)
    {
        return 1;
    }
    return 0;
}

int PvP_Room::check_start_state()
{
    lock();
    int local_10 = 0;
    while (true)
    {
        if (7 < local_10)
        {
            unlock();
            return 1;
        }
        bool bVar1;
        char cVar2;
        if (P_SEATS(local_10) == 0)
        {
            bVar1 = false;
        }
        else
        {
            cVar2 = (char)IsPvpObserver(local_10);
            if (cVar2 == 1)
            {
                bVar1 = false;
            }
            else
            {
                bVar1 = true;
            }
        }
        if ((bVar1) && (P_SEATSTATE(local_10) != 1))
        {
            break;
        }
        local_10 = local_10 + 1;
    }
    unlock();
    return 0;
}

int PvP_Room::is_closeable_seat_state()
{
    int local_18 = 0;
    int local_14 = 0;
    int local_10 = 0;
    do
    {
        if (7 < local_10)
        {
            if ((local_18 == 1) && (local_14 < 2))
            {
                return 0;
            }
            return 1;
        }
        bool bVar1;
        char cVar2;
        if (P_SEATS(local_10) == 0)
        {
            bVar1 = false;
        }
        else
        {
            cVar2 = (char)IsPvpObserver(local_10);
            if (cVar2 == 1)
            {
                bVar1 = false;
            }
            else
            {
                bVar1 = true;
            }
        }
        if (bVar1)
        {
            local_18 = local_18 + 1;
        }
        if (P_TEAMS(local_10) == 0xff)
        {
            local_14 = local_14 + 1;
        }
        local_10 = local_10 + 1;
    } while (true);
}

int PvP_Room::get_recv_pvp_rank_count()
{
    int local_c = 0;
    for (int local_8 = 0; local_8 < 8; local_8 = local_8 + 1)
    {
        if ((P_SEATS(local_8) != 0) &&
            (P_TEAMS(local_8) != 0xff) &&
            (m_recvFlagB4.IsRecv(local_8) != 0))
        {
            local_c = local_c + 1;
        }
    }
    return local_c;
}

int PvP_Room::get_recv_pvp_end_result_count() const
{
    int local_c = 0;
    for (int local_8 = 0; local_8 < 8; local_8 = local_8 + 1)
    {
        if ((P_SEATS(local_8) != 0) &&
            (P_TEAMS(local_8) != 0xff) &&
            (m_recvFlagBC.IsRecv(local_8) != 0))
        {
            local_c = local_c + 1;
        }
    }
    return local_c;
}

int PvP_Room::recv_pvp_rank_all() const
{
    int local_8 = 0;
    while (true)
    {
        if (7 < local_8)
        {
            return 1;
        }
        if ((P_SEATS(local_8) != 0) &&
            (P_TEAMS(local_8) != 0xff) &&
            (m_recvFlagB4.IsRecv(local_8) != 1))
        {
            break;
        }
        local_8 = local_8 + 1;
    }
    return 0;
}

int PvP_Room::recv_pvp_end_result_all() const
{
    int local_8 = 0;
    while (true)
    {
        if (7 < local_8)
        {
            return 1;
        }
        if ((P_SEATS(local_8) != 0) &&
            (P_TEAMS(local_8) != 0xff) &&
            (m_recvFlagBC.IsRecv(local_8) != 1))
        {
            break;
        }
        local_8 = local_8 + 1;
    }
    return 0;
}

int PvP_Room::recv_pvp_end_result_team(CUser* user) const
{
    int local_8 = 0xff;
    int local_c = 0;
    do
    {
        if (7 < local_c)
        {
        LAB_085dc14e:
            if (local_8 == 0xff)
            {
                return 0;
            }
            for (local_c = 0; local_c < 8; local_c = local_c + 1)
            {
                if ((P_SEATS(local_c) != 0) &&
                    (P_TEAMS(local_c) == local_8) &&
                    (m_recvFlagBC.IsRecv(local_c) != 1))
                {
                    return 0;
                }
            }
            return 1;
        }
        if ((P_SEATS(local_c) != 0) && (P_SEATS(local_c) == user))
        {
            local_8 = P_TEAMS(local_c);
            goto LAB_085dc14e;
        }
        local_c = local_c + 1;
    } while (true);
}

int PvP_Room::set_recv_pvp_rank_flag(CUser* user)
{
    int iVar2 = get_user_seat(user);
    if (iVar2 < 0)
    {
        return 0;
    }
    if (m_recvFlagB4.IsRecv(iVar2) == 0)
    {
        m_recvFlagB4.SetRecvFlag(iVar2);
        return 1;
    }
    return 0;
}

int PvP_Room::set_recv_pvp_end_result_flag(CUser* user)
{
    int iVar1 = get_user_seat(user);
    if (iVar1 >= 0)
    {
        m_recvFlagBC.SetRecvFlag(iVar1);
    }
    return iVar1 >= 0;
}

int PvP_Room::is_last_win(int seat)
{
    if (P_SEATS(seat) == 0)
    {
        return 0;
    }
    int iVar1 = GetTeamIndex(P_SEATS(seat));
    int iVar2 = m_userTable.GetWinTeam();
    return iVar2 == iVar1;
}

int PvP_Room::IsAllMoreThanGrade(int grade)
{
    if ((grade < 0x26) && (grade >= 0))
    {
        for (int local_14 = 0; local_14 < 8; local_14 = local_14 + 1)
        {
            if ((P_SEATS(local_14) != 0) &&
                (P_SEATS(local_14)->get_pvp_grade() < grade))
            {
                return 0;
            }
        }
        return 1;
    }
    return 0;
}

int PvP_Room::check_acquire_guild_point()
{
    if (G_GameWorld()->IsPVPChannel() == 0)
    {
        return 0;
    }
    if (sub_GameWorld_IsFreePvPChannel(G_GameWorld()) != 0)
    {
        return 0;
    }
    if (IsAllMoreThanGrade(10) != 1)
    {
        return 0;
    }
    if (IsThereSameGuildMember() != 0)
    {
        return 0;
    }
    return 1;
}

int PvP_Room::check_winner()
{
    if (m_fieldC0 != 0)
    {
        return 0;
    }
    if ((m_pvpBattleMode == 2) || (m_pvpBattleMode == 3))
    {
        lock();
        int local_6c[6];
        local_6c[3] = 0;
        local_6c[4] = 0;
        local_6c[5] = 0;
        local_6c[0] = 0;
        local_6c[1] = 0;
        local_6c[2] = 0;
        for (int local_2c = 0; local_2c < 8; local_2c = local_2c + 1)
        {
            if ((P_TEAMWIN(local_2c) != 0) && (P_TEAMWIN(local_2c) < 0xfe))
            {
                local_6c[P_TEAMWIN(local_2c) + 3] = local_6c[P_TEAMWIN(local_2c) + 3] + 1;
                if (P_WIN(local_2c) != 0)
                {
                    local_6c[P_TEAMWIN(local_2c)] = local_6c[P_TEAMWIN(local_2c)] + 1;
                }
            }
        }
        int local_34 = 0;
        unsigned int local_30 = 0;
        for (int local_28 = 1; (int)local_28 < 3; local_28 = local_28 + 1)
        {
            if (0 < local_6c[local_28])
            {
                local_34 = local_34 + 1;
                local_30 = local_28;
            }
        }
        if (local_34 == 1)
        {
            for (int local_24 = 0; local_24 < 8; local_24 = local_24 + 1)
            {
                if (P_TEAMWIN(local_24) == local_30)
                {
                    P_WIN(local_24) = 1;
                }
                else
                {
                    P_WIN(local_24) = 0;
                }
            }
        }
        unlock();
        if (local_34 == 1)
        {
            m_userTable.SetWinTeam(local_30);
            return 1;
        }
    }
    else if (m_pvpBattleMode == 4)
    {
        int local_20 = 0;
        for (int local_1c = 0; local_1c < 8; local_1c = local_1c + 1)
        {
            if ((P_SEATS(local_1c) != 0) && (P_TEAMS(local_1c) != 4))
            {
                local_20 = local_20 + 1;
            }
        }
        cMyTrace local_54("bool PvP_Room::check_winner()", 0xa3c, 0);
        local_54("PvP_Room::check_winner single : %d", local_20);
        if (m_deathMatch.checkGameOverForSinglePlay(local_20) != 0)
        {
            m_deathMatch.checkWinnerForSinglePlay(m_users, m_winCheck, m_userTable);
            return 1;
        }
    }
    else if (m_pvpBattleMode == 5)
    {
        int uVar2 = get_pvp_team_cnt();
        cMyTrace local_44("bool PvP_Room::check_winner()", 0xa46, 0);
        local_44("PvP_Room::check_winner team  : %d", uVar2);
        int iVar3 = get_pvp_team_cnt();
        if (m_deathMatch.checkGameOverForTeamPlay(m_users, iVar3, m_teamWin) != 0)
        {
            m_deathMatch.checkWinnerForTeamPlay(m_users, m_winCheck, m_teamWin, m_userTable);
            return 1;
        }
    }
    else
    {
        lock();
        int local_18 = 0;
        int local_14 = 0;
        for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
        {
            if ((P_WIN(local_10) != 0) && (P_TEAMS(local_10) != 4))
            {
                local_18 = local_18 + 1;
                local_14 = local_10;
            }
        }
        unlock();
        if (local_18 == 1)
        {
            m_userTable.SetWinTeam(local_14);
            return 1;
        }
    }
    return 0;
}

// ----------------------------------------------------------------------------
// 房间生命周期
// ----------------------------------------------------------------------------

int PvP_Room::create_room(CUser* user, char a, char* name, short b, bool c,
                          const char* password, int d, PVP_BATTLE_MODE mode)
{
    char local_40 = a;
    short local_44 = b;
    bool local_45 = c;
    int local_48 = (int)mode;
    int roomIdx = m_index;
    const char* characName = user->getCurCharacName();
    const char* accName = user->get_acc_name();
    {
        CSwitchLog local_30("int PvP_Room::create_room(CUser*, char, char*, short int, bool, const char*, int, PVP_BATTLE_MODE)",
                            0x43a, 0, 0);
        local_30("pvp@log %s,%s,room(%d),%d", accName, characName, roomIdx, local_48);
    }
    if (user->getCurCharacR() == 0)
    {
        return 0x13;
    }
    m_matching = (int)m_pMatchingSystem->GetMatchingSystem((ENUM_PVP_MATCHING_TYPE)sub_GetMatchingType());
    (*(int (**)(void*, short*, int*))(*(char**)m_matching + 0x18))(
        (void*)m_matching, &m_field5c4, &m_pvpBattleMode);
    if (m_state != 0)
    {
        return 0x13;
    }
    m_state = 1;
    m_seats[7] = user;
    m_managerSeat = 0;
    P_USERS(0) = user;
    P_TEAMS(0) = 1;
    m_relayMgr.OnChangeTeam(0, 1);
    m_mode8 = local_40;
    if (m_mode8 == 0)
    {
        size_t len = strlen(name);
        strncpy(m_roomName, name, len);
    }
    sub_CUser_SetPvpIndex(user, (short)m_index);
    m_joinCount = 1;
    m_field5c4 = local_44;
    m_field5c6 = m_field5c4;
    m_field604 = local_45 ? 1 : 0;
    if (m_field604 != 0)
    {
        m_passwordLen = d;
        memset(m_password, 0, 9);
        memcpy(m_password, password, m_passwordLen);
    }
    m_relayMgr.OnCreateRoom(m_managerSeat);
    m_deathMatch.OnCreateRoom(m_managerSeat);
    m_pvpBattleMode = local_48;
    int ret = (*(int (**)(void*, void*))(*(char**)m_matching + 0x20))(
        (void*)m_matching, user);
    if (ret != 0)
    {
        return ret;
    }
    return 0;
}

int PvP_Room::join_room(CUser* user, int& seat, bool flag)
{
    int local_48 = get_waiter_count();
    int local_7c[3];
    PacketGuard local_64;
    PacketGuard local_70;
    {
        CSwitchLog local_58("int PvP_Room::join_room(CUser*, int&, bool)", 0x4a8, 0, 0);
        local_58("pvp@log %s,%s,room(%d),%d,%d,%d", user->get_acc_name(),
                 user->getCurCharacName(), m_index, m_pvpBattleMode, flag ? 1 : 0, local_48);
    }
    bool bVar3;
    if (G_GameWorld()->GetChannelType() != 0xf)
    {
        if (G_GameWorld()->GetChannelType() != 0x10)
        {
            bVar3 = false;
            goto LAB_085d6dde;
        }
    }
    bVar3 = true;
LAB_085d6dde:
    if (bVar3)
    {
        int iVar9 = sub_CDataManager_GetMaxGradePvPChannel_i(G_CDataManager());
        int iVar10 = user->get_pvp_grade();
        if (iVar9 < iVar10)
        {
            return 0xd;
        }
        int iVar11 = sub_CFairPvPScore_GetGiveItem(user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)7));
        iVar10 = user->get_pvp_grade();
        iVar11 = sub_ServerParameterScript_get_pvp_item_give((char*)G_CDataManager() + 0x68, iVar10);
        if (iVar9 <= iVar11)
        {
            return 7;
        }
    }
    if (user->getCurCharacR() == 0)
    {
        return 0x13;
    }
    if (m_state == 2)
    {
        return 0x13;
    }
    local_48 = get_waiter_count();
    local_64.put_header(0, 2);
    local_70.put_header(0, 0xb);
    local_64.put_byte(1);
    local_64.put_short(local_48);
    local_70.put_byte(local_48);
    for (int local_3c = 0; local_3c < 8; local_3c = local_3c + 1)
    {
        if (P_SEATS(local_3c) != 0)
        {
            P_SEATS(local_3c)->make_basic_info((char*)&local_64, 1);
            local_70.put_short((int)P_SEATS(local_3c)->get_unique_id() & 0xffff);
            local_70.put_int(sub_CNetwork_get_inner_ip((char*)P_SEATS(local_3c) + 0xe0));
            local_70.put_int(sub_CNetwork_get_ip((char*)P_SEATS(local_3c) + 0xe0));
            local_70.put_short(sub_CNetwork_get_port((char*)P_SEATS(local_3c) + 0xe0) & 0xffff);
            local_70.put_int((int)P_SEATS(local_3c)->get_acc_id());
            local_70.put_byte(sub_CNetwork_get_nat_type((char*)P_SEATS(local_3c) + 0xe0));
            local_70.put_int(sub_CNetwork_get_mtu((char*)P_SEATS(local_3c) + 0xe0));
            const void* r = P_SEATS(local_3c)->getCurCharacR();
            if (r == 0)
            {
                local_70.put_byte(0);
            }
            else
            {
                local_70.put_byte(*(unsigned char*)((const char*)r + 0x75));
            }
        }
    }
    unlock();
    local_64.finalize(true);
    local_70.finalize(true);
    lock();
    int local_44 = (*(int (**)(void*, void*, void*))(*(char**)m_matching + 0x70))(
        (void*)m_matching, this, user);
    if (local_44 != 0)
    {
        unlock();
        return local_44;
    }
    int local_40 = -1;
    for (int local_38 = 0; local_38 < 8; local_38 = local_38 + 1)
    {
        if ((P_SEATS(local_38) == 0) && (P_TEAMS(local_38) == 0xff))
        {
            local_40 = local_38;
            P_SEATS(local_38) = user;
            sub_CUser_SetPvpIndex(user, (short)m_index);
            seat = local_38;
            break;
        }
    }
    if (local_40 == -1)
    {
        unlock();
        return 4;
    }
    user->Send(local_64);
    user->Send(local_70);
    if (m_pvpBattleMode == 1)
    {
        P_TEAMS(local_40) = 0;
        m_relayMgr.OnChangeTeam(local_40, 0);
    }
    else if (m_pvpBattleMode == 4)
    {
        P_TEAMS(local_40) = 0;
    }
    else if (flag)
    {
        int local_34 = 1;
        P_TEAMS(local_40) = 1;
        m_relayMgr.OnChangeTeam(local_40, 1);
    }
    else
    {
        local_7c[0] = 0;
        local_7c[1] = 0;
        local_7c[2] = 0;
        for (int local_28 = 0; local_28 < 8; local_28 = local_28 + 1)
        {
            if ((0 < P_TEAMS(local_28)) && (P_TEAMS(local_28) < 3))
            {
                local_7c[P_TEAMS(local_28)] = local_7c[P_TEAMS(local_28)] + 1;
            }
        }
        int local_30 = 8;
        int local_2c = 1;
        for (int local_24 = 1; local_24 < 3; local_24 = local_24 + 1)
        {
            if (local_7c[local_24] <= local_30)
            {
                local_2c = local_24;
                local_30 = local_7c[local_24];
            }
        }
        P_TEAMS(local_40) = local_2c;
        m_relayMgr.OnChangeTeam(local_40, local_2c);
    }
    unlock();
    local_64.clear();
    local_64.put_header(0, 2);
    local_64.put_byte(1);
    local_64.put_short(1);
    user->make_basic_info((char*)&local_64, 1);
    local_64.finalize(true);
    send_to_pvp(local_64);
    local_70.clear();
    local_70.put_header(0, 0xb);
    local_70.put_byte(1);
    local_70.put_short((int)user->get_unique_id() & 0xffff);
    local_70.put_int(sub_CNetwork_get_inner_ip((char*)user + 0xe0));
    local_70.put_int(sub_CNetwork_get_ip((char*)user + 0xe0));
    local_70.put_short(sub_CNetwork_get_port((char*)user + 0xe0) & 0xffff);
    local_70.put_int((int)user->get_acc_id());
    local_70.put_byte(sub_CNetwork_get_nat_type((char*)user + 0xe0));
    local_70.put_int(sub_CNetwork_get_mtu((char*)user + 0xe0));
    const void* r2 = user->getCurCharacR();
    if (r2 == 0)
    {
        local_70.put_byte(0);
    }
    else
    {
        local_70.put_byte(*(unsigned char*)((const char*)r2 + 0x75));
    }
    local_70.finalize(true);
    send_to_pvp(local_70);
    local_64.clear();
    local_64.put_header(1, 0x36);
    local_64.put_byte(1);
    lock();
    for (int local_20 = 0; local_20 < 8; local_20 = local_20 + 1)
    {
        local_64.put_byte(P_SEATSTATE(local_20));
    }
    unlock();
    local_64.finalize(true);
    user->Send(local_64);
    *(int*)((char*)user + 0x796fc) = 0;
    m_joinCount = m_joinCount + 1;
    m_relayMgr.OnJoinRoom(seat);
    (*(int (**)(void*, void*))(*(char**)m_matching + 0x28))((void*)m_matching, user);
    return 0;
}

int PvP_Room::leave_room(CUser* user, bool& flag)
{
    int local_24;
    {
        CSwitchLog local_34("int PvP_Room::leave_room(CUser*, bool&)", 0x6c2, 0, 0);
        local_34("pvp@log %s,%s,room(%d),%d,%d,%d", user->get_acc_name(),
                 user->getCurCharacName(), m_index, m_pvpBattleMode, flag ? 1 : 0,
                 get_waiter_count());
    }
    lock();
    if (m_seats[7] == user)
    {
        select_new_manager();
        flag = true;
    }
    local_24 = get_user_seat(user);
    if (local_24 < 0)
    {
        local_24 = -1;
    }
    else
    {
        if (get_state() == 2)
        {
            GiveOuterPenalty(user);
        }
        int local_20 = get_team(local_24);
        sub_CUser_SetPvpIndex(user, -1);
        P_SEATSTATE(local_24) = 0;
        P_TEAMS(local_24) = 0xff;
        P_SEATS(local_24) = 0;
        m_joinCount = m_joinCount - 1;
        P_WIN(local_24) = 0;
        P_TEAMWIN(local_24) = 0;
        unlock();
        sub_CHackAnalyzer_reportHackInfo(user->getHackAnalyzer());
        user->getHackAnalyzer()->resetHackInfo();
        if (m_state == 2)
        {
            if (check_winner() != 0)
            {
                pvp_request_pvp_rank();
            }
        }
        m_relayMgr.OnLeaveRoom(local_24);
        (*(int (**)(void*, void*, void*, int, int))(*(char**)m_matching + 0x2c))(
            (void*)m_matching, user, this, local_20, local_24);
    }
    return local_24;
}

int PvP_Room::destroy_room(int idx)
{
    int local_14;
    {
        CSwitchLog local_24("CUser* PvP_Room::destroy_room(int)", 0x721, 0, 0);
        local_24("pvp@log room(%d), %d", get_index(), idx);
    }
    local_14 = (int)P_SEATS(idx);
    if (local_14 == 0)
    {
        for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
        {
            if (P_SEATS(local_10) != 0)
            {
                local_14 = (int)P_SEATS(local_10);
                break;
            }
        }
    }
    reset();
    return local_14;
}

int PvP_Room::Disconnect(CUser* user)
{
    int local_10 = 0;
    while (true)
    {
        if (7 < local_10)
        {
            return 0;
        }
        if ((P_SEATS(local_10) != 0) && (P_SEATS(local_10) == user))
        {
            break;
        }
        local_10 = local_10 + 1;
    }
    (*(int (**)(void*, void*, int))(*(char**)m_matching + 0x44))(
        (void*)m_matching, this, local_10);
    int ret = WalkOut(P_SEATS(local_10), local_10, ENUM_SEAT_STATE_0xfe,
                      (ENUM_WALKOUT_TYPE)3);
    if (ret != 1)
    {
        ClearPvPRoom(user);
    }
    return 1;
}

int PvP_Room::SendWalkOut(int seat, ENUM_WALKOUT_TYPE walkoutType)
{
    PacketGuard local_18;
    local_18.put_header(0, 10);
    local_18.put_byte(seat);
    local_18.put_byte((int)walkoutType);
    local_18.finalize(true);
    send_to_pvp(local_18);
    return 1;
}

int PvP_Room::WalkOut(CUser* user, int seat, ENUM_SEAT_STATE state,
                      ENUM_WALKOUT_TYPE walkoutType)
{
    PacketGuard local_20;
    CUser* local_24 = 0;
    bool local_25 = false;
    bool local_26 = false;
    bool local_27 = false;
    int local_14 = get_team(seat);
    SendWalkOut(seat, walkoutType);
    int local_10 = set_seat_state(user, seat, state, &local_24, local_25, local_26, local_27);
    if ((get_waiter_count() == 0) && (local_27 != 1))
    {
        LogManager::logFormat(1, "pvp.cpp",
                              "bool PvP_Room::WalkOut(CUser*, int, ENUM_SEAT_STATE, ENUM_WALKOUT_TYPE)",
                              0x1210, "PvP Room(%d) is empty!!!", get_index());
    }
    if (local_10 == 0)
    {
        if (local_24 != 0)
        {
            (*(int (**)(void*, void*, void*, int, int))(*(char**)m_matching + 0x2c))(
                (void*)m_matching, local_24, this, local_14, seat);
            sub_GameWorld_out_from_pvp(G_GameWorld(), local_24);
        }
        if (local_25 != 0)
        {
            local_20.clear();
            make_seat_info((char*)&local_20, seat);
            G_GameWorld()->send_all(local_20);
        }
        if (local_26 != 0)
        {
            local_20.clear();
            make_state_info((char*)&local_20);
            G_GameWorld()->send_all(local_20);
        }
        if (local_27 != 0)
        {
            return 0;
        }
    }
    return 1;
}

int PvP_Room::OnSetPVPSeatState(CUser* user, int seat, ENUM_SEAT_STATE state,
                                ENUM_WALKOUT_TYPE walkoutType)
{
    CUser* local_24;
    bool local_25;
    bool local_26;
    bool local_27;
    int local_10;
    if (user == 0)
    {
        return 1;
    }
    PacketGuard local_20;
    int local_14 = 0;
    if (check_authority(user, seat) != 1)
    {
        local_14 = 8;
        goto LAB_085dd357;
    }
    if ((*(int (**)(void*, int))(*(char**)m_matching + 0x74))(
            (void*)m_matching, (int)state) != 1)
    {
        local_14 = 8;
        goto LAB_085dd357;
    }
    if (m_state == 2)
    {
        local_14 = 0x13;
        goto LAB_085dd357;
    }
    local_24 = 0;
    local_25 = false;
    local_26 = false;
    local_27 = false;
    local_10 = get_team(seat);
    if (state == ENUM_SEAT_STATE_0xfe)
    {
        if (walkoutType == (ENUM_WALKOUT_TYPE)3)
        {
            SendWalkOut(seat, walkoutType);
        }
        else if ((m_seats[7] == user) && (P_SEATS(seat) != 0) &&
                 (P_SEATS(seat) != user))
        {
            SendWalkOut(seat, walkoutType);
        }
    }
    local_14 = set_seat_state(user, seat, state, &local_24, local_25, local_26, local_27);
    if ((get_waiter_count() == 0) && (local_27 != 1))
    {
        LogManager::logFormat(1, "pvp.cpp",
                              "bool PvP_Room::OnSetPVPSeatState(CUser*, int, ENUM_SEAT_STATE, ENUM_WALKOUT_TYPE)",
                              0x11be, "PvP Room(%d) is empty!!!", get_index());
    }
    if (local_14 == 0)
    {
        if (local_24 != 0)
        {
            (*(int (**)(void*, void*, void*, int, int))(*(char**)m_matching + 0x2c))(
                (void*)m_matching, local_24, this, local_10, seat);
            sub_GameWorld_out_from_pvp(G_GameWorld(), local_24);
            sub_CUser_insert_pvp_masterid_walkingout_me(local_24, (unsigned)user->getCurCharacNo());
        }
        if (local_25 != 0)
        {
            local_20.clear();
            make_seat_info((char*)&local_20, seat);
            G_GameWorld()->send_all(local_20);
        }
        if (local_26 != 0)
        {
            local_20.clear();
            make_state_info((char*)&local_20);
            G_GameWorld()->send_all(local_20);
        }
        if (local_27 != 0)
        {
            return 0;
        }
    }
LAB_085dd357:
    if (local_14 > 0)
    {
        local_20.put_header(1, 0x37);
        local_20.put_byte(0);
        local_20.put_byte(local_14);
        local_20.finalize(true);
        user->Send(local_20);
    }
    return 1;
}

int PvP_Room::set_seat_state(CUser* user, int seat, ENUM_SEAT_STATE state,
                             CUser** out, bool& a, bool& b, bool& c)
{
    if (user == 0)
    {
        return 0;
    }
    int local_24;
    {
        CSwitchLog local_34("int PvP_Room::set_seat_state(CUser*, int, ENUM_SEAT_STATE, CUser**, bool&, bool&, bool&)",
                            0x594, 0, 0);
        local_34("pvp@log %s,%s,room(%d),%d,%d,%d,%d", user->get_acc_name(),
                 user->getCurCharacName(), m_index, m_pvpBattleMode, seat, (int)state,
                 get_waiter_count());
    }
    if (state == ENUM_SEAT_STATE_0xfe)
    {
        if (m_state == 2)
        {
            if (P_SEATS(seat) == 0)
            {
                return 4;
            }
            if (m_seats[7] == P_SEATS(seat))
            {
                select_new_manager();
                a = true;
                b = true;
            }
            *out = P_SEATS(seat);
            GiveOuterPenalty(*out);
            sub_CUser_SetPvpIndex(P_SEATS(seat), -1);
            local_24 = get_team(seat);
            lock();
            P_SEATSTATE(seat) = 0;
            P_TEAMS(seat) = 0xff;
            P_SEATS(seat) = 0;
            m_joinCount = m_joinCount - 1;
            P_WIN(seat) = 0;
            unlock();
            m_relayMgr.OnLeaveRoom(seat);
            (*(int (**)(void*, void*, void*, int, int))(*(char**)m_matching + 0x2c))(
                (void*)m_matching, *out, this, local_24, seat);
            if (get_waiter_count() == 0)
            {
                reset();
                a = true;
                b = true;
                c = true;
            }
            else
            {
                if (check_winner() != 0)
                {
                    b = true;
                    pvp_request_pvp_rank();
                }
                a = true;
            }
        }
        else if (m_seats[7] == user)
        {
            if (P_SEATS(seat) == 0)
            {
                if (is_closeable_seat_state() == 0)
                {
                    return 0x13;
                }
                change_team(seat, 0xfe);
                a = true;
            }
            else
            {
                sub_CUser_SetPvpIndex(P_SEATS(seat), -1);
                if (P_SEATS(seat) == m_seats[7])
                {
                    select_new_manager();
                    b = true;
                }
                *out = P_SEATS(seat);
                P_SEATSTATE(seat) = 0;
                P_TEAMS(seat) = 0xff;
                P_SEATS(seat) = 0;
                m_relayMgr.OnChangeTeam(seat, 0xff);
                if (get_waiter_count() == 0)
                {
                    reset();
                    a = true;
                    b = true;
                    c = true;
                }
                else
                {
                    a = true;
                    b = true;
                }
            }
        }
        else if (P_SEATS(seat) != 0)
        {
            sub_CUser_SetPvpIndex(P_SEATS(seat), -1);
            *out = P_SEATS(seat);
            int local_20 = get_team(seat);
            P_SEATSTATE(seat) = 0;
            P_TEAMS(seat) = 0xff;
            P_SEATS(seat) = 0;
            m_joinCount = m_joinCount - 1;
            m_relayMgr.OnLeaveRoom(seat);
            (*(int (**)(void*, void*, void*, int, int))(*(char**)m_matching + 0x2c))(
                (void*)m_matching, *out, this, local_20, seat);
            if (get_waiter_count() == 0)
            {
                reset();
                a = true;
                b = true;
                c = true;
            }
            else
            {
                a = true;
            }
        }
    }
    else if (state == (ENUM_SEAT_STATE)0xff)
    {
        if (m_seats[7] != user)
        {
            return 8;
        }
        change_team(seat, 0xff);
        a = true;
    }
    else
    {
        if (P_SEATS(seat) != user)
        {
            return 8;
        }
        change_team(seat, (int)state);
        a = true;
    }
    return 0;
}

int PvP_Room::set_ready_state(CUser* user, bool ready)
{
    if ((*(int (**)(void*, void*, bool))(*(char**)m_matching + 0x48))(
            (void*)m_matching, user, ready) != 1)
    {
        return 0;
    }
    PacketGuard local_1c;
    if (m_seats[7] == user)
    {
        if (ready)
        {
            lock();
            P_SEATSTATE(m_managerSeat) = 1;
            m_relayMgr.OnReady(m_managerSeat, true);
            unlock();
            if (check_start_state() == 0)
            {
                P_SEATSTATE(m_managerSeat) = 0;
                m_relayMgr.OnReady(m_managerSeat, false);
                return 0x16;
            }
            if (check_team_pvp() == 1)
            {
                make_ready_info((char*)&local_1c, m_managerSeat);
                send_to_pvp(local_1c);
                return 0xffffffff;
            }
            return 0xe;
        }
    }
    else
    {
        int local_10 = 0;
        lock();
        for (local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
        {
            if (P_SEATS(local_10) == user)
            {
                P_SEATSTATE(local_10) = ready ? 1 : 0;
                m_relayMgr.OnReady(local_10, ready);
                break;
            }
        }
        unlock();
        if (local_10 < 8)
        {
            make_ready_info((char*)&local_1c, local_10);
            send_to_pvp(local_1c);
        }
    }
    return 0;
}

int PvP_Room::make_ready_info(char* buf, int seat)
{
    InterfacePacketBuf* local_f4 = (InterfacePacketBuf*)buf;
    local_f4->put_header(0, 0x2c);
    local_f4->put_byte(seat);
    local_f4->put_byte(P_SEATSTATE(seat));
    local_f4->finalize(true);
}

int PvP_Room::make_room_info(char* buf)
{
    InterfacePacketBuf* local_ec = (InterfacePacketBuf*)buf;
    local_ec->put_short(m_index);
    local_ec->put_byte((int)m_mode8);
    if (m_mode8 == 0)
    {
        size_t len = strlen(m_roomName);
        local_ec->put_int((int)len);
        local_ec->put_str(m_roomName, (int)len);
    }
    local_ec->put_byte(m_state);
    local_ec->put_byte(m_managerSeat);
    local_ec->put_short((int)m_field5c4);
    local_ec->put_byte(m_pvpBattleMode);
    for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
    {
        local_ec->put_byte(P_TEAMS(local_10));
        if (P_SEATS(local_10) == 0)
        {
            local_ec->put_short(0xffff);
            local_ec->put_byte(0);
        }
        else
        {
            local_ec->put_short((int)P_SEATS(local_10)->get_unique_id() & 0xffff);
            local_ec->put_byte((int)P_SEATS(local_10)->isAllowLocationGeoState());
        }
    }
    local_ec->put_byte(m_field604);
    local_ec->put_int((*(int (**)(void*))(*(char**)m_matching + 8))((void*)m_matching));
}

int PvP_Room::make_seat_info(char* buf, int seat)
{
    InterfacePacketBuf* local_18 = (InterfacePacketBuf*)buf;
    local_18->put_header(0, 0x2b);
    local_18->put_short(m_index);
    local_18->put_byte(m_pvpBattleMode);
    if (seat == 0xffff)
    {
        int local_14 = get_waiter_count();
        local_18->put_byte(local_14);
        if (local_14 == 0)
        {
            LogManager::logFormat(1, "pvp.cpp", "void PvP_Room::make_seat_info(char*, int)",
                                  0xe66, "Send Member 0 PvpRoom %d State(%d)", m_index, m_state);
        }
        lock();
        for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
        {
            if (P_SEATS(local_10) != 0)
            {
                local_18->put_byte(local_10);
                local_18->put_byte(P_TEAMS(local_10));
                local_18->put_short((int)P_SEATS(local_10)->get_unique_id() & 0xffff);
                local_18->put_byte((int)P_SEATS(local_10)->isAllowLocationGeoState());
                cMyTrace local_28("void PvP_Room::make_seat_info(char*, int)", 0xe74, 0);
                local_28("RELAY_BATTLE SeatInfo player(%d), team(%d), uid(%d)\n",
                         local_10, P_TEAMS(local_10),
                         (int)P_SEATS(local_10)->get_unique_id() & 0xffff);
            }
        }
        unlock();
    }
    else
    {
        local_18->put_byte(1);
        lock();
        local_18->put_byte(seat);
        local_18->put_byte(P_TEAMS(seat));
        if (P_SEATS(seat) == 0)
        {
            local_18->put_short(0xffff);
            local_18->put_byte(0);
        }
        else
        {
            local_18->put_short((int)P_SEATS(seat)->get_unique_id() & 0xffff);
            local_18->put_byte((int)P_SEATS(seat)->isAllowLocationGeoState());
        }
        unlock();
    }
    local_18->finalize(true);
}

int PvP_Room::make_state_info(char* buf)
{
    InterfacePacketBuf* local_f4 = (InterfacePacketBuf*)buf;
    local_f4->put_header(0, 0x2a);
    local_f4->put_short(m_index);
    local_f4->put_byte(m_state);
    local_f4->put_byte(m_managerSeat);
    local_f4->put_short((int)m_field5c4);
    local_f4->put_byte(m_pvpBattleMode);
    local_f4->put_int((*(int (**)(void*))(*(char**)m_matching + 8))((void*)m_matching));
    local_f4->finalize(true);
}

void PvP_Room::get_room_info(PVP_ROOM_INFO* out)
{
    out->title = m_roomName;
    out->byte4 = m_mode8;
    out->short6 = m_field5c4;
    out->byte8 = m_field604;
    out->passwd = m_password;
}

void PvP_Room::get_user_list(void* out)
{
    (*(void (**)(void*, void*))(*(char**)m_matching + 0x14))((void*)m_matching, out);
}

void PvP_Room::send_to_pvp(PacketGuard& guard)
{
    for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
    {
        if (P_SEATS(local_10) != 0)
        {
            P_SEATS(local_10)->Send(guard);
        }
    }
}

void PvP_Room::send_to_pvp_team(PacketGuard& guard, CUser* user)
{
    if (m_pvpBattleMode > 1)
    {
        int local_18 = 0;
        for (int local_14 = 0; local_14 < 8; local_14 = local_14 + 1)
        {
            bool bVar1;
            char cVar2;
            if (P_SEATS(local_14) == 0)
            {
                bVar1 = false;
            }
            else
            {
                cVar2 = (char)IsPvpObserver(local_14);
                if (cVar2 == 1)
                {
                    bVar1 = false;
                }
                else
                {
                    bVar1 = true;
                }
            }
            if ((bVar1) && (P_SEATS(local_14) == user))
            {
                local_18 = P_TEAMS(local_14);
                break;
            }
        }
        if (local_18 != 0)
        {
            for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
            {
                bool bVar1;
                char cVar2;
                if (P_SEATS(local_10) == 0)
                {
                    bVar1 = false;
                }
                else
                {
                    cVar2 = (char)IsPvpObserver(local_10);
                    if (cVar2 == 1)
                    {
                        bVar1 = false;
                    }
                    else
                    {
                        bVar1 = true;
                    }
                }
                if ((bVar1) && (P_TEAMS(local_10) == local_18))
                {
                    P_SEATS(local_10)->Send(guard);
                }
            }
        }
    }
}

int PvP_Room::ClearPvPRoom(CUser* user)
{
    destroy_room(0);
    sub_CGameManager_PutPvp(sub_G_CGameManager(), this);
    PacketGuard local_18;
    local_18.clear();
    make_state_info((char*)&local_18);
    G_GameWorld()->send_all(local_18);
}

int PvP_Room::change_team(int seat, int team)
{
    lock();
    P_TEAMS(seat) = team;
    m_relayMgr.OnChangeTeam(seat, team);
    unlock();
}

int PvP_Room::select_new_manager()
{
    int local_8 = 0;
    while (true)
    {
        if (7 < local_8)
        {
            return 0;
        }
        if ((P_SEATS(local_8) != 0) && (P_SEATS(local_8) != m_seats[7]))
        {
            break;
        }
        local_8 = local_8 + 1;
    }
    m_seats[7] = P_SEATS(local_8);
    m_managerSeat = local_8;
    P_SEATSTATE(local_8) = 0;
    return 1;
}

int PvP_Room::change_manager_to_winner()
{
    if (m_seats[7] == 0)
    {
        return 0;
    }
    if (G_GameWorld()->GetChannelType() == 0xf)
    {
        return 1;
    }
    bool bVar1;
    if (G_GameWorld()->GetChannelType() != 7)
    {
        if (sub_GameWorld_IsSchoolPvPChannel(G_GameWorld()) == 0)
        {
            bVar1 = false;
            goto LAB_085d7e5a;
        }
    }
    bVar1 = true;
LAB_085d7e5a:
    if (bVar1)
    {
        return 1;
    }
    if (is_last_win(m_managerSeat) != 0)
    {
        return 1;
    }
    for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
    {
        if ((P_SEATS(local_10) != 0) && (is_last_win(local_10) == 1))
        {
            m_seats[7] = P_SEATS(local_10);
            m_managerSeat = local_10;
            return 1;
        }
    }
    return 0;
}

int PvP_Room::GiveOuterPenalty(CUser* user)
{
    PvpResultType local_b4;
    local_b4.Clear();
    sub_CUser_update_pvp_rank(user, &local_b4);
    int local_10 = GetTeamIndex(user);
    if (m_userTable.IsValidTeam(local_10) != 1)
    {
        cMyTrace local_20("void PvP_Room::GiveOuterPenalty(CUser*)", 0x10f8, 5);
        local_20("Invalid Team(%d)", local_10);
        goto LAB_085dcd66;
    }
    if (G_GameWorld()->IsIntegratedPvPBaseChannel() != 0)
    {
        void* ev = sub_CEventManager_GetRepeatEvent_p(GlobalData_s_event_manager, 0x38);
        if ((*(int (**)(void*, int))(*(char**)ev + 0x34))(ev, 0) != 0)
        {
            if ((int)(long)user->GetServerGroup() != 0)
            {
                struct Packet_Server_Match_data
                {
                    char m_pad[0x28];
                } local_2d;
                sub_Packet_Server_Match_data_C1(&local_2d);
                *((char*)&local_2d + 0xa) = (char)(long)user->GetServerGroup();
                *((char*)&local_2d + 0xc) = 1;
                void* proxy = sub_CServerProxyMgr_GetServerProxy(
                    GlobalData_s_statistic_proxy_mgr, (int)(long)user->GetServerGroup());
                sub_CStatisticServerProxy_SendPacket(proxy, (char*)&local_2d, 0xd);
            }
        }
    }
    if (G_GameWorld()->IsPVPChannel() != 0)
    {
        if ((G_GameWorld()->GetChannelType() == 4) ||
            (G_GameWorld()->GetChannelType() == 8))
        {
            sub_CUser_update_pvp_point(user, m_userTable.GetLeavePenalty(local_10));
        }
        else if (G_GameWorld()->GetChannelType() == 6)
        {
            sub_CUser_add_guild_pvp_result(user, -1);
            goto LAB_085dcd66;
        }
    }
    if (sub_GameWorld_IsWinPointPvPChannel(G_GameWorld()) != 0)
    {
        sub_CUser_add_pvp_result(user, false, 0);
    }
    (*(int (**)(void*, void*))(*(char**)m_matching + 0x60))((void*)m_matching, user);
LAB_085dcd66:
    ;
}

int PvP_Room::VoteLackUser(CUser* user, int vote)
{
    if ((vote < 0) || (7 < vote))
    {
        LogManager::logFormat(1, "pvp.cpp", "bool PvP_Room::VoteLackUser(CUser*, int)",
                              0x116d, "VoteSlotNo(%d) < 0 || MAX_PVP_PLAYER < VoteSlotNo", vote);
        return 0;
    }
    if ((P_SEATS(vote) == user) || (P_SEATS(vote) == 0))
    {
        return 0;
    }
    int iVar2 = get_user_seat(user);
    if ((iVar2 < 0) || (8 < iVar2))
    {
        LogManager::logFormat(1, "pvp.cpp", "bool PvP_Room::VoteLackUser(CUser*, int)",
                              0x117b, "UserSlotNo(%d) < 0 || MAX_PVP_PLAYER < UserSlotNo", iVar2);
        return 0;
    }
    m_election.Vote(iVar2, vote);
    unsigned int key = gen_timer_key((TIMER_MESSAGE)0x31);
    TimerQueue* tq = sub_G_TimerQueue();
    tq->InsertTimerInMilisecond((TimerEntry::OBJ_TYPE)1, m_index,
                                (TIMER_MESSAGE)0x31, 0x9c4LL, (int)key, 0);
    return 1;
}

int PvP_Room::OnConnectP2PPvPTimeout()
{
    if (check_start_state() != 1)
    {
        (*(int (**)(void*, void*))(*(char**)m_matching + 0x58))((void*)m_matching, this);
    }
}

int PvP_Room::OnTimerWalkOutLackUser()
{
    int local_10 = m_election.GetElected();
    while (local_10 >= 0)
    {
        m_election.CancelCandidate(local_10);
        m_election.CancelVoter(local_10);
        (*(int (**)(void*, void*, int))(*(char**)m_matching + 0x44))(
            (void*)m_matching, this, local_10);
        if (OnSetPVPSeatState(P_SEATS(local_10), local_10, ENUM_SEAT_STATE_0xfe,
                              (ENUM_WALKOUT_TYPE)3) != 1)
        {
            return 0;
        }
        local_10 = m_election.GetElected();
    }
    return 1;
}

int PvP_Room::HandleTimerUserRevive(int seat)
{
    if (((seat >= 0) && (seat < 8)) && (P_SEATS(seat) != 0) &&
        (P_WIN(seat) == 0))
    {
        P_WIN(seat) = 1;
        PacketGuard local_18;
        local_18.put_header(0, 0x78);
        local_18.put_byte(seat);
        local_18.put_byte(rand() % 0x100);
        local_18.finalize(true);
        send_to_pvp(local_18);
    }
}

int PvP_Room::SkipLastRankResultTimer()
{
    gen_timer_key((TIMER_MESSAGE)0x32);
}

int PvP_Room::CancelAllTimer()
{
    gen_timer_key((TIMER_MESSAGE)0x32);
}

int PvP_Room::end_pvp()
{
    (*(int (**)(void*, void*))(*(char**)m_matching + 0x40))((void*)m_matching, this);
}

int PvP_Room::end_pvp_result()
{
    change_manager_to_winner();
    m_state = 1;
    for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
    {
        if (P_SEATS(local_10) != 0)
        {
            P_SEATS(local_10)->set_state((ch_state)6);
        }
    }
    SkipLastRankResultTimer();
}

int PvP_Room::send_pvp_end()
{
    m_field617 = 1;
    if (G_GameWorld()->GetChannelType() != 6)
    {
        update_pvp_point();
    }
    pvp_reward();
    return 1;
}

int PvP_Room::pvp_request_pvp_rank()
{
    if (m_field615 == 0)
    {
        m_field615 = 1;
        PacketGuard local_18;
        local_18.put_header(0, 0x31);
        local_18.finalize(true);
        send_to_pvp(local_18);
        m_fieldC0 = 1;
    }
}

int PvP_Room::request_fight(CUser* user)
{
    if (m_field614 == 0)
    {
        int iVar2 = get_user_seat(user);
        if (iVar2 < 0)
        {
            return 0;
        }
        m_relayMgr.OnRequestFight(iVar2);
        return 1;
    }
    return 1;
}

int PvP_Room::revengeMission(bool flag)
{
    for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
    {
        if (P_SEATS(local_10) != 0)
        {
            void* mission = P_SEATS(local_10)->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
            sub_CMissionList_Charac_Update_Revenge_event(mission, P_SEATS(local_10),
                                                         m_pvpBattleMode, flag);
        }
    }
}

int PvP_Room::ChangePvPMode(int mapNo)
{
    bool local_d;
    local_d = false;
    char cVar1 = (char)IsDeathMatchMap(mapNo, local_d);
    if (cVar1 == 0)
    {
        if (m_pvpBattleMode == 4)
        {
            m_pvpBattleMode = 1;
        }
        else if (m_pvpBattleMode == 5)
        {
            m_pvpBattleMode = 2;
        }
    }
    else if ((m_pvpBattleMode != 4) && (m_pvpBattleMode != 5))
    {
        if (m_pvpBattleMode == 1)
        {
            m_pvpBattleMode = 4;
        }
        else if (m_pvpBattleMode == 2)
        {
            m_pvpBattleMode = 5;
        }
    }
}

int PvP_Room::CheckPvPMapMode(CUser* user, int mapNo, bool& flag)
{
    if (m_pvpBattleMode == 6)
    {
        return 1;
    }
    char cVar1 = (char)IsDeathMatchMap(mapNo, flag);
    if ((cVar1 != 0) && (flag == false) && (m_pvpBattleMode == 4))
    {
        return 1;
    }
    if ((cVar1 != 0) && (m_pvpBattleMode == 3))
    {
        m_pvpBattleMode = 4;
    }
    if ((cVar1 != 0) && (m_pvpBattleMode == 1))
    {
        set_pvp_mode(user, 4);
    }
    if ((cVar1 != 0) && (m_pvpBattleMode == 2))
    {
        m_pvpBattleMode = 5;
    }
    if ((cVar1 != 1) && (m_pvpBattleMode == 4))
    {
        set_pvp_mode(user, 1);
    }
    if ((cVar1 != 1) && (m_pvpBattleMode == 5))
    {
        m_pvpBattleMode = 2;
    }
    return 1;
}

int PvP_Room::set_pvp_mode(CUser* user, int mode)
{
    if (m_state != 1)
    {
        return 0x13;
    }
    if (m_seats[7] != user)
    {
        return 8;
    }
    lock();
    int iVar1 = m_pvpBattleMode;
    m_pvpBattleMode = mode;
    if (mode == 1)
    {
        for (int local_30 = 0; local_30 < 8; local_30 = local_30 + 1)
        {
            if (P_SEATS(local_30) != 0)
            {
                P_TEAMS(local_30) = 0;
                m_relayMgr.OnChangeTeam(local_30, 0);
            }
        }
    }
    else if (mode == 3)
    {
        if (iVar1 == 2)
        {
            for (int local_2c = 0; local_2c < 8; local_2c = local_2c + 1)
            {
                if (P_SEATS(local_2c) != 0)
                {
                    m_relayMgr.OnChangeTeam(local_2c, P_TEAMS(local_2c));
                }
            }
        }
        else if (iVar1 == 1)
        {
            int local_28 = 1;
            for (int local_24 = 0; local_24 < 8; local_24 = local_24 + 1)
            {
                if (P_SEATS(local_24) != 0)
                {
                    P_TEAMS(local_24) = local_28;
                    m_relayMgr.OnChangeTeam(local_24, P_TEAMS(local_24));
                    if (local_28 == 1)
                    {
                        local_28 = 2;
                    }
                    else
                    {
                        local_28 = 1;
                    }
                }
            }
        }
    }
    else if (mode == 4)
    {
        for (int local_20 = 0; local_20 < 8; local_20 = local_20 + 1)
        {
            if (P_SEATS(local_20) != 0)
            {
                P_TEAMS(local_20) = 0;
            }
        }
    }
    else if (mode == 5)
    {
        int local_1c = 1;
        for (int local_18 = 0; local_18 < 8; local_18 = local_18 + 1)
        {
            if (P_SEATS(local_18) != 0)
            {
                P_TEAMS(local_18) = local_1c;
                m_relayMgr.OnChangeTeam(local_18, local_1c);
                if (local_1c == 1)
                {
                    local_1c = 2;
                }
                else
                {
                    local_1c = 1;
                }
            }
        }
    }
    else if (iVar1 != 3)
    {
        int local_14 = 1;
        for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
        {
            if (P_SEATS(local_10) != 0)
            {
                P_TEAMS(local_10) = local_14;
                m_relayMgr.OnChangeTeam(local_10, P_TEAMS(local_10));
                if (local_14 == 1)
                {
                    local_14 = 2;
                }
                else
                {
                    local_14 = 1;
                }
            }
        }
    }
    unlock();
    return 0;
}

int PvP_Room::GetCandidateMapIndexList(std::vector<int>& out)
{
    bool local_d;
    local_d = false;
    int local_14 = 1;
    while (true)
    {
        int iVar3 = sub_CMapList_GetPVPMapCount((char*)G_CDataManager() + 4);
        if (!((local_14 < iVar3 + 1) && (local_14 < 0x14)))
        {
            break;
        }
        char cVar2 = (char)IsDeathMatchMap(local_14, local_d);
        if ((cVar2 == 0) ||
            ((m_pvpBattleMode != 3) &&
             (((m_pvpBattleMode != 1 && m_pvpBattleMode != 4) ||
               (local_d == true)))))
        {
            if (G_GameWorld()->GetChannelType() == 7)
            {
                if (PVPMAP_ONLY_TOURNAMENT_RANDOM[local_14] != 0)
                {
                    out.push_back(local_14);
                }
            }
            else if (PVPMAP_ONLY_NORMAL_EXPOSURE[local_14] != 0)
            {
                out.push_back(local_14);
            }
        }
        local_14 = local_14 + 1;
    }
}

int PvP_Room::insert_timer_recv_pvp_rank()
{
    if (m_field616 != 0)
    {
        return 1;
    }
    unsigned int key = gen_timer_key((TIMER_MESSAGE)0x3c);
    TimerQueue* tq = sub_G_TimerQueue();
    tq->InsertTimerInMilisecond((TimerEntry::OBJ_TYPE)1, m_index,
                                (TIMER_MESSAGE)0x3c, 10000LL, (int)key, 0);
    m_field616 = 1;
    return 1;
}

int PvP_Room::insert_timer_recv_pvp_end_result()
{
    unsigned int key = gen_timer_key((TIMER_MESSAGE)0x3d);
    int ms = (*(int (**)(void*))(*(char**)m_matching + 0x7c))((void*)m_matching);
    TimerQueue* tq = sub_G_TimerQueue();
    tq->InsertTimerInMilisecond((TimerEntry::OBJ_TYPE)1, m_index,
                                (TIMER_MESSAGE)0x3d, (long long)ms, (int)key, 0);
    return 1;
}

int PvP_Room::check_recv_pvp_rank()
{
    char local_11 = 0;
    for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
    {
        if ((P_SEATS(local_10) != 0) && (P_TEAMS(local_10) != 0xff) &&
            (m_recvFlagB4.IsRecv(local_10) != 1))
        {
            m_fieldC0 = 0;
            sub_CGameManager_CheckOutPvp(sub_G_CGameManager(), P_SEATS(local_10), true);
            local_11 = 1;
        }
    }
    return local_11;
}

int PvP_Room::IsThereSameGuildMember()
{
    if ((m_pvpBattleMode == 2) || (m_pvpBattleMode == 3))
    {
        int local_54[8];
        local_54[4] = 0;
        local_54[5] = 0;
        local_54[6] = 0;
        local_54[7] = 0;
        local_54[0] = 0;
        local_54[1] = 0;
        local_54[2] = 0;
        local_54[3] = 0;
        int local_2c = 0;
        int local_28 = 0;
        for (int local_24 = 0; local_24 < 8; local_24 = local_24 + 1)
        {
            if (P_TEAMS(local_24) == 1)
            {
                CUser* local_34 = P_SEATS(local_24);
                if (local_34 != 0)
                {
                    local_54[local_2c + 4] = local_34->get_charac_guildkey();
                    local_2c = local_2c + 1;
                }
            }
            else if (P_TEAMS(local_24) == 2)
            {
                CUser* local_34 = P_SEATS(local_24);
                if (local_34 != 0)
                {
                    local_54[local_28] = local_34->get_charac_guildkey();
                    local_28 = local_28 + 1;
                }
            }
        }
        if ((local_2c == 0) || (local_28 == 0))
        {
            return 0;
        }
        for (int local_20 = 0; local_20 < local_2c; local_20 = local_20 + 1)
        {
            for (int local_1c = 0; local_1c < local_28; local_1c = local_1c + 1)
            {
                if (local_54[local_20 + 4] == local_54[local_1c])
                {
                    return 1;
                }
            }
        }
    }
    else
    {
        int local_18 = 0;
        int local_14;
        for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
        {
            CUser* local_34 = P_SEATS(local_10);
            if ((local_34 != 0) && (local_14 = local_34->get_charac_guildkey(), local_14 != 0))
            {
                bool bVar3 = local_18 == local_14;
                local_18 = local_14;
                if (bVar3)
                {
                    return 1;
                }
            }
        }
    }
    return 0;
}

int PvP_Room::get_extra_win_point(int team)
{
    return (*(int (**)(void*, int))(*(char**)m_matching + 0x1c))((void*)m_matching, team);
}

int PvP_Room::battle_reset()
{
    lock();
    for (int local_14 = 0; local_14 < 8; local_14 = local_14 + 1)
    {
        P_WIN(local_14) = 0;
        P_TEAMWIN(local_14) = 0;
        P_SEATSTATE(local_14) = 0;
        if (P_SEATS(local_14) != 0)
        {
            sub_CHackAnalyzer_reportHackInfo(P_SEATS(local_14)->getHackAnalyzer());
            P_SEATS(local_14)->getHackAnalyzer()->resetHackInfo();
        }
    }
    memset(m_field5dc, 0, 0xc);
    m_field5d8 = 0;
    m_state = 1;
    m_recvFlagB4.Clear();
    m_recvFlagBC.Clear();
    m_userTable.Clear();
    m_fieldC0 = 0;
    m_relayMgr.Reset();
    m_field614 = 0;
    m_field615 = 0;
    m_field616 = 0;
    m_field617 = 0;
    for (int local_10 = 0x31; local_10 < 0x3f; local_10 = local_10 + 1)
    {
        if (local_10 != 0x3e)
        {
            gen_timer_key((TIMER_MESSAGE)local_10);
        }
    }
    unlock();
}

int PvP_Room::reset()
{
    battle_reset();
    lock();
    m_joinCount = 0;
    m_state = 0;
    m_pvpBattleMode = 2;
    m_mode8 = 0;
    memset(m_roomName, 0, 0x1e);
    m_seats[7] = 0;
    m_managerSeat = 0;
    for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
    {
        P_SEATS(local_10) = 0;
        P_TEAMS(local_10) = 0xff;
        P_SEATSTATE(local_10) = 0;
    }
    if (G_GameWorld()->GetChannelType() == 6)
    {
        m_guildWarLog.Clear();
    }
    m_relayMgr.Reset();
    m_relayMgr.SetParent(this);
    m_deathMatch.Reset();
    m_deathMatch.SetParent(this);
    m_field614 = 0;
    m_field615 = 0;
    m_field616 = 0;
    m_field617 = 0;
    unlock();
}

int PvP_Room::calculate_pvp_point()
{
    m_userTable.Clear();
    for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
    {
        if ((P_SEATS(local_10) != 0) &&
            (IsPvpObserver(local_10) == 0))
        {
            if ((m_pvpBattleMode == 1) || (m_pvpBattleMode == 4))
            {
                m_userTable.Set(local_10, P_SEATS(local_10));
            }
            else
            {
                m_userTable.Set(P_TEAMWIN(local_10), P_SEATS(local_10));
            }
        }
    }
    m_userTable.Calculate();
}

int PvP_Room::update_pvp_point()
{
    if (G_GameWorld()->IsPVPChannel() != 0)
    {
        if (sub_GameWorld_IsFreePvPChannel(G_GameWorld()) != 0)
        {
            for (int local_18 = 0; local_18 < 8; local_18 = local_18 + 1)
            {
                if ((P_SEATS(local_18) != 0) &&
                    (IsPvpObserver(local_18) == 0))
                {
                    sub_CUser_update_old_pvp_point(P_SEATS(local_18));
                }
            }
            goto LAB_085dca42;
        }
    }
    for (int local_14 = 0; local_14 < 8; local_14 = local_14 + 1)
    {
        if ((P_SEATS(local_14) != 0) &&
            (IsPvpObserver(local_14) == 0))
        {
            sub_CUser_update_old_pvp_point(P_SEATS(local_14));
            if ((m_pvpBattleMode == 1) || (m_pvpBattleMode == 4))
            {
                int iVar3 = P_SEATS(local_14)->get_pvp_grade();
                iVar3 = m_userTable.GetResult(local_14, false, iVar3);
                sub_CUser_update_pvp_point(P_SEATS(local_14), iVar3);
            }
            else
            {
                int iVar3 = P_SEATS(local_14)->get_pvp_grade();
                int iVar4 = P_SEATS(local_14)->get_pvp_grade();
                iVar3 = m_userTable.GetResult(P_TEAMWIN(local_14), iVar3 > 10, iVar4);
                sub_CUser_update_pvp_point(P_SEATS(local_14), iVar3);
            }
        }
    }
LAB_085dca42:
    ;
}

int PvP_Room::start_pvp()
{
    if (get_waiter_count() < 2)
    {
        goto LAB_085d91da;
    }
    {
        (*(int (**)(void*, short*, int*))(*(char**)m_matching + 0x18))(
            (void*)m_matching, &m_field5c4, &m_pvpBattleMode);
        if ((m_field5c4 < 0) || (m_field5c4 > sub_CMapList_GetPVPMapCount((char*)G_CDataManager() + 4)))
        {
            cMyTrace local_48("void PvP_Room::start_pvp()", 0x880, 0);
            local_48("START_PVP request_map_index(%d)", (int)m_field5c4);
            m_field5c4 = 0;
        }
        if (m_field5c4 == 0)
        {
            m_field5c6 = (short)SelectRandomMap();
            if ((m_field5c6 < 0) ||
                (m_field5c6 >= sub_CMapList_GetPVPMapCount((char*)G_CDataManager() + 4)))
            {
                cMyTrace local_38("void PvP_Room::start_pvp()", 0x88b, 0);
                local_38("START_PVP select_map_index(%d)", (int)m_field5c6);
            }
        }
        else
        {
            m_field5c6 = m_field5c4;
        }
        if (G_GameWorld()->GetChannelType() == 6)
        {
            m_guildWarLog.SetpvpStartTime(GlobalData_s_systemTime_.getCurSec());
        }
        int local_18 = 0;
        PacketGuard local_54;
        for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
        {
            if (P_SEATS(local_10) != 0)
            {
                local_18 = local_18 + 1;
                P_WIN(local_10) = 1;
                P_TEAMWIN(local_10) = (unsigned char)P_TEAMS(local_10);
                m_relayMgr.OnChangeTeam(local_10, (int)P_TEAMWIN(local_10));
                P_SEATS(local_10)->reset_pvp_masterid_walkingout_me();
                local_54.clear();
                local_54.put_header(0, 0x2d);
                local_54.put_byte((int)m_field5c6);
                local_54.put_byte(m_pvpBattleMode);
                local_54.finalize(true);
                P_SEATS(local_10)->Send(local_54);
                P_SEATS(local_10)->set_state((ch_state)7);
                sub_CHackAnalyzer_beginCollectHackInfo(P_SEATS(local_10)->getHackAnalyzer(),
                                                       P_SEATS(local_10));
                sub_CDisconnectDetecter_RegisterUser(sub_CDisconnectDetecter_GetInstance(),
                                                     P_SEATS(local_10));
            }
        }
        sub_Statistics_PVPPlay2((int)m_field5c6, m_pvpBattleMode, local_18);
        calculate_pvp_point();
        m_fieldBc = get_waiter_count();
        m_state = 2;
        if (m_pvpBattleMode == 3)
        {
            m_relayMgr.OnStart();
        }
        if ((m_pvpBattleMode == 4) || (m_pvpBattleMode == 5))
        {
            m_deathMatch.OnStart();
        }
        m_field618 = GlobalData_s_systemTime_.getCurSec();
        int local_14 = 0;
        switch (m_pvpBattleMode)
        {
        case 1:
        case 4:
            if (local_18 < 3)
            {
                local_14 = 0;
            }
            else
            {
                local_14 = 1;
            }
            break;
        case 2:
        case 5:
            if (local_18 < 3)
            {
                local_14 = 2;
            }
            else
            {
                local_14 = 3;
            }
            break;
        case 3:
            if (local_18 < 3)
            {
                local_14 = 4;
            }
            else
            {
                local_14 = 5;
            }
            break;
        }
        sub_CCommonStatisticsMgr_IncreaseQuantity(sub_GetInstanceCommonStatisticsMgr(), 1, local_14);
        if ((m_field5c6 >= 0) &&
            (m_field5c6 < sub_CMapList_GetPVPMapCount((char*)G_CDataManager() + 4)))
        {
            sub_CCommonStatisticsMgr_IncreaseQuantity(sub_GetInstanceCommonStatisticsMgr(),
                                                      2, (int)m_field5c6);
        }
        else
        {
            cMyTrace local_28("void PvP_Room::start_pvp()", 0x942, 0);
            local_28("START_PVP filter select_map_index(%d)", (int)m_field5c6);
        }
        (*(int (**)(void*))(*(char**)m_matching + 0x3c))((void*)m_matching);
    }
LAB_085d91da:
    ;
}

int PvP_Room::die_charac(CUser* killer, CUser* victim)
{
    int mode;
    int local_30;
    int local_2c;
    int local_28;
    int local_24;
    int local_20;
    {
        CSwitchLog local_40("int PvP_Room::die_charac(CUser*, CUser*)", 0x9a1, 0, 0);
        local_40("pvp@log %s,%s,%d,%d", victim->get_acc_name(), victim->getCurCharacName(),
                 get_index(), get_pvp_battle_mode());
    }
    if (m_state != 2)
    {
        return 0x13;
    }
    lock();
    local_30 = get_user_seat(victim);
    if (local_30 < 0)
    {
        unlock();
        return 0xffffffff;
    }
    if (P_WIN(local_30) != 1)
    {
        unlock();
        return 0x12;
    }
    P_WIN(local_30) = 0;
    unlock();
    PacketGuard local_4c;
    local_4c.put_header(0, 0x2e);
    local_4c.put_byte(local_30);
    if (killer == 0)
    {
        local_4c.put_byte(-1);
    }
    else
    {
        local_4c.put_byte(get_user_seat(killer));
    }
    local_4c.finalize(true);
    send_to_pvp(local_4c);
    if ((m_pvpBattleMode == 4) || (m_pvpBattleMode == 5))
    {
        local_2c = get_user_seat(killer);
        m_deathMatch.IncreaseKillCount(local_2c);
        m_deathMatch.IncreaseDeathCount(local_30);
        m_deathMatch.InsertTimerKilledUserForRevive(get_index(), local_30);
    }
    if (killer != 0)
    {
        local_28 = (int)killer->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
        void* local_5b;
        sub_MissionClearCondition_Parameter_C1(&local_5b, m_pvpBattleMode);
        sub_CMissionList_Charac_Update_Kill_event((void*)local_28, killer, &local_5b);
    }
    if (check_winner() == 0)
    {
        local_20 = get_user_seat(killer);
        m_relayMgr.OnDiePlayer(local_30, local_20, m_pvpBattleMode == 3);
        return 0;
    }
    local_24 = get_user_seat(killer);
    m_relayMgr.OnDiePlayer(local_30, local_24, false);
    pvp_request_pvp_rank();
    return 0xffffffff;
}

int PvP_Room::time_out(int* seats)
{
    if (m_field614 != 0)
    {
        return 0;
    }
    m_field614 = 1;
    int local_4c = 0;
    if (G_GameWorld()->GetChannelType() == 7)
    {
        local_4c = 1;
    }
    int local_48 = GlobalData_s_systemTime_.getCurSec() - m_field618;
    int local_44 = 0;
    int iVar2 = m_pvpBattleMode;
    if (0 < iVar2)
    {
        if (iVar2 < 3)
        {
            local_44 = PVP_NORMAL_BATTLE_TIME[local_4c] / 1000;
        }
        else if (iVar2 - 4U < 2)
        {
            local_44 = PVP_DM_BATTLE_TIME[local_4c] / 1000;
        }
    }
    if ((local_44 != local_48) && (local_44 - local_48 >= 0))
    {
        cMyTrace local_5c("int PvP_Room::time_out(int*)", 0xa9a, 0);
        local_5c("PVP_TIMEOUT_CHECK end_time(%d), play_time(%d)", local_44, local_48);
    }
    if (m_state != 2)
    {
        return 0x13;
    }
    char local_3d = 1;
    for (int local_3c = 0; local_3c < 7; local_3c = local_3c + 1)
    {
        if (seats[local_3c] != seats[local_3c + 1])
        {
            local_3d = 0;
            break;
        }
    }
    if ((m_pvpBattleMode == 2) || (m_pvpBattleMode == 3))
    {
        lock();
        if (local_3d == 1)
        {
            for (int local_24 = 0; local_24 < 8; local_24 = local_24 + 1)
            {
                P_WIN(local_24) = 0;
            }
        }
        else
        {
            int local_84[10];
            local_84[5] = 0;
            local_84[6] = 0;
            local_84[7] = 0;
            local_84[8] = 0;
            local_84[9] = 0;
            local_84[0] = 0;
            local_84[1] = 0;
            local_84[2] = 0;
            local_84[3] = 0;
            local_84[4] = 0;
            for (int local_30 = 0; local_30 < 8; local_30 = local_30 + 1)
            {
                if ((P_SEATS(local_30) != 0) && (P_TEAMWIN(local_30) != 0) &&
                    (P_TEAMWIN(local_30) < 0xfe))
                {
                    local_84[P_TEAMWIN(local_30) + 5] = local_84[P_TEAMWIN(local_30) + 5] + 1;
                    local_84[P_TEAMWIN(local_30)] =
                        local_84[P_TEAMWIN(local_30)] + seats[local_30];
                }
            }
            unsigned int local_38 = 0;
            int local_34 = 0x7fffffff;
            for (unsigned int local_2c = 1; (int)local_2c < 5; local_2c = local_2c + 1)
            {
                if ((0 < local_84[local_2c + 5]) && (local_84[local_2c] < local_34))
                {
                    local_38 = local_2c;
                    local_34 = local_84[local_2c];
                }
            }
            for (int local_28 = 0; local_28 < 8; local_28 = local_28 + 1)
            {
                if (P_TEAMWIN(local_28) == local_38)
                {
                    P_WIN(local_28) = 1;
                }
                else
                {
                    P_WIN(local_28) = 0;
                }
            }
            m_userTable.SetWinTeam(local_38);
        }
        unlock();
    }
    else if (m_pvpBattleMode == 5)
    {
        m_deathMatch.checkWinnerForTeamPlay(m_users, m_winCheck, m_teamWin, m_userTable);
    }
    else if (m_pvpBattleMode == 4)
    {
        m_deathMatch.checkWinnerForSinglePlay(m_users, m_winCheck, m_userTable);
    }
    else
    {
        lock();
        if (local_3d == 1)
        {
            for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
            {
                P_WIN(local_10) = 0;
            }
        }
        else
        {
            int local_20 = 0;
            int local_1c = 0x7fffffff;
            for (int local_18 = 0; local_18 < 8; local_18 = local_18 + 1)
            {
                if ((P_SEATS(local_18) != 0) && (seats[local_18] < local_1c))
                {
                    local_20 = local_18;
                    local_1c = seats[local_18];
                }
            }
            for (int local_14 = 0; local_14 < 8; local_14 = local_14 + 1)
            {
                if (local_20 == local_14)
                {
                    P_WIN(local_14) = 1;
                }
                else
                {
                    P_WIN(local_14) = 0;
                }
            }
            m_userTable.SetWinTeam(local_20);
        }
        unlock();
    }
    pvp_request_pvp_rank();
    return 0;
}

int PvP_Room::pvp_reward()
{
    int local_dc[8];
    int local_70 = GetRanking(local_dc);
    int local_6c = GetAce();
    if (G_GameWorld()->IsPVPChannel() != 0)
    {
        if (sub_GameWorld_IsFreePvPChannel(G_GameWorld()) != 1)
        {
            CalcuateVictoryPoint(local_dc, local_70);
        }
    }
    unsigned char local_66 = 1;
    unsigned char local_65 = 0;
    for (int local_64 = 0; local_64 < 8; local_64 = local_64 + 1)
    {
        if ((P_WIN(local_64) != 0) && (IsPvpObserver(local_64) != 1))
        {
            local_65 = (unsigned char)local_64;
            local_66 = 0;
            break;
        }
    }
    char local_5d = (char)check_acquire_guild_point();
    (*(int (**)(void*, void*, int, bool))(*(char**)m_matching + 0x4c))(
        (void*)m_matching, this, get_team((int)local_65), local_66 == 1);
    int local_5c = 0;
    while (true)
    {
        if (7 < local_5c)
        {
            if (G_GameWorld()->GetChannelType() == 6)
            {
                m_guildWarLog.WriteGuildWarPvPLog(m_index, (char)m_pvpBattleMode, m_users);
            }
            (*(int (**)(void*, void*))(*(char**)m_matching + 0x50))((void*)m_matching, this);
            break;
        }
        CUser* local_58 = P_SEATS(local_5c);
        if (local_58 != 0)
        {
            sub_CDisconnectDetecter_UnregisterUser(sub_CDisconnectDetecter_GetInstance(),
                                                   P_SEATS(local_5c));
            sub_CUser_add_pvp_play_info(local_58,
                                        GlobalData_s_systemTime_.getCurSec() - m_field618, 1);
            bool bVar15;
            if (G_GameWorld()->IsIntegratedPvPBaseChannel() != 0)
            {
                void* ev = sub_CEventManager_GetRepeatEvent_p(GlobalData_s_event_manager, 0x38);
                if ((*(int (**)(void*, int))(*(char**)ev + 0x34))(ev, 0) != 0 &&
                    IsPvpObserver(local_5c) == 0)
                {
                    struct Packet_Server_Match_data
                    {
                        char m_pad[0x28];
                    } local_a1;
                    sub_Packet_Server_Match_data_C1(&local_a1);
                    *((char*)&local_a1 + 0xa) = (char)(long)local_58->GetServerGroup();
                    if (P_WIN(local_5c) == 0)
                    {
                        *((char*)&local_a1 + 0xc) = 1;
                    }
                    else
                    {
                        *((char*)&local_a1 + 0xb) = 1;
                    }
                    void* proxy = sub_CServerProxyMgr_GetServerProxy(
                        GlobalData_s_statistic_proxy_mgr, (int)(long)local_58->GetServerGroup());
                    sub_CStatisticServerProxy_SendPacket(proxy, (char*)&local_a1, 0xd);
                    bVar15 = true;
                }
                else
                {
                    bVar15 = false;
                }
            }
            else
            {
                bVar15 = false;
            }
            unsigned int local_84 = 0;
            if (((local_5d == 0) || (P_WIN(local_5c) == 0)) ||
                (IsPvpObserver(local_5c) == 1))
            {
                bVar15 = false;
            }
            else
            {
                bVar15 = true;
            }
            if (bVar15)
            {
                sub_CUser_add_guild_point_item(local_58);
            }
            void* local_48 = local_58->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
            sub_CMissionList_Charac_setIssue(local_48, P_WIN(local_5c) != 0,
                                             m_pvpBattleMode == 2);
            char local_b0[0x40];
            sub_MissionClearCondition_Parameter_C1(local_b0, m_pvpBattleMode);
            short local_af = sub_CMissionList_Charac_getIssue_mode(local_48,
                                                                  m_pvpBattleMode == 2);
            bool local_b1 = false;
            sub_CMissionList_Charac_Update_Win_event(local_48, local_58, local_b0, local_b1);
            if (local_b1 != false)
            {
                const PvpResultType* r = local_58->getPVPResultRefR();
                int local_44 = sub_RefPvpGrade_GetPvpGrade(GlobalData_s_ref_pvp_grade,
                                                           *(int*)((const char*)r + 8));
                r = local_58->getPVPResultRefR();
                if (*(int*)((const char*)r + 0x14) < local_44)
                {
                    PvpResultType* w = local_58->getPVPResultRefW();
                    *(int*)((char*)w + 0x14) = local_44;
                }
            }
            local_58->send_MissionList();
            if (G_GameWorld()->IsPVPChannel() != 0)
            {
                if (sub_GameWorld_IsWinPointPvPChannel(G_GameWorld()) != 0)
                {
                    sub_CUser_add_pvp_result(local_58, P_WIN(local_5c) != 0, &local_84);
                    if ((m_pvpBattleMode == 4) || (m_pvpBattleMode == 5))
                    {
                        local_84 = 0;
                    }
                    if (P_WIN(local_5c) != 0)
                    {
                        if ((m_pvpBattleMode == 1) || (m_pvpBattleMode == 4))
                        {
                            sub_CUserProc_ClearActionAndSendtoUser(local_58, 0x12, 0, 0);
                        }
                        else if ((m_pvpBattleMode == 2) || (m_pvpBattleMode == 5))
                        {
                            sub_CUserProc_ClearActionAndSendtoUser(local_58, 0x13, 0, 0);
                        }
                        else if (m_pvpBattleMode == 3)
                        {
                            sub_CUserProc_ClearActionAndSendtoUser(local_58, 0x14, 0, 0);
                        }
                    }
                }
                else if (G_GameWorld()->GetChannelType() == 6)
                {
                    int local_40;
                    if (P_WIN(local_5c) == 0)
                    {
                        local_40 = -1;
                    }
                    else if (((m_pvpBattleMode == 2) || (m_pvpBattleMode == 3)) ||
                             (m_pvpBattleMode == 5))
                    {
                        local_40 = 1;
                    }
                    else
                    {
                        int local_3c = count_pvp_user();
                        if (local_3c < 4)
                        {
                            local_40 = 1;
                        }
                        else if (3 < local_3c)
                        {
                            local_40 = 2;
                        }
                    }
                    sub_CUser_add_guild_pvp_result(local_58, local_40);
                }
            }
            unsigned int local_54 = 0xffff;
            int local_50 = 0;
            int local_88 = 0;
            if (((m_pvpBattleMode == 3) &&
                 (local_50 = m_relayMgr.GetAllKillUserIndex(&local_88), local_50 >= 0)) &&
                (local_50 < 8) && (P_SEATS(local_50) != 0))
            {
                local_54 = (unsigned int)P_SEATS(local_50)->get_unique_id() & 0xffff;
            }
            PacketGuard local_94;
            local_94.clear();
            local_94.put_header(0, 0x2f);
            local_94.put_byte((int)local_65);
            if (G_GameWorld()->GetChannelType() == 0xf)
            {
                void* fair = local_58->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)7);
                local_94.put_int(sub_CFairPvPScore_GetLastTrainingSealCount_i(fair));
            }
            else if (G_GameWorld()->GetChannelType() == 0x10)
            {
                local_94.put_int(0);
            }
            else if (G_GameWorld()->GetChannelType() == 0xe)
            {
                local_94.put_int(0);
            }
            else
            {
                local_94.put_int(local_58->GetWinPoint());
            }
            local_94.put_byte(local_58->get_pvp_grade());
            if (G_GameWorld()->GetChannelType() == 6)
            {
                local_94.put_int(0);
            }
            else
            {
                int iVar14 = local_58->get_pvp_result_exp_point();
                int local_38 = iVar14 - local_58->get_pvp_result_oldexp_point();
                if ((local_54 != 0xffff) && (P_TEAMS(local_5c) == local_88))
                {
                    int iVar9 = GetTeamCount(local_88);
                    local_38 = local_38 + (iVar9 * 5 * local_38) / 100;
                }
                PvpResultType* w = local_58->getPVPResultRefW();
                int old = local_58->get_pvp_result_oldexp_point();
                *(int*)((char*)w + 8) = old + local_38;
                local_94.put_int(local_38);
                const PvpResultType* r = local_58->getPVPResultRefR();
                sub_CUser_LogHistory(local_58, "PvPExp,%d,%d,%d",
                                     *(int*)((const char*)r + 8), old, local_38);
            }
            int sVar4 = sub_CUser_has_within_Mission(local_58);
            if (sVar4 == 0)
            {
                sub_CUser_acceptable_within_mission(local_58);
            }
            local_94.put_byte(local_70);
            for (int local_34 = 0; local_34 < local_70; local_34 = local_34 + 1)
            {
                int local_30 = local_dc[local_34];
                if (P_SEATS(local_30) != 0)
                {
                    local_94.put_short((int)P_SEATS(local_30)->get_unique_id() & 0xffff);
                    local_94.put_int(*(int*)((char*)P_SEATS(local_30) + 0x78));
                    local_94.put_byte(P_SEATS(local_30)->get_pvp_grade());
                    local_94.put_int(P_SEATS(local_30)->get_pvp_result_exp_point());
                    local_94.put_int(sub_RefPvpGrade_GetPvpCurrRankPoint(
                        GlobalData_s_ref_pvp_grade, P_SEATS(local_30)->get_pvp_grade()));
                    local_94.put_int(sub_RefPvpGrade_GetPvpNextRankPoint(
                        GlobalData_s_ref_pvp_grade, P_SEATS(local_30)->get_pvp_grade()));
                    int local_2c = m_deathMatch.getKillCount(local_30);
                    int local_28 = m_deathMatch.getDeathCount(local_30);
                    local_94.put_int(local_2c);
                    local_94.put_int(local_28);
                    if ((m_pvpBattleMode == 4) || (m_pvpBattleMode == 5))
                    {
                        cMyTrace local_80("void PvP_Room::pvp_reward()", 0xdab, 0);
                        local_80("<PVP_REWARD> Room(%d), iRank(%d), iRankMember(%d), Player(%s), mode(%d), KillCnt(%d), DeathCnt(%d)",
                                 get_index(), local_34, local_30,
                                 P_SEATS(local_30)->getCurCharacName(), m_pvpBattleMode,
                                 local_2c, local_28);
                    }
                }
            }
            if (local_70 != 0)
            {
                if ((local_6c == -1) || (P_SEATS(local_6c) == 0))
                {
                    local_94.put_short(0);
                }
                else
                {
                    local_94.put_short((int)P_SEATS(local_6c)->get_unique_id() & 0xffff);
                }
            }
            local_94.put_short((int)local_54);
            local_94.put_int((int)local_84);
            local_94.put_byte((int)local_66);
            if (G_GameWorld()->GetChannelType() == 0xf)
            {
                void* fair = local_58->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)7);
                local_94.put_short(sub_CFairPvPScore_GetLastTrainingSealBonusCount_i(fair));
                fair = local_58->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)7);
                local_94.put_short(sub_CFairPvPScore_GetRemainBonusCount_i(fair));
            }
            else if (G_GameWorld()->GetChannelType() == 0x10)
            {
                local_94.put_short(0);
                local_94.put_short(0);
            }
            sVar4 = sub_CUser_has_within_Mission(local_58);
            local_94.put_byte(sVar4 != 0);
            unsigned char local_49 = 0xff;
            if (m_pvpBattleMode == 3)
            {
                local_49 = (unsigned char)m_relayMgr.GetKillCount(local_5c);
            }
            local_94.put_byte((int)local_49);
            local_94.finalize(true);
            local_58->Send(local_94);
            int mode = get_pvp_battle_mode();
            if (((mode == 4) || (get_pvp_battle_mode() == 5)) && (local_84 != 0))
            {
                int local_b8 = 0;
                int local_bc = 0;
                if (sub_CUser_gain_exp_sp_i(local_58, (int)local_84, local_b8, local_bc,
                                            10, 0, 0) != 0)
                {
                    local_58->SendNotiPacket((CUser::eSendTarget)2, (ENUM_NOTIPACKET)2, 1);
                }
            }
            long long lVar13 = GlobalData_s_systemTime_.getCurSec();
            sub_CHackAnalyzer_setLastMonsterDeadTime(local_58->getHackAnalyzer(), lVar13);
            void* ev = sub_CEventManager_GetRepeatEvent_p(GlobalData_s_event_manager, 0x37);
            if ((*(int (**)(void*, int))(*(char**)ev + 0x34))(ev, 0) != 0)
            {
                void* live = local_58->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)3);
                sub_CPvPLiveEventData_IncreasePlayCount(live, local_58);
            }
            const PvpResultType* r2 = local_58->getPVPResultRefR();
            int local_24 = sub_RefPvpGrade_GetPvpGrade(GlobalData_s_ref_pvp_grade,
                                                       *(int*)((const char*)r2 + 8));
            if ((local_24 < 1) ||
                (*(int*)((const char*)local_58->getPVPResultRefR() + 0x14) == local_24))
            {
                bVar15 = false;
            }
            else
            {
                bVar15 = true;
            }
            if (bVar15)
            {
                PvpResultType* w2 = local_58->getPVPResultRefW();
                *(int*)((char*)w2 + 0x14) = local_24;
                void* mission = local_58->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
                sub_CMissionList_Charac_MakeMissionList_forOldUser(mission, local_58);
                sub_GameWorld_send_AllBasicInfo(G_GameWorld(), local_58);
            }
        }
        local_5c = local_5c + 1;
    }
}

int PvP_Room::DrawRandomMapIndex(std::vector<int>& out)
{
    bool local_21 = false;
    std::vector<int> local_30;
    std::vector<int>::iterator it = out.begin();
    while (it != out.end())
    {
        if (IsDeathMatchMap(*it, local_21) != 0)
        {
            local_30.push_back(*it);
            it = out.erase(it);
        }
        else
        {
            ++it;
        }
    }
    int local_1c = 0;
    if ((local_30.size() == 0) && (out.size() == 0))
    {
        local_1c = rand() % sub_CMapList_GetPVPMapCount((char*)G_CDataManager() + 4) + 1;
    }
    else if ((local_30.size() == 0) && (out.size() != 0))
    {
        local_1c = out[rand() % out.size()];
    }
    else if ((out.size() == 0) && (local_30.size() != 0))
    {
        local_1c = local_30[rand() % local_30.size()];
    }
    else
    {
        local_1c = out[rand() % out.size()];
    }
    return local_1c;
}

int PvP_Room::SelectRandomMap()
{
    std::vector<int> local_1c;
    GetCandidateMapIndexList(local_1c);
    int local_10 = DrawRandomMapIndex(local_1c);
    ChangePvPMode(local_10);
    return local_10;
}

int PvP_Room::GetAce() const
{
    int local_10 = -1;
    int local_c = -1;
    for (int local_8 = 0; local_8 < 8; local_8 = local_8 + 1)
    {
        if (P_SEATS(local_8) != 0)
        {
            if (local_c < *(int*)((char*)P_SEATS(local_8) + 0x98))
            {
                local_c = *(int*)((char*)P_SEATS(local_8) + 0x98);
                local_10 = local_8;
            }
            else if ((*(int*)((char*)P_SEATS(local_8) + 0x98) == local_c) &&
                     (*(int*)((char*)P_SEATS(local_10) + 0x78) <
                      *(int*)((char*)P_SEATS(local_8) + 0x78)))
            {
                local_10 = local_8;
            }
        }
    }
    return local_10;
}

int PvP_Room::GetRanking(int* out) const
{
    if ((m_pvpBattleMode == 4) || (m_pvpBattleMode == 5))
    {
        return m_deathMatch.getRanking(const_cast<CUser**>(m_users), out);
    }
    int local_24 = 0;
    int aiStack_44[8];
    int local_14;
    for (int local_20 = 0; local_20 < 8; local_20 = local_20 + 1)
    {
        if (P_SEATS(local_20) != 0)
        {
            int local_1c = *(int*)((char*)P_SEATS(local_20) + 0x78);
            int local_18 = 0;
            for (local_18 = 0;
                 (local_18 < local_24 &&
                  (local_14 = aiStack_44[local_18], local_1c <= local_14));
                 local_18 = local_18 + 1)
            {
            }
            for (int local_10 = local_24; local_18 < local_10; local_10 = local_10 - 1)
            {
                out[local_10] = out[local_10 - 1];
                aiStack_44[local_10] = aiStack_44[local_10 - 1];
            }
            out[local_18] = local_20;
            aiStack_44[local_18] = local_1c;
            local_24 = local_24 + 1;
        }
    }
    return local_24;
}

int PvP_Room::SetPVPResult(int seat, const PvpResultType& result)
{
    if ((seat >= 0) && (seat < 8))
    {
        *(int*)((char*)P_SEATS(seat) + 0x78) = sub_PvpResultType_GetTotalScore(&result);
        *(int*)((char*)P_SEATS(seat) + 0x98) = *(int*)((const char*)&result + 0x18);
    }
}
