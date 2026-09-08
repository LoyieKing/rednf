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
#include "PacketGuard.h"
#include "TimerQueue.h"
#include "GameWorld.h"
#include "CDataManager.h"
#include "CGameManager.h"        // G_CGameManager / PutPvp / CheckOutPvp
#include "GlobalData.h"          // GlobalData::s_event_manager / s_statistic_proxy_mgr / s_ref_pvp_grade / s_systemTime_
#include "CGuildServerProxy.h"   // CEventManager::GetRepeatEvent
#include "CFairPvPScore.h"
#include "RefPvpGrade.h"
#include "CMissionList_Charac.h"
#include "CPvPLiveEventData.h"
#include "CStatisticServerProxy.h"
#include "CServerProxyMgr.h"
#include "CCommonStatisticsMgr.h"
#include "CDisconnectDetecter.h"
#include "Statistics.h"
#include "CUserProc.h"
#include "Packet_Server_Match_data.h"
#include "MatchRoomList.h"     // IMatch（m_matching 虚表 36 槽权威声明）

// ============================================================================
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
// PvP 全局数据（ORIG 定义）：
//   PVP_NORMAL_BATTLE_TIME 0x93f4ee0 / PVP_DM_BATTLE_TIME 0x93f4ee8，各 [2] int
//   —— ORIG 中与 g_equipmentPartSetMap_/characterEquipmentTypeTags 同 TU，
//      定义归 InitScriptProducers_2.cpp（首个具名对象，键名 _GLOBAL__I_PVP_NORMAL_BATTLE_TIME）。
//   PVPMAP_DEATHMATCH_MODE 0x94f70e0 / PVPMAP_ONLY_NORMAL_EXPOSURE 0x94f7108
//   PVPMAP_ONLY_TOURNAMENT_RANDOM 0x94f711c，各 0x14 char（零初始化 BSS）
// ============================================================================
extern int PVP_NORMAL_BATTLE_TIME[2];
extern int PVP_DM_BATTLE_TIME[2];
unsigned char PVPMAP_DEATHMATCH_MODE[0x14] = {0};
char PVPMAP_ONLY_NORMAL_EXPOSURE[0x14] = {0};
char PVPMAP_ONLY_TOURNAMENT_RANDOM[0x14] = {0};
// ============================================================================
// 自由函数
// ============================================================================
// ORIG 0x85d4690：查 PvP map；onlyDeathMatch = info.m_playable；
// mapNo∈[0,0x13] 时返回 PVPMAP_DEATHMATCH_MODE[mapNo]
// 末段 ORIG `movzbl tbl(%eax),%eax` 全字直返（无 setne 归一）⇒ 原返回 int +
// (unsigned char) 提升（mangled 无返回类型，零 ABI 风险，技巧33）。
unsigned char IsDeathMatchMap(int mapNo, bool& onlyDeathMatch)
{
    PvPMapInfo info;
    if (CMapList::GetPvpMapInfo(mapNo, info))
    {
        onlyDeathMatch = info.m_playable;
        if (mapNo > 0x13 || mapNo < 0)
            return 0;
        return PVPMAP_DEATHMATCH_MODE[mapNo];
    }
    return 0;
}
// ORIG 0x855c8f4：按频道类型返回匹配类型（9→2, 10→3, 14→4, 15→5, 其余→0）
int GetMatchingType()
{
    int type = 0;
    switch (G_GameWorld()->GetChannelType())
    {
    case 9:  type = 2; break;
    case 10: type = 3; break;
    case 14: type = 4; break;
    // ORIG 0855c946: case15 赋值后 fall-through 到 merge（无 default 复写；
    // 0 已由初始化 0855c8fa 提供）。
    case 15: type = 5; break;
    }
    return type;
}
struct PSeatsView { CUser* m_arr[24]; };
// ORIG 直用成员 m_users[i]（+0x30）/m_teams[i]（+0x50）：偏移为 4 倍数，
// gcc 折叠为 (i + off/4)*4 scale-index（0x30/4=0xc、0x50/4=0x14）。
#define P_SEATS(i)   (m_users[i])
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

unsigned char PvP_Room::isRematchable() const
{
    return m_field61c;
}

void PvP_Room::setReamtchable(bool flag)
{
    m_field61c = flag;
}

int PvP_Room::GetRequestedMapNo()
{
    return (int)m_requestedMapNo;
}

void PvP_Room::SetRequestedMapNo(short mapNo)
{
    m_requestedMapNo = mapNo;
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
// CheckTimerKey 0x863bccc，报告缺失，objdump 取证）：计数数组即成员
// m_field22c（+0x22c）；ORIG 寻址折叠为 0xc(%this,(idx+0x88)*4)（0x22c=
// 0xc+0x88*4，gcc scale-index 折叠），索引 (int)msg-0x30 越界区为相邻数组带。
#define P_TIMER_KEY(m)   (m_field22c[(int)(m) - 0x30])

unsigned int PvP_Room::gen_timer_key(TIMER_MESSAGE msg)
{
    return ++P_TIMER_KEY(msg);
}

int PvP_Room::get_timer_key(TIMER_MESSAGE msg)
{
    return P_TIMER_KEY(msg);
}

bool PvP_Room::CheckTimerKey(TIMER_MESSAGE msg, int key)
{
    return P_TIMER_KEY(msg) == key;
}
unsigned char PvP_Room::IsExistPassword()
{
    return m_isExistPassword;
}

bool PvP_Room::IsEndPvpBattle()
{
    return m_isEndPvpBattle;
}

unsigned char PvP_Room::IsInsertTimerRecvPvpRank()
{
    return m_isInsertTimerRecvPvpRank;
}

// ----------------------------------------------------------------------------
// 积分计算
// ----------------------------------------------------------------------------

int PvP_Room::CalculateVictoryPointOfSinglePlay(int rank, bool win, int score)
{
    int local_10;
    if (win)
    {
        return score + 1;                 // ORIG win 分支直返（不写 local_10）
    }
    else if (rank == 1)
    {
        local_10 = (score + 1) / 2;
    }
    else if (rank == 2)
    {
        local_10 = (score + 1) / 4;
    }
    else if (rank == 3)
    {
        local_10 = (score + 1) / 8;
    }
    else if (rank == 4)
    {
        local_10 = (score + 1) / 16;
    }
    else if (rank == 5)
    {
        local_10 = (score + 1) / 32;
    }
    else if (rank == 6)
    {
        local_10 = (score + 1) / 64;
    }
    else if (rank == 7)
    {
        local_10 = (score + 1) / 128;
    }
    else if (rank == 0)
    {
        local_10 = 1;
    }
    else
    {
        cMyTrace local_20(__PRETTY_FUNCTION__,
                          0xb50, 5);
        local_20("invalid rank %d", rank);
    }
    return local_10;
}

int PvP_Room::CalculateVictoryPointOfTeamPlay(int rank, bool win, int score)
{
    // ORIG 085da1f0/085da203：两条路径均不改写 score 形参槽（0x14(%ebp) 只读），
    // 用 return 表达式而非赋值后统一返回。
    if ((win) && (rank == 0))
    {
        return score + 2;
    }
    else if (!win)
    {
        return 1;
    }
    return score;
}

void PvP_Room::CalcuateVictoryPoint(int* rank, int rankCount)   // [b28c-16 SIGFIX] ORIG 串 void
{
    if ((m_pvpBattleMode == 1) || (m_pvpBattleMode == 4))
    {
        for (int local_2c = 0; local_2c < 8; local_2c = local_2c + 1)
        {
            if (P_SEATS(local_2c) != 0)
            {
                bool bVar1;
                if (P_SEATS(local_2c) == 0)
                {
                    bVar1 = false;
                }
                else
                {
                    bVar1 = IsPvpObserver(local_2c);
                }
                if (!bVar1)
                {
                    int local_28 = 0;
                    while (local_28 < rankCount)
                    {
                        if (rank[local_2c] == local_2c)
                            break;
                        local_28 = local_28 + 1;
                    }
                    if (local_28 == rankCount)
                    {
                        cMyTrace local_54(__PRETTY_FUNCTION__, 0xb7c, 5);
                        local_54("1 PvP_Room::CalcuateVictoryPoint Error, No Rank User Found i<%d> RankCount<%d>",
                                 local_2c, rankCount);
                        goto LAB_085da583;
                    }
                    int local_34 = local_28;
                    bool local_2d = m_userTable.GetWinTeam() == local_2c;
                    P_SEATS(local_2c)->m_winPointBuffer =
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
                if (P_SEATS(local_24) == 0)
                {
                    bVar1 = false;
                }
                else
                {
                    bVar1 = IsPvpObserver(local_24);
                }
                if (!bVar1)
                {
                    int local_20 = GetTeamIndex(P_SEATS(local_24));
                    int local_1c = 0;
                    while (local_1c < rankCount)
                    {
                        if (rank[local_24] == local_24)
                            break;
                        local_1c = local_1c + 1;
                    }
                    if (local_1c == rankCount)
                    {
                        cMyTrace local_44(__PRETTY_FUNCTION__, 0xb97, 5);
                        local_44("1 PvP_Room::CalcuateVictoryPoint Error, No Rank User Found i<%d> RankCount<%d>",
                                 local_24, rankCount);
                        goto LAB_085da583;
                    }
                    int local_18 = local_1c;
                    int local_34 = local_1c;
                    int local_14 = local_1c;
                    while (local_14 = local_14 - 1, -1 < local_14)
                    {
                        int local_10 = rank[local_14];
                        bool bVar3;
                        if (P_SEATS(local_10) == 0)
                        {
                            bVar3 = false;
                        }
                        else
                        {
                            bVar3 = IsPvpObserver(local_10);
                        }
                        if ((!bVar3) &&
                            (GetTeamIndex(P_SEATS(local_10)) != local_20))
                        {
                            local_34 = local_34 - 1;
                        }
                    }
                    bool local_2d = m_userTable.GetWinTeam() == local_20;
                    P_SEATS(local_24)->m_winPointBuffer =
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

bool PvP_Room::CheckPassword(const char* password, int len)
{
    if (m_passwordLen != len)
    {
        return 0;
    }
    return memcmp(m_password, password, m_passwordLen) == 0;
}

int PvP_Room::get_player_count()
{
    int local_14 = 0;
    for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
    {
        if (P_SEATS(local_10) != 0 && !IsPvpObserver(local_10))
        {
            local_14 = local_14 + 1;
        }
    }
    return local_14;
}

bool PvP_Room::IsPvpObserver(int seat)
{
    if (seat > 7)
    {
        return 0;
    }
    if ((m_teams[seat] == 3) || (m_teams[seat] == 4))
    {
        return 1;
    }
    return 0;
}

int PvP_Room::check_joinable(CUser* user)
{
    int local_10;
    if (m_state == 1)
    {
        if ((local_10 = ((IMatch*)m_matching)->CheckJoinable(this, user)) != 0)
        {
            return local_10;
        }
        for (int local_f4 = 0; local_f4 < 8; local_f4 = local_f4 + 1)
        {
            if ((P_SEATS(local_f4) == 0) && (m_teams[local_f4] == 0xff))
            {
                return 0;
            }
        }
        return 4;
    }
    else
    {
        return 4;
    }
}

int PvP_Room::IsQuickJoinablePVPRoom(CUser* user)
{
    // P24：ORIG 内联实参（0x085d6c1c），getCurCharacNo 结果直传不落栈槽。
    // ORIG 0x085dc18 逐段：layer1 `call check_joinable; test%eax; jne L0(mov$0)`；
    // layer2 `call find…; xor$1; test%al; je L0`；真值路 mov$1，尾 `test%al;
    // je; mov$1/jmp; mov$0` 双归一岛（&&-结果岛 + return 归一岛）。P0：结果全程
    // %eax 无槽 ⇒ 源形 = 短路 && 条件 + if-return 1/0（无具名局部，P56 形）。
    if (check_joinable(user) == 0 &&
        !user->find_pvp_masterid_walkingout_me(m_seats[7]->getCurCharacNo()))
        return 1;
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
    for (int local_8 = 0; local_8 <= 7; local_8 = local_8 + 1)
    {
        if (P_SEATS(local_8) == user)
            return local_8;
    }
    return -1;
}

int PvP_Room::get_user_seat(int seat)
{
    // ORIG 085d921e：无帧；`mov $0;jmp pop` 块物理在前、body 尾块 fall 直 ret。
    // 越界或形守卫（js/jle 双入 return0 块）复现块序。
    if ((seat < 0) || (seat > 7))
        return 0;
    return (int)P_SEATS(seat);
}

int PvP_Room::GetTeamIndex(CUser* user)
{
    // ORIG 0x085dc6ee：for+jmp-check+break 岛；-0x4=游标、-0x8=结果。
    int local_8;
    int local_4;
    for (local_4 = 0; local_4 < 8; local_4 = local_4 + 1)
    {
        if (P_SEATS(local_4) == 0)
        {
            continue;
        }
        if (P_SEATS(local_4) == user)
        {
            break;
        }
    }
    if (local_4 == 8)
    {
        return -1;
    }
    if ((m_pvpBattleMode == 1) || (m_pvpBattleMode == 4))
    {
        local_8 = local_4;
    }
    else
    {
        local_8 = P_TEAMWIN(local_4);
    }
    return local_8;
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
            if (P_SEATS(local_10) == 0)
            {
                bVar1 = false;
            }
            else
            {
                bVar1 = !IsPvpObserver(local_10);
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
            if (P_SEATS(local_10) == 0)
            {
                bVar1 = false;
            }
            else
            {
                bVar1 = !IsPvpObserver(local_10);
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

int PvP_Room::get_remain_seat() const
{
    return 8 - get_waiter_count();
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

int PvP_Room::count_pvp_user()
{
    // ORIG 0x085da840：continue 形产生 true 尾 jmp merge + false nop 岛。
    int local_c = 0;
    for (int local_8 = 0; local_8 < 8; local_8 = local_8 + 1)
    {
        if (P_SEATS(local_8) == 0)
        {
            continue;
        }
        local_c = local_c + 1;
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

bool PvP_Room::check_start_state()
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
        if (P_SEATS(local_10) == 0)
        {
            bVar1 = false;
        }
        else
        {
            bVar1 = !IsPvpObserver(local_10);
        }
        if ((bVar1) && (!P_SEATSTATE(local_10)))
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
    for (int local_10 = 0; local_10 <= 7; local_10 = local_10 + 1)
    {
        bool bVar1;
        if (P_SEATS(local_10) == 0)
        {
            bVar1 = false;
        }
        else
        {
            bVar1 = !IsPvpObserver(local_10);
        }
        if (bVar1)
        {
            local_18 = local_18 + 1;
        }
        if (P_TEAMS(local_10) == 0xff)
        {
            local_14 = local_14 + 1;
        }
    }
    if ((local_18 == 1) && (local_14 < 2))
    {
        return 0;
    }
    return 1;
}

int PvP_Room::get_recv_pvp_rank_count()
{
    // ORIG 0x085dc2cc：bool IsRecv 直用条件（test %al/je，无 !=0 物化）；
    // 槽 -0x8=count、-0x4=i，帧 $0x18。
    // N 封顶：callee PvpResultRecvFlag::IsRecv 属外 TU T 符号（4.4 不可见 ⇒ 恒保守
    // +0x10 → 帧 $0x28 vs $0x18），N95 槽基 gap 差（4.1 局部顶到 -0x4，4.4 恒 -0x8 起）
    // ⇒ 全帧槽号平移；另 4.1 三 je 分落独立 nop/jmp 岛、4.4 合并共享 inc 出口
    // （CFG 简化跨版本差）。指令语义全等，勿再耗轮次。
    int local_c = 0;
    for (int local_8 = 0; local_8 <= 7; local_8 = local_8 + 1)
    {
        if (P_SEATS(local_8) != 0)
        {
            if (P_TEAMS(local_8) != 0xff)
            {
                if (m_recvFlagB4.IsRecv(local_8))
                {
                    local_c = local_c + 1;
                }
            }
        }
    }
    return local_c;
}

int PvP_Room::get_recv_pvp_end_result_count() const
{
    int local_c = 0;
    for (int local_8 = 0; local_8 <= 7; local_8 = local_8 + 1)
    {
        // [b28c-16 定论 N95/N82] ORIG 三失败出口分落独立岛（je +0x5b:nop;jmp /
        // +0x5e:nop / +0x5f）+ 帧 $0x18/槽 -0x4；4.4 因 callee 属外 TU 恒保守
        // +0x10（帧 $0x28）且槽基 gap 差 ⇒ 全帧槽号平移，嵌套 if 恒合并单 inc
        // 出口。goto 尾空标签形实测反产 xor$1+额外 jmp（39>35）⇒ 保持嵌套 if。
        if (P_SEATS(local_8) != 0)
        {
            if (P_TEAMS(local_8) != 0xff)
            {
                if (m_recvFlagBC.IsRecv(local_8))
                {
                    local_c = local_c + 1;
                }
            }
        }
    }
    return local_c;
}

int PvP_Room::recv_pvp_rank_all() const
{
    for (int local_8 = 0; local_8 <= 7; ++local_8)
    {
        if (P_SEATS(local_8) == 0)
            continue;
        if (P_TEAMS(local_8) == 0xff)
            continue;
        if (m_recvFlagB4.IsRecv(local_8) == false)
            return 0;
    }
    return 1;
}

int PvP_Room::recv_pvp_end_result_all() const
{
    for (int local_8 = 0; local_8 <= 7; ++local_8)
    {
        if (P_SEATS(local_8) == 0)
            continue;
        if (P_TEAMS(local_8) == 0xff)
            continue;
        if (m_recvFlagBC.IsRecv(local_8) == false)
            return 0;
    }
    return 1;
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
    if (m_recvFlagB4.IsRecv(iVar2))
    {
        return 0;
    }
    m_recvFlagB4.SetRecvFlag(iVar2);
    return 1;
}

int PvP_Room::set_recv_pvp_end_result_flag(CUser* user)
{
    // ORIG 0x085dbfec：seat<0 早退 mov $0（fall 侧），显式 if/return 物化，
    // 非 bool 算术化（避免 not/shr 折叠）。
    int seat = get_user_seat(user);
    if (seat < 0)
    {
        return 0;
    }
    m_recvFlagBC.SetRecvFlag(seat);
    return 1;
}
bool PvP_Room::is_last_win(int seat)
{
    if (P_SEATS(seat) == 0)
    {
        return false;
    }
    int iVar1 = GetTeamIndex(P_SEATS(seat));
    return m_userTable.GetWinTeam() == iVar1;
}

bool PvP_Room::IsAllMoreThanGrade(int grade)
{
    if ((grade < 0x26) && (grade >= 0))
    {
        for (int local_14 = 0; local_14 < 8; local_14 = local_14 + 1)
        {
            if ((P_SEATS(local_14) != 0) &&
                (P_SEATS(local_14)->get_pvp_grade() < grade))
            {
                return false;
            }
        }
        return true;
    }
    return false;
}

int PvP_Room::check_acquire_guild_point()
{
    if (G_GameWorld()->IsPVPChannel())
    {
        if (G_GameWorld()->IsFreePvPChannel())
        {
            return 0;
        }
        if (!IsAllMoreThanGrade(10))
        {
            return 0;
        }
        if (IsThereSameGuildMember())
        {
            return 0;
        }
        return 1;
    }
    return 0;
}
bool PvP_Room::check_winner()
{
    // ORIG 0x085d98ea（274 条；本侧 290，+16 全为 N 类形态差，台账豁免）：
    //  - 帧调整 4.1 `add $0xffffff80,%esp`（负常量折叠）vs 4.4 `sub $0x90`；
    //    帧洞 0x10（outgoing frame N）；
    //  - local_6c[P_TEAMWIN+3] 索引 4.1 单 lea 折叠、4.4 拆 mov+add 两段
    //    （N18a/N76）；bVar 常量岛 `mov$1/mov$0/test;je`×3（setcc-fold）；
    //  - mode5 段 &m_deathMatch 先期 lea 缓存（P52 callee-saved），4.4 实参处
    //    现算。GetRanking/insert 调用链与错误码逐段核对无语义缺口。
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
        cMyTrace local_54(__PRETTY_FUNCTION__, 0xa3c, 0);
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
        cMyTrace local_44(__PRETTY_FUNCTION__, 0xa46, 0);
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
    // ORIG +0x25..+0x41：三值以 callee-saved（%edi/%esi/%ebx）跨后续调用缓存
    // （P87 register 形），日志首参为 __PRETTY_FUNCTION__ 局部静态（P118/P139）。
    register int roomIdx = m_index;
    register const char* characName = user->getCurCharacName();
    register const char* accName = user->get_acc_name();
    {
        CSwitchLog local_30(__PRETTY_FUNCTION__, 0x43a, 0, 0);
        local_30("pvp@log %s,%s,room(%d),%d", accName, characName, roomIdx, local_48);
    }
    if (user->getCurCharacR() == 0)
    {
        return 0x13;
    }
    m_matching = (int)m_pMatchingSystem->GetMatchingSystem((ENUM_PVP_MATCHING_TYPE)GetMatchingType());
    ((IMatch*)m_matching)->RequestMap(m_requestedMapNo,
                                      reinterpret_cast<PVP_BATTLE_MODE&>(m_pvpBattleMode));
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
    user->SetPvpIndex((short)m_index);
    m_joinCount = 1;
    m_requestedMapNo = local_44;
    m_playMapNo = m_requestedMapNo;
    m_isExistPassword = local_45 ? 1 : 0;
    if (m_isExistPassword != 0)
    {
        m_passwordLen = d;
        memset(m_password, 0, 9);
        memcpy(m_password, password, m_passwordLen);
    }
    m_relayMgr.OnCreateRoom(m_managerSeat);
    m_deathMatch.OnCreateRoom(m_managerSeat);
    m_pvpBattleMode = local_48;
    int ret = 0;
    if ((ret = ((IMatch*)m_matching)->OnCreateRoom(user)) != 0)
    {
        return ret;
    }
    return 0;
}

// §6.2 巨件核销：P167 102/102 [0缺0多]；gap 702→+7；残差归因 N82（ORIG 帧
// 0x9c/21 槽 vs 4.4 帧 0xac/23 槽，PacketGuard/CSwitchLog 对象槽平移）+ 4.4
// 布尔物化（sete/cmpb/movb/jcc+1）；逐块人工比对语义与 Ghidra C 一致（含
// OnJoinRoom(seat) 实参、m_state==2 判 0x13、双 PacketGuard 布局；详见
// docs/asm_translation_patterns.md 负例表）⇒ N 封顶。
int PvP_Room::join_room(CUser* user, int& seat, bool flag)
{
    int local_48 = get_waiter_count();
    int local_7c[3];
    PacketGuard local_64;
    PacketGuard local_70;
    {
        CSwitchLog local_58(__PRETTY_FUNCTION__, 0x4a8, 0, 0);   // ORIG _ZZ…join_room…__PRETTY_FUNCTION__ 同文（P139）
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
        int iVar9 = G_CDataManager()->GetMaxGradePvPChannel();
        int iVar10 = user->get_pvp_grade();
        if (iVar9 < iVar10)
        {
            return 0xd;
        }
        int iVar11 = (int)((fair_pvp::CFairPvPScore*)user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)7))->GetGiveItem();
        iVar10 = user->get_pvp_grade();
        iVar11 = G_CDataManager()->m_serverParameter.get_pvp_item_give(iVar10);
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
            local_70.put_int(P_SEATS(local_3c)->m_network.get_inner_ip());
            local_70.put_int(P_SEATS(local_3c)->m_network.get_ip());
            local_70.put_short(P_SEATS(local_3c)->m_network.get_port() & 0xffff);
            local_70.put_int((int)P_SEATS(local_3c)->get_acc_id());
            local_70.put_byte(P_SEATS(local_3c)->m_network.get_nat_type());
            local_70.put_int(P_SEATS(local_3c)->m_network.get_mtu());
            // ORIG 0x085d709d..0x085d70c3（P48/P0 去缓存）：条件臂与 else 臂各自
            // 独立调用 getCurCharacR() 一次，不缓存到具名局部（缓存则单 call 不同形）。
            if (P_SEATS(local_3c)->getCurCharacR() == 0)
            {
                local_70.put_byte(0);
            }
            else
            {
                local_70.put_byte(*(unsigned char*)((const char*)P_SEATS(local_3c)->getCurCharacR() + 0x75));
            }
        }
    }
    unlock();
    local_64.finalize(true);
    local_70.finalize(true);
    lock();
    int local_44 = ((IMatch*)m_matching)->CheckJoinable(this, user);
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
            user->SetPvpIndex((short)m_index);
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
    local_70.put_int(user->m_network.get_inner_ip());
    local_70.put_int(user->m_network.get_ip());
    local_70.put_short(user->m_network.get_port() & 0xffff);
    local_70.put_int((int)user->get_acc_id());
    local_70.put_byte(user->m_network.get_nat_type());
    local_70.put_int(user->m_network.get_mtu());
    // ORIG 0x085d..（P48/P0 去缓存）：条件臂与 else 臂各自独立调用
    // getCurCharacR() 一次（Ghidra C 双臂 getter），不缓存到具名局部。
    if (user->getCurCharacR() == 0)
    {
        local_70.put_byte(0);
    }
    else
    {
        local_70.put_byte(*(unsigned char*)((const char*)user->getCurCharacR() + 0x75));
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
    user->m_winPointBuffer = 0;   // ORIG CUser.h:2282 具名字段
    m_joinCount = m_joinCount + 1;
    m_relayMgr.OnJoinRoom(seat);
    ((IMatch*)m_matching)->OnJoinRoom(user);
    return 0;
}

int PvP_Room::leave_room(CUser* user, bool& flag)
{
    // [b23S3C] P37/P52 日志形：ORIG ctor 前 6 值分两路物化 —— waiter/flag/mode
    // 落槽 -0x44/-0x40/-0x3c（具名局部），index/name/acc 经 callee-saved
    // edi/esi/ebx 跨 ctor 调用缓存（register 形）；varargs 装载序
    // ebx=acc,esi=name,edi=index,槽 mode/flag/waiter。
    // P16/P56：fail 分支直接 `return -1`（ORIG mov$-1;jmp epilogue，
    // 非槽写-1再共享尾读 —— 旧形多 1 条即 sheet gap=-1 来源）。
    int local_44 = get_waiter_count();
    int local_40 = flag;
    int local_3c = get_pvp_battle_mode();
    register int roomIndex = get_index();
    register const char* characName = user->getCurCharacName();
    register const char* accName = user->get_acc_name();
    {
        CSwitchLog local_34(__PRETTY_FUNCTION__, 0x6c2, 0, 0);   // ORIG 同文（P139）
        local_34("pvp@log %s,%s,room(%d),%d,%d,%d", accName, characName,
                 roomIndex, local_3c, local_40, local_44);
    }
    lock();
    if (m_seats[7] == user)
    {
        select_new_manager();
        flag = true;
    }
    int local_24 = get_user_seat(user);
    if (local_24 < 0)
    {
        return -1;
    }
    {
        if (get_state() == 2)
        {
            GiveOuterPenalty(user);
        }
        int local_20 = get_team(local_24);
        user->SetPvpIndex(-1);
        P_SEATSTATE(local_24) = 0;
        P_TEAMS(local_24) = 0xff;
        P_SEATS(local_24) = 0;
        m_joinCount = m_joinCount - 1;
        P_WIN(local_24) = 0;
        P_TEAMWIN(local_24) = 0;
        unlock();
        user->getHackAnalyzer()->reportHackInfo();
        user->getHackAnalyzer()->resetHackInfo();
        if (m_state == 2)
        {
            // [b23S3C] P9/P159：check_winner 头改 bool（ORIG 体尾 movw $0,%ax），
            // 调用点裸形即复现 ORIG test %al 直判。
            if (check_winner())
            {
                pvp_request_pvp_rank();
            }
        }
        m_relayMgr.OnLeaveRoom(local_24);
        ((IMatch*)m_matching)->OnLeaveRoom(user, this, local_20, local_24);
    }
    return local_24;
}

CUser* PvP_Room::destroy_room(int idx)
{
    // ORIG 0x085d81d0：get_index() 在 CSwitchLog 构造前求值并以 callee-saved
    // 跨构造调用缓存（ORIG +0x07..+0x12），日志首参为 __PRETTY_FUNCTION__
    // 局部静态符号（_ZZ…E19__PRETTY_FUNCTION__，P118/P139）。
    register int roomIdx = get_index();
    CUser* local_14;
    {
        CSwitchLog local_24(__PRETTY_FUNCTION__, 0x721, 0, 0);
        local_24("pvp@log room(%d), %d", roomIdx, idx);
    }
    local_14 = P_SEATS(idx);
    if (local_14 == 0)
    {
        for (int local_10 = 0; local_10 <= 7; local_10 = local_10 + 1)
        {
            if (P_SEATS(local_10) != 0)
            {
                local_14 = P_SEATS(local_10);
                break;
            }
        }
    }
    reset();
    return local_14;
}

int PvP_Room::Disconnect(CUser* user)
{
    // 残余为 N7 互换（虚调用 callee 寄存器跨版本分配，见 get_user_list 注释）。
    // ORIG 0x085dd754：for 预跳+setle 检查；命中座（seat==user）后
    // **无条件** return 1（ClearPvPRoom 仅在 !WalkOut 时执行，return 1 在
    // if 外——旧版把 return 1 放进 if 内属语义偏差，已修正）。
    for (int i = 0; i < 8; i = i + 1)
    {
        if ((P_SEATS(i) != 0) && (P_SEATS(i) == user))
        {
            ((IMatch*)m_matching)->OnWalkOut(this, i);
            if (!WalkOut(P_SEATS(i), i, ENUM_SEAT_STATE_0xfe,
                         (ENUM_WALKOUT_TYPE)3))
            {
                ClearPvPRoom(user);
            }
            return 1;
        }
    }
    return 0;
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

bool PvP_Room::WalkOut(CUser* user, int seat, ENUM_SEAT_STATE state,
                      ENUM_WALKOUT_TYPE walkoutType)
{
    // 残余为 N7 互换（虚调用 callee 寄存器跨版本分配，见 get_user_list 注释）。
    PacketGuard local_20;
    CUser* local_24 = 0;
    bool local_25 = false;
    bool local_26 = false;
    bool local_27 = false;
    int local_14 = get_team(seat);
    SendWalkOut(seat, walkoutType);
    int local_10 = set_seat_state(user, seat, state, &local_24, local_25, local_26, local_27);
    if ((get_waiter_count() == 0) && (local_27 == false))
    {
        LogManager::logFormat(1, "pvp.cpp",
                              __PRETTY_FUNCTION__,
                              0x1210, "PvP Room(%d) is empty!!!", get_index());
    }
    if (local_10 == 0)
    {
        if (local_24 != 0)
        {
            ((IMatch*)m_matching)->OnLeaveRoom(local_24, this, local_14, seat);
            G_GameWorld()->out_from_pvp(local_24);
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

bool PvP_Room::OnSetPVPSeatState(CUser* user, int seat, ENUM_SEAT_STATE state,
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
    if (check_authority(user, seat) == 0)
    {
        local_14 = 8;
        goto LAB_085dd357;
    }
    if (((IMatch*)m_matching)->CheckSeat((int)state) == 0)
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
    if ((get_waiter_count() == 0) && (local_27 == false))
    {
        LogManager::logFormat(1, "pvp.cpp",
                              __PRETTY_FUNCTION__,
                              0x11be, "PvP Room(%d) is empty!!!", get_index());
    }
    if (local_14 == 0)
    {
        if (local_24 != 0)
        {
            ((IMatch*)m_matching)->OnLeaveRoom(local_24, this, local_10, seat);
            G_GameWorld()->out_from_pvp(local_24);
            local_24->insert_pvp_masterid_walkingout_me((unsigned)user->getCurCharacNo());
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
        // ORIG(4.1) 实参先求值（后四入槽/前三入寄存器）→ ctor 后推参，保形直写
        int wc = get_waiter_count();                    // -0x44
        int st = (int)state;                            // -0x40
        int mode = get_pvp_battle_mode();               // -0x3c
        int idx = get_index();                          // ORIG %edi（指针类右值直传不入槽）
        CSwitchLog local_34(__PRETTY_FUNCTION__, 0x594, 0, 0);   // ORIG 同文（P139）
        local_34("pvp@log %s,%s,room(%d),%d,%d,%d,%d", user->get_acc_name(),
                 user->getCurCharacName(), idx, mode, seat, st, wc);
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
            P_SEATS(seat)->SetPvpIndex(-1);
            local_24 = get_team(seat);
            lock();
            P_SEATSTATE(seat) = 0;
            P_TEAMS(seat) = 0xff;
            P_SEATS(seat) = 0;
            m_joinCount = m_joinCount - 1;
            P_WIN(seat) = 0;
            unlock();
            m_relayMgr.OnLeaveRoom(seat);
            ((IMatch*)m_matching)->OnLeaveRoom(*out, this, local_24, seat);
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
                P_SEATS(seat)->SetPvpIndex(-1);
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
            P_SEATS(seat)->SetPvpIndex(-1);
            *out = P_SEATS(seat);
            int local_20 = get_team(seat);
            P_SEATSTATE(seat) = 0;
            P_TEAMS(seat) = 0xff;
            P_SEATS(seat) = 0;
            m_joinCount = m_joinCount - 1;
            m_relayMgr.OnLeaveRoom(seat);
            ((IMatch*)m_matching)->OnLeaveRoom(*out, this, local_20, seat);
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
    if (((IMatch*)m_matching)->OnReady(user, ready) != 1)
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
            if (!check_start_state())
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
    // P75/N7 封顶（ORIG 0x085dbcd4）：ORIG(4.1) 对常量/形参实参点先载 receiver
    // (-0xc→%eax) 再 store 常量槽，计算值实参点先物化值(%edx) 后载 receiver；
    // 4.4 恒先 store 实参后载 receiver 且 scratch 用 %eax 顶替 %edx。指令集合/
    // 语义全等，装载时刻与寄存器角色为跨版本调度固有差（P75 判例），勿再耗轮次。
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
    local_ec->put_short((int)m_requestedMapNo);
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
    local_ec->put_byte(m_isExistPassword);
    local_ec->put_int(((IMatch*)m_matching)->GetIndentification());
}

void PvP_Room::make_seat_info(char* buf, int seat)   // [b28c-16 SIGFIX] ORIG 串 void
{
    // ORIG 0x085db9e2（199；本侧 193，-6 全为 N 类形态差，multiset 核对无语义
    // 缺口）：ORIG 将 vptr 读值/getCurCharacName/get_acc_name 结果以 %edi/%esi/
    // %ebx callee-saved 跨 3 次调用缓存（push 对 +2、`mov (%eax),%edx` 直落
    // outgoing 槽 vs 4.4 `mov…,%eax; mov %eax,…` 两段，N7/N18a），错误码/日志
    // 串/PRETTY 符号逐条一致。
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
            LogManager::logFormat(1, "pvp.cpp", __PRETTY_FUNCTION__,
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
                cMyTrace local_28(__PRETTY_FUNCTION__, 0xe74, 0);
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
    local_f4->put_short((int)m_requestedMapNo);
    local_f4->put_byte(m_pvpBattleMode);
    local_f4->put_int(((IMatch*)m_matching)->GetIndentification());
    local_f4->finalize(true);
}

void PvP_Room::get_room_info(PVP_ROOM_INFO* out)
{
    out->title = m_roomName;
    out->byte4 = m_mode8;
    out->short6 = m_requestedMapNo;
    out->byte8 = m_isExistPassword;
    out->passwd = m_password;
}

void PvP_Room::get_user_list(void* out)
{
    // N7 封顶：ORIG 虚表槽函数指针落 %ecx、receiver 链 %eax；4.4 恒 eax/edx 形，
    // 临时/引用/限定名均无效（负例表 N7）。
    ((IMatch*)m_matching)->GetUserList(out);
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
    G_CGameManager()->PutPvp(this);
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
    for (int local_8 = 0; local_8 <= 7; local_8 = local_8 + 1)
    {
        if (P_SEATS(local_8) != 0)
        {
            if (P_SEATS(local_8) != m_seats[7])
            {
                m_seats[7] = P_SEATS(local_8);
                m_managerSeat = local_8;
                P_SEATSTATE(local_8) = 0;
                return 1;
            }
        }
    }
    return 0;
}

int PvP_Room::change_manager_to_winner()
{
    // ORIG 0x085d7d54（81；本侧 83，+2=N 落点：ORIG seat==0 skip 的 je 与
    // is_last_win!=true skip 的 jne 分落 +0x15c/+0x15d 两独立 landing（其间
    // 1 条 nop/重叠指令布局产物），4.4 合并共享出口+中继 jmp。bVar 常量岛/
    // !=1→xor 物化/嵌套实参消槽三形已应用，141→83。）
    // [batch-19 N 封顶 ndiff=1] 再实测三形均不优于双 continue：
    // ① 合并 `if (seat!=0 && is_last_win(i)==true)` → 84 insn（丢 `xor$1`
    //   物化且中继仍在）；② 纯嵌套 `if(seat){if(==true){body}}` → 79 insn
    //   （极性翻 test/je，丢 ORIG `xor$1;jne`）；③ `!is_last_win(i)` 替
    //   `!= true` → 83 insn 逐条同（xor$1 保留），无增益。
    if (m_seats[7] == 0)
    {
        return 0;
    }
    if (G_GameWorld()->GetChannelType() == 0xf)
    {
        return 1;
    }
    if (G_GameWorld()->GetChannelType() == 7 ||
        G_GameWorld()->IsSchoolPvPChannel() != 0)
    {
        return 1;
    }
    if (is_last_win(m_managerSeat))
    {
        return 1;
    }
    for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
    {
        if (P_SEATS(local_10) == 0)
            continue;
        if (is_last_win(local_10) != true)
            continue;
        m_seats[7] = P_SEATS(local_10);
        m_managerSeat = local_10;
        return 1;
    }
    return 0;
}

void PvP_Room::GiveOuterPenalty(CUser* user)   // [b28c-16 SIGFIX] ORIG 串 void
{
    PvpResultType local_b4;
    local_b4.Clear();
    user->update_pvp_rank(local_b4);
    int local_10 = GetTeamIndex(user);
    if (m_userTable.IsValidTeam(local_10) != 1)
    {
        cMyTrace local_20(__PRETTY_FUNCTION__, 0x10f8, 5);
        local_20("Invalid Team(%d)", local_10);
        goto LAB_085dcd66;
    }
    if (G_GameWorld()->IsIntegratedPvPBaseChannel() != 0)
    {
        void* ev = GlobalData::s_event_manager->GetRepeatEvent(0x38);
        if ((*(int (**)(void*, int))(*(char**)ev + 0x34))(ev, 0) != 0)
        {
            if ((int)(long)user->GetServerGroup() != 0)
            {
                Packet_Server_Match_data local_2d;
                local_2d.m_serverGroup = (char)(long)user->GetServerGroup();
                local_2d.m_lose = 1;
                CStatisticServerProxy* proxy =
                    GlobalData::s_statistic_proxy_mgr->GetServerProxy(
                        (ENUM_SERVER_GROUP)(long)user->GetServerGroup());
                proxy->SendPacket((char*)&local_2d, 0xd);
            }
        }
    }
    if (G_GameWorld()->IsPVPChannel() != 0)
    {
        if ((G_GameWorld()->GetChannelType() == 4) ||
            (G_GameWorld()->GetChannelType() == 8))
        {
            user->update_pvp_point(m_userTable.GetLeavePenalty(local_10));
        }
        else if (G_GameWorld()->GetChannelType() == 6)
        {
            user->add_guild_pvp_result(-1);
            goto LAB_085dcd66;
        }
    }
    if (G_GameWorld()->IsWinPointPvPChannel() != 0)
    {
        user->add_pvp_result(false, 0);
    }
    ((IMatch*)m_matching)->OnGiveup(user);
LAB_085dcd66:
    ;
}

bool PvP_Room::VoteLackUser(CUser* user, int vote)   // [b28c-16 SIGFIX] ORIG 串 bool
{
    if ((vote < 0) || (7 < vote))
    {
        LogManager::logFormat(1, "pvp.cpp", __PRETTY_FUNCTION__,
                              0x116d, "VoteSlotNo(%d) < 0 || MAX_PVP_PLAYER < VoteSlotNo", vote);
        return false;
    }
    CUser* u = P_SEATS(vote);
    // ORIG 085dccbe: mov 0xc(%ebp),%eax（user 先载 reg）/ cmp -0x10(%ebp),%eax
    // —— 操作数序 user==u；u==user 反产 slot 先载。
    if ((user == u) || (u == 0))
    {
        return false;
    }
    int iVar2 = get_user_seat(user);
    if ((iVar2 < 0) || (8 < iVar2))
    {
        LogManager::logFormat(1, "pvp.cpp", __PRETTY_FUNCTION__,
                              0x117b, "UserSlotNo(%d) < 0 || MAX_PVP_PLAYER < UserSlotNo", iVar2);
        return false;
    }
    m_election.Vote(iVar2, vote);
    // P24/P52：ORIG 链式内联实参（0x085dd01a），key 先物化 ebx、m_index 缓存 esi，
    // G_TimerQueue() 置后，跨调用 callee-saved 存活
    // ORIG 085dd00e/085dd014：key→%ebx、index→%esi（4.4 反产 = N7 互换，
    // asm("ebx") 实测反产复制链更差）⇒ 保持直写。
    G_TimerQueue()->InsertTimerInMilisecond((TimerEntry::OBJ_TYPE)1, m_index,
                                            (TIMER_MESSAGE)0x31, 0x9c4LL,
                                            (int)gen_timer_key((TIMER_MESSAGE)0x31), 0);
    return true;
}

int PvP_Room::OnConnectP2PPvPTimeout()
{
    // 残余 2 行 N7 互换（虚调用 callee 寄存器跨版本分配，见 get_user_list 注释）。
    if (!check_start_state())
    {
        ((IMatch*)m_matching)->OnConnectP2PTimeout(this);
    }
}

bool PvP_Room::OnTimerWalkOutLackUser()
{
    // ORIG 0x085dce28：Ghidra 双条件 `-1 < local_10 && -1 < local_10`——尾 cond
    // 用 not/shr$0x1f 物化（loop tail），body 头重复 `cmpl$0; js→nop 岛`
    // （P184 死块照抄）。双 guard 形（while(x>-1) + 体内 if(x<0) break）使
    // 4.4 复现全部 69 条含 not/shr 岛（合并 && 形被 bool 汇聚，P133）；
    // 残余 = OnWalkOut 虚 call callee-saved 互换 10 行（N7，同 get_user_list 判例），
    // NEAR 封顶。
    int local_10 = m_election.GetElected();
    while (local_10 > -1)
    {
        if (local_10 < 0)
            break;
        m_election.CancelCandidate(local_10);
        m_election.CancelVoter(local_10);
        ((IMatch*)m_matching)->OnWalkOut(this, local_10);
        if (!OnSetPVPSeatState(P_SEATS(local_10), local_10, ENUM_SEAT_STATE_0xfe,
                               (ENUM_WALKOUT_TYPE)3))
        {
            return 0;
        }
        local_10 = m_election.GetElected();
    }
    return 1;
}

int PvP_Room::HandleTimerUserRevive(int seat)
{
    // ORIG 085dd924/085dd927/085dd92a/085dd92d：四 skip 条件各落独立 nop 岛
    // （js/jg/je/jne→nop×4，末岛 fall merge）；P_WIN 非零跳形 = if(P_WIN) 直写。
    if (seat < 0)
        goto HTR_OUT_RANGE1;
    if (seat > 7)
        goto HTR_OUT_RANGE2;
    if (P_SEATS(seat) == 0)
        goto HTR_OUT_SEAT;
    if (P_WIN(seat))
        goto HTR_OUT_WIN;
    P_WIN(seat) = 1;
    {
        PacketGuard local_18;
        local_18.put_header(0, 0x78);
        local_18.put_byte(seat);
        local_18.put_byte(rand() % 0x100);
        local_18.finalize(true);
        send_to_pvp(local_18);
    }
HTR_OUT_WIN:
    ;
HTR_OUT_SEAT:
    ;
HTR_OUT_RANGE2:
    ;
HTR_OUT_RANGE1:
    ;
}

int PvP_Room::SkipLastRankResultTimer()
{
    // P31：ORIG 帧 0x18 vs gcc4.4 0x8，补 4 个未用局部对齐帧深（ORIG 0x085d9ba4）
    int d1;
    int d2;
    int d3;
    int d4;
    gen_timer_key((TIMER_MESSAGE)0x32);
}

int PvP_Room::CancelAllTimer()
{
    // P31：ORIG 帧 0x18 vs gcc4.4 0x8，补 4 个未用局部对齐帧深（ORIG 0x085d6c42）
    int d1;
    int d2;
    int d3;
    int d4;
    gen_timer_key((TIMER_MESSAGE)0x32);
}

int PvP_Room::end_pvp()
{
    // N7 封顶（虚调用 callee 寄存器互换，见 get_user_list 注释）。
    ((IMatch*)m_matching)->OnEndFight(this);
}

void PvP_Room::end_pvp_result()
{
    change_manager_to_winner();
    m_state = 1;
    for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
    {
        if (P_SEATS(local_10) == 0)
        {
            continue;
        }
        P_SEATS(local_10)->set_state((ch_state)6);
    }
    SkipLastRankResultTimer();
}

int PvP_Room::send_pvp_end()
{
    m_isEndPvpBattle = 1;
    if (G_GameWorld()->GetChannelType() != 6)
    {
        update_pvp_point();
    }
    pvp_reward();
    return 1;
}

int PvP_Room::pvp_request_pvp_rank()
{
    if (m_rankRequested == 0)
    {
        m_rankRequested = 1;
        PacketGuard local_18;
        local_18.put_header(0, 0x31);
        local_18.finalize(true);
        send_to_pvp(local_18);
        m_fieldC0 = 1;
    }
}

bool PvP_Room::request_fight(CUser* user)
{
    if ((unsigned char)m_battleEndFlag != 0)
    {
        return 1;
    }
    int iVar2 = get_user_seat(user);
    if (iVar2 < 0)
    {
        return 0;
    }
    m_relayMgr.OnRequestFight(iVar2);
    return 1;
}

void PvP_Room::revengeMission(bool flag)
{
    CMissionList_Charac* mission;
    for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
    {
        if (P_SEATS(local_10) != 0)
        {
            mission = (CMissionList_Charac*)P_SEATS(local_10)->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
            // ORIG 085ddf7f: mov 0x8(%ebp),%eax / mov 0x4(%eax),%edx —— 成员值单步
            // 直落 %edx（技巧2 register 直落型），再统一 spill 到 0x8(%esp)。
            // 4.4 对「成员→%eax 后再挪 %edx 槽」恒物化两步（N18a 族：4.1 可单步
            // 直落任意硬寄存器，4.4 fold 仅到首寄存器），register asm 反产更差形。
            // [batch-19 复核] `static_cast`↔C 式 `(PVP_BATTLE_MODE)` 两种写法实测
            // 输出全等（45 insn），中转步与 cast 形式无关 ⇒ N18a 封顶，ndiff=3。
            mission->Update_Revenge_event(*P_SEATS(local_10),
                                          static_cast<PVP_BATTLE_MODE>(m_pvpBattleMode),
                                          flag);
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
    // ORIG 085dd9xx：bool 型 cVar1（call 结果 al 直存 byte 槽、无归一）；
    // !cVar1 判定走 movzbl+xor $1 的 4.1 取反形。
    bool cVar1 = IsDeathMatchMap(mapNo, flag);   // bool 形：后两处 !cVar1 的 movzbl+xor 物化与 ORIG 同；
    // 差仅入口 mov%al 直存 vs 4.4 test;setne 归一（N bool ABI），净差 -2 优于 uchar 形 +4，
    // char 形实测 ndiff 12（!cVar1 折成 cmpb/jne，丢 movzbl+xor+je 四段×2）
    if ((cVar1 != 0) && (!flag) && (m_pvpBattleMode == 4))
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
    // ORIG 085dd9ef/085dda0x：!char-slot 形 = movzbl -0x9 + xor $1 + test（xor
    // 值化，非 cmp 归一）；(cVar1^1) 写法被 4.4 fold-const 折成 cmpb $1。
    if ((!cVar1) && (m_pvpBattleMode == 4))
    {
        set_pvp_mode(user, 1);
    }
    if ((!cVar1) && (m_pvpBattleMode == 5))
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
    // ORIG 085d84ee: arg 先载（%edx）再 this（%eax）写回。N76 异型化翻案
    // （NL7/NL8 新定律）：m_pvpBattleMode int→unsigned int（private，正数
    // == 比较，无符号性语义差），4.4 异型 store 出实参先载形，ndiff 2→0。
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
        int iVar3 = G_CDataManager()->m_mapList->GetPVPMapCount();
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
    // 残余 4 行 N7 互换：gen_timer_key 结果与 m_index 的 callee-saved 缓存
    // ORIG 用 ebx/esi、4.4 反序（同 VoteLackUser 判例），封顶。
    if (m_isInsertTimerRecvPvpRank != 0)
    {
        return 1;
    }
    G_TimerQueue()->InsertTimerInMilisecond(
        (TimerEntry::OBJ_TYPE)1, m_index, (TIMER_MESSAGE)0x3c, 10000LL,
        (int)gen_timer_key((TIMER_MESSAGE)0x3c), 0);
    m_isInsertTimerRecvPvpRank = 1;
    return 1;
}

int PvP_Room::insert_timer_recv_pvp_end_result()
{
    // P24 内联实参形（ORIG：key/this 结果以 callee-saved 跨后续调用直落
    // outgoing 槽，0x085dc253/0x085dc27a/0x085dc28c）。
    // P24 内联实参形（ORIG 0x085dc253/27a/28c：key/ms 结果 callee-saved 直落
    // outgoing 槽）。残余 1 条：ORIG 把 m_index 经 -0x1c 槽 store/reload（4.1
    // 具名局部 spill），4.4 表达式直传不可复现（N18a 族）。
    // [b28 BM14 fresh 复核] 完整残差 = N82附互换族：4.1 将 gen_timer_key 结果放
    // %edi、m_index(vptr) spill 至 -0x1c 槽；4.4 恰好反配（key→槽、vptr→%edi），
    // 叠加虚 call 目标 eax↔edx（N7）与 0x18/0x8 装载序。寄存器/槽分配对调无源形
    // 解（同 VoteLackUser 判例），语义全等，N 封顶。
    G_TimerQueue()->InsertTimerInMilisecond(
        (TimerEntry::OBJ_TYPE)1, m_index, (TIMER_MESSAGE)0x3d,
        ((IMatch*)m_matching)->GetEndPvPResultTimeout(),
        (int)gen_timer_key((TIMER_MESSAGE)0x3d), 0);
    return 1;
}

int PvP_Room::check_recv_pvp_rank()
{
    // ORIG 0x085dc3ca：bool 槽 -0xd（movzbl 返回）；user 直落 %ebx 跨调用；
    // IsRecv(char) 的 ! 以 xor $1/test/je 短路，不物化 0/1。
    unsigned char changed = 0;
    for (int i = 0; i < 8; i = i + 1)
    {
        // ORIG 085dc3ca：4.1 双空 skip 块岛 085dc455(nop;jmp end)/085dc458(nop;fall end)。
        // goto 落点保留独立块（4.4 直写 continue 会被 threading 折叠）。
        if (P_SEATS(i) == 0)
            goto SKIP_RANK_SEAT;
        if (P_TEAMS(i) == 0xff)
            goto SKIP_RANK_TEAM;
        if (!m_recvFlagB4.IsRecv(i))
        {
            m_fieldC0 = 0;
            register CUser* user = P_SEATS(i);
            G_CGameManager()->CheckOutPvp(user, true);
            changed = 1;
        }
SKIP_RANK_TEAM:
        ;
SKIP_RANK_SEAT:
        ;
    }
    return changed;
}

bool PvP_Room::IsThereSameGuildMember()
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
                if (local_18 == local_14)
                    return 1;
                local_18 = local_14;
            }
        }
    }
    return 0;
}

int PvP_Room::get_extra_win_point(int team)
{
    // N7 封顶（虚调用 callee 寄存器互换，见 get_user_list 注释）。
    return ((IMatch*)m_matching)->GetExtraWinPoint(team);
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
            P_SEATS(local_14)->getHackAnalyzer()->reportHackInfo();
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
    m_battleEndFlag = 0;
    m_rankRequested = 0;
    m_isInsertTimerRecvPvpRank = 0;
    m_isEndPvpBattle = 0;
    for (int local_10 = 0x31; local_10 < 0x3f; local_10 = local_10 + 1)
    {
        // ORIG 085d657d: cmpl $0x3e / je 085d6597(空块 nop) / call / jmp 085d6598
        // 4.1 保留的空 skip 块落点在 call 之后、++ 之前，用 goto 钉住块序。
        if (local_10 == 0x3e)
            goto SKIP_GEN_TIMER_KEY;
        gen_timer_key((TIMER_MESSAGE)local_10);
SKIP_GEN_TIMER_KEY:
        ;
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
    m_battleEndFlag = 0;
    m_rankRequested = 0;
    m_isInsertTimerRecvPvpRank = 0;
    m_isEndPvpBattle = 0;
    unlock();
}

int PvP_Room::calculate_pvp_point()
{
    m_userTable.Clear();
    for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
    {
        // ORIG 085d9090：call 后 test %al;jne（无 xor/归一）；if/else 两支尾
        // + observer 空 then 共三条 jmp merge（@41/@56/@57）。
        if (P_SEATS(local_10) == 0)
            goto CP_NEXT;
        if (IsPvpObserver(local_10))
        {
            goto CP_NEXT;
        }
        if ((m_pvpBattleMode == 1) || (m_pvpBattleMode == 4))
        {
            m_userTable.Set(local_10, P_SEATS(local_10));
            goto CP_NEXT;
        }
        else
        {
            m_userTable.Set(P_TEAMWIN(local_10), P_SEATS(local_10));
            goto CP_NEXT;
        }
CP_NEXT:
        ;
    }
    m_userTable.Calculate();
}

int PvP_Room::update_pvp_point()
{
    if (G_GameWorld()->IsPVPChannel() != 0)
    {
        if (G_GameWorld()->IsFreePvPChannel() != 0)
        {
            for (int local_18 = 0; local_18 < 8; local_18 = local_18 + 1)
            {
                if (P_SEATS(local_18) == 0)
                    continue;
                if (IsPvpObserver(local_18) != false)
                    continue;
                P_SEATS(local_18)->update_old_pvp_point();
            }
            goto LAB_085dca42;
        }
    }
    for (int local_14 = 0; local_14 < 8; local_14 = local_14 + 1)
    {
        if (P_SEATS(local_14) == 0)
            continue;
        if (IsPvpObserver(local_14) != false)
            continue;
        {
            P_SEATS(local_14)->update_old_pvp_point();
            if ((m_pvpBattleMode == 1) || (m_pvpBattleMode == 4))
            {
                P_SEATS(local_14)->update_pvp_point(
                    m_userTable.GetResult(local_14, false,
                                          P_SEATS(local_14)->get_pvp_grade()));
            }
            else
            {
                bool mid = false;                       // ORIG -0x9 byte 槽
                if (P_SEATS(local_14)->get_pvp_grade() > 10)
                    mid = true;
                P_SEATS(local_14)->update_pvp_point(
                    m_userTable.GetResult(P_TEAMWIN(local_14), mid,
                                          P_SEATS(local_14)->get_pvp_grade()));
            }
        }
    }
LAB_085dca42:
    ;
}

void PvP_Room::start_pvp()   // [b28c-16 SIGFIX] ORIG 串 void
{
    if (get_waiter_count() < 2)
    {
        goto LAB_085d91da;
    }
    {
        ((IMatch*)m_matching)->RequestMap(m_requestedMapNo,
                                          reinterpret_cast<PVP_BATTLE_MODE&>(m_pvpBattleMode));
        if ((m_requestedMapNo < 0) || (m_requestedMapNo > G_CDataManager()->m_mapList->GetPVPMapCount()))
        {
            cMyTrace local_48(__PRETTY_FUNCTION__, 0x880, 0);
            local_48("START_PVP request_map_index(%d)", (int)m_requestedMapNo);
            m_requestedMapNo = 0;
        }
        if (m_requestedMapNo == 0)
        {
            m_playMapNo = (short)SelectRandomMap();
            if ((m_playMapNo < 0) ||
                (m_playMapNo >= G_CDataManager()->m_mapList->GetPVPMapCount()))
            {
                cMyTrace local_38(__PRETTY_FUNCTION__, 0x88b, 0);
                local_38("START_PVP select_map_index(%d)", (int)m_playMapNo);
            }
        }
        else
        {
            m_playMapNo = m_requestedMapNo;
        }
        if (G_GameWorld()->GetChannelType() == 6)
        {
            m_guildWarLog.SetpvpStartTime(GlobalData::s_systemTime_.getCurSec());
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
                local_54.put_byte((int)m_playMapNo);
                local_54.put_byte(m_pvpBattleMode);
                local_54.finalize(true);
                P_SEATS(local_10)->Send(local_54);
                P_SEATS(local_10)->set_state((ch_state)7);
                P_SEATS(local_10)->getHackAnalyzer()->beginCollectHackInfo(P_SEATS(local_10));
                disconnect_detecter::GetInstanceDisconnectDetecter()->RegisterUser(
                    P_SEATS(local_10));
            }
        }
        Statistics::PVPPlay2((int)m_playMapNo, m_pvpBattleMode, local_18);
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
        m_battleStartSec = GlobalData::s_systemTime_.getCurSec();
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
        GetInstanceCommonStatisticsMgr()->IncreaseQuantity((CCommonStatisticsMgr::ENUM_STATISTIC_CONTAINER)1, local_14);
        if ((m_playMapNo >= 0) &&
            (m_playMapNo < G_CDataManager()->m_mapList->GetPVPMapCount()))
        {
            GetInstanceCommonStatisticsMgr()->IncreaseQuantity((CCommonStatisticsMgr::ENUM_STATISTIC_CONTAINER)2, (int)m_playMapNo);
        }
        else
        {
            cMyTrace local_28(__PRETTY_FUNCTION__, 0x942, 0);
            local_28("START_PVP filter select_map_index(%d)", (int)m_playMapNo);
        }
        ((IMatch*)m_matching)->OnStartFight();
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
        CSwitchLog local_40(__PRETTY_FUNCTION__, 0x9a1, 0, 0);   // ORIG 同文（P139）
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
        MissionClearCondition_Parameter local_5b((PVP_BATTLE_MODE)m_pvpBattleMode);
        ((CMissionList_Charac*)local_28)->Update_Kill_event(*killer, local_5b);
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
    // ORIG 0x085d9b0e（365 条；本侧 373，+8 全为 N 类形态差，台账豁免）：
    //  - local_4c/local_3d 的 ==1/!=0 检查 4.1 折成 `xor$1,%eax;test%al;je`
    //    常量岛（4.4 恒 `cmpb/cmpl` 直判，setcc-fold 负例在册）；
    //  - PVP_*_BATTLE_TIME[local_4c] 与 local_84[teamwin+5] 索引 4.1 单段
    //    `mov &arr(,%eax,4)`/`lea …,%edx` 直落，4.4 拆 lea+reload（N76/N18a）；
    //  - end_time 日志 3 实参 4.1 以 callee-saved 缓存（push 对），4.4 逐槽
    //    reload（+4 mov）。lock()/unlock() 调用点、模式分派、do-while 比较
    //    （jl/jle 方向）已逐段核对无语义缺口。
    if (m_battleEndFlag != 0)
    {
        return 0;
    }
    m_battleEndFlag = 1;
    int local_4c = 0;
    if (G_GameWorld()->GetChannelType() == 7)
    {
        local_4c = 1;
    }
    int local_48 = GlobalData::s_systemTime_.getCurSec() - m_battleStartSec;
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
        cMyTrace local_5c(__PRETTY_FUNCTION__, 0xa9a, 0);
        local_5c("PVP_TIMEOUT_CHECK end_time(%d), play_time(%d)", local_44, local_48);
    }
    if (m_state != 2)
    {
        return 0x13;
    }
    int local_3d = 1;
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
        if (!local_3d)
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
        else
        {
            for (int local_24 = 0; local_24 < 8; local_24 = local_24 + 1)
            {
                P_WIN(local_24) = 0;
            }
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

// §6.2 巨件核销：P167 153/153 [0缺0多]；gap 1045→-55；残差归因 N82（ORIG 帧
// 0xfc/36 槽 vs 4.4 帧 0x140/55 槽全帧平移，edi/esi/ebx 与 esi/ebx 寄存器分配
// 差）+ N88 装载序 + 少量 4.4 布尔物化（jcc+5）；逐块人工比对语义与 Ghidra C
// 一致（详见 docs/asm_translation_patterns.md 负例表）⇒ N 封顶。
void PvP_Room::pvp_reward()   // [b28c-16 SIGFIX] ORIG 串 void
{
    int local_dc[8];
    int local_70 = GetRanking(local_dc);
    int local_6c = GetAce();
    if (G_GameWorld()->IsPVPChannel() != 0)
    {
        if (G_GameWorld()->IsFreePvPChannel() != 1)
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
    ((IMatch*)m_matching)->OnReward(this, get_team((int)local_65), local_66 == 1);
    int local_5c = 0;
    while (true)
    {
        if (7 < local_5c)
        {
            if (G_GameWorld()->GetChannelType() == 6)
            {
                m_guildWarLog.WriteGuildWarPvPLog(m_index, (char)m_pvpBattleMode, m_users);
            }
            ((IMatch*)m_matching)->OnEndPvP(this);
            break;
        }
        CUser* local_58 = P_SEATS(local_5c);
        if (local_58 != 0)
        {
            disconnect_detecter::GetInstanceDisconnectDetecter()->UnregisterUser(
                P_SEATS(local_5c));
            local_58->add_pvp_play_info(
                GlobalData::s_systemTime_.getCurSec() - m_battleStartSec, 1);
            bool bVar15;
            if (G_GameWorld()->IsIntegratedPvPBaseChannel() != 0)
            {
                CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(0x38);
                if (ev->IsEventing((CUser*)0) != 0 &&
                    IsPvpObserver(local_5c) == 0)
                {
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
            // ORIG 0x085daa76..（P167 缺 getCurCharacR 同族）：bVar15 由 integrated
            // 频道 eventing 判定独立算出；发包块整体包在 `bVar15 && GetServerGroup()!=0`
            // 条件内（条件臂 + 块内 m_serverGroup + GetServerProxy = 3 次独立调用，P0/P48）。
            if (bVar15 && local_58->GetServerGroup() != 0)
            {
                Packet_Server_Match_data local_a1;
                local_a1.m_serverGroup = (char)(long)local_58->GetServerGroup();
                if (P_WIN(local_5c) == 0)
                {
                    local_a1.m_lose = 1;
                }
                else
                {
                    local_a1.m_win = 1;
                }
                CStatisticServerProxy* proxy =
                    GlobalData::s_statistic_proxy_mgr->GetServerProxy(
                        (ENUM_SERVER_GROUP)(long)local_58->GetServerGroup());
                proxy->SendPacket((char*)&local_a1, 0xd);
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
                local_58->add_guild_point_item();
            }
            CMissionList_Charac* local_48 =
                (CMissionList_Charac*)local_58->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
            local_48->setIssue(P_WIN(local_5c) != 0, m_pvpBattleMode == 2);
            MissionClearCondition_Parameter local_b0((PVP_BATTLE_MODE)m_pvpBattleMode);
            short local_af = (short)local_48->getIssue_mode(m_pvpBattleMode == 2);
            bool local_b1 = false;
            local_48->Update_Win_event(*local_58, local_b0, local_b1);
            if (local_b1 != false)
            {
                const PvpResultType* r = local_58->getPVPResultRefR();
                int local_44 =
                    GlobalData::s_ref_pvp_grade->GetPvpGrade(r->m_expPoint);
                r = local_58->getPVPResultRefR();
                if (r->m_pvpGrade < local_44)
                {
                    PvpResultType* w = local_58->getPVPResultRefW();
                    w->m_pvpGrade = local_44;
                }
            }
            local_58->send_MissionList();
            if (G_GameWorld()->IsPVPChannel() != 0)
            {
                if (G_GameWorld()->IsWinPointPvPChannel() != 0)
                {
                    local_58->add_pvp_result(P_WIN(local_5c) != 0, &local_84);
                    if ((m_pvpBattleMode == 4) || (m_pvpBattleMode == 5))
                    {
                        local_84 = 0;
                    }
                    if (P_WIN(local_5c) != 0)
                    {
                        if ((m_pvpBattleMode == 1) || (m_pvpBattleMode == 4))
                        {
                            APSystem::CUserProc::ClearActionAndSendtoUser(
                                local_58, (APSystem::_ActionGroupIndex)0x12, 0,
                                (ENUM_PACKETCLASS)0);
                        }
                        else if ((m_pvpBattleMode == 2) || (m_pvpBattleMode == 5))
                        {
                            APSystem::CUserProc::ClearActionAndSendtoUser(
                                local_58, (APSystem::_ActionGroupIndex)0x13, 0,
                                (ENUM_PACKETCLASS)0);
                        }
                        else if (m_pvpBattleMode == 3)
                        {
                            APSystem::CUserProc::ClearActionAndSendtoUser(
                                local_58, (APSystem::_ActionGroupIndex)0x14, 0,
                                (ENUM_PACKETCLASS)0);
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
                    local_58->add_guild_pvp_result(local_40);
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
                local_94.put_int(((fair_pvp::CFairPvPScore*)fair)->GetLastTrainingSealCount());
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
                // ORIG 0x085dc1ce..（P0/P48 去缓存）：w->m_expPoint 与 LogHistory 的
                // 第 3 参各自独立调用 get_pvp_result_oldexp_point()，不缓存到具名局部
                // （缓存则少 1 call 点，P167 差集 -1 根因）。
                w->m_expPoint = local_58->get_pvp_result_oldexp_point() + local_38;
                local_94.put_int(local_38);
                const PvpResultType* r = local_58->getPVPResultRefR();
                local_58->LogHistory("PvPExp,%d,%d,%d", r->m_expPoint,
                                     local_58->get_pvp_result_oldexp_point(),
                                     local_38);
            }
            int sVar4 = local_58->has_within_Mission();
            if (sVar4 == 0)
            {
                local_58->acceptable_within_mission();
            }
            local_94.put_byte(local_70);
            for (int local_34 = 0; local_34 < local_70; local_34 = local_34 + 1)
            {
                int local_30 = local_dc[local_34];
                if (P_SEATS(local_30) != 0)
                {
                    local_94.put_short((int)P_SEATS(local_30)->get_unique_id() & 0xffff);
                    local_94.put_int(m_field78[local_30]);
                    local_94.put_byte(P_SEATS(local_30)->get_pvp_grade());
                    local_94.put_int(P_SEATS(local_30)->get_pvp_result_exp_point());
                    local_94.put_int(GlobalData::s_ref_pvp_grade->GetPvpCurrRankPoint(
                        P_SEATS(local_30)->get_pvp_grade()));
                    local_94.put_int(GlobalData::s_ref_pvp_grade->GetPvpNextRankPoint(
                        P_SEATS(local_30)->get_pvp_grade()));
                    int local_2c = m_deathMatch.getKillCount(local_30);
                    int local_28 = m_deathMatch.getDeathCount(local_30);
                    local_94.put_int(local_2c);
                    local_94.put_int(local_28);
                    if ((m_pvpBattleMode == 4) || (m_pvpBattleMode == 5))
                    {
                        cMyTrace local_80(__PRETTY_FUNCTION__, 0xdab, 0);
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
                local_94.put_short(((fair_pvp::CFairPvPScore*)fair)->GetLastTrainingSealBonusCount());
                fair = local_58->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)7);
                local_94.put_short(((fair_pvp::CFairPvPScore*)fair)->GetRemainBonusCount());
            }
            else if (G_GameWorld()->GetChannelType() == 0x10)
            {
                local_94.put_short(0);
                local_94.put_short(0);
            }
            sVar4 = local_58->has_within_Mission();
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
                if (local_58->gain_exp_sp((int)local_84, local_b8, local_bc,
                                          (CUser::eExpAddReason)10, 0, 0) != 0)
                {
                    local_58->SendNotiPacket((CUser::eSendTarget)2, (ENUM_NOTIPACKET)2, 1);
                }
            }
            long long lVar13 = GlobalData::s_systemTime_.getCurSec();
            local_58->getHackAnalyzer()->setLastMonsterDeadTime(lVar13);
            CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(0x37);
            if (ev->IsEventing((CUser*)0) != 0)
            {
                CPvPLiveEventData* live = (CPvPLiveEventData*)local_58->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)3);
                live->IncreasePlayCount(local_58);
            }
            const PvpResultType* r2 = local_58->getPVPResultRefR();
            int local_24 =
                GlobalData::s_ref_pvp_grade->GetPvpGrade(r2->m_expPoint);
            if ((local_24 < 1) ||
                (local_58->getPVPResultRefR()->m_pvpGrade == local_24))
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
                w2->m_pvpGrade = local_24;
                CMissionList_Charac* mission = (CMissionList_Charac*)local_58->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
                mission->MakeMissionList_forOldUser(*local_58);
                G_GameWorld()->send_AllBasicInfo(local_58);
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
        local_1c = rand() % G_CDataManager()->m_mapList->GetPVPMapCount() + 1;
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
    // ORIG 0x085dc654：m_field98[seat] 为主判据、m_field78[seat] 为平局决胜
    // （mov 0x8(%eax,(seat+0x24)*4) = this+seat*4+0x98；+0x1c → +0x78），
    // 房间内嵌数组，非 CUser 对象字段。
    int local_10 = -1;
    int local_c = -1;
    for (int local_8 = 0; local_8 < 8; local_8 = local_8 + 1)
    {
        if (P_SEATS(local_8) != 0)
        {
            if (local_c < m_field98[local_8])
            {
                local_c = m_field98[local_8];
                local_10 = local_8;
            }
            else if ((m_field98[local_8] == local_c) &&
                     (m_field78[local_10] < m_field78[local_8]))
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
            // ORIG 0x085dc58e：m_field78[seat]（PvP_Room 内嵌分数数组）
            int local_1c = m_field78[local_20];
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
    // ORIG 0x085dc060：写 PvP_Room 内嵌数组 m_field78[seat] / m_field98[seat]
    // （lea 0x1c(seat),*4 后 mov 0x8(disp) → +0x78；+0x24 → +0x98）。
    // ORIG 085dc040/085dc043：js/jg 各落独立空块岛（nop;jmp end / nop;fall end），
    // 双 goto 保块（同 check_recv_pvp_rank 已验证模式）。
    // 残差仅 outgoing-args reserve 跨版本尺寸差（ORIG sub $0x4 vs 4.4 $0x14），
    // 指令序列全等，N 封顶。
    if (seat < 0)
        goto SET_RES_RANGE;
    if (seat > 7)
        goto SET_RES_RANGE2;
    m_field78[seat] = result.GetTotalScore();
    m_field98[seat] = result.m_avg[0];
SET_RES_RANGE2:
    ;
SET_RES_RANGE:
    ;
}
