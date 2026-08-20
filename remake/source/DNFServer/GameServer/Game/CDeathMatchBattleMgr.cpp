#include "LogManager.h"
// ============================================================================
// df_game_r CDeathMatchBattleMgr（G2-5 批次，PvP_Room +0x69c，尺寸 0x44）
// 逐函数对照 docs/class_func_reports/CDeathMatchBattleMgr.md 与 ORIG 反汇编还原。
// 布局（由 SetParent/IncreaseKillCount/IncreaseDeathCount/OnStart 反汇编推导）：
//   +0x00 PvP_Room* m_pRoom
//   +0x04 int m_killCount[8]     （IncreaseKillCount 用 this+idx*4+4）
//   +0x24 int m_deathCount[8]    （IncreaseDeathCount 用 this+(idx+8)*4+4）
// sizeof == 0x44。
// 返回类型以 ORIG 反汇编/mangling 为准：checkWinner* 为 void（GameStubs 桩的
// int 是错的）；checkGameOver* 返回 0/1（调用点 test %al，声明为 int 与 ORIG
// mov $imm,%eax 形态一致）。
// 跨类调用一律经 asm-label extern（PvP_Room/PvpUserTable/TimerQueue/LogManager/
// cMyTrace），mangled 名与 ORIG 完全一致。
// ============================================================================

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <utility>

class CUser;
class PvP_Room;

// ---- 最小同名类：仅用于 SetWinTeam 调用（未定义，链 ORIG/桩实现） ----
class PvpUserTable
{
public:
    void SetWinTeam(int team);
};

// ---- cMyTrace（ORIG ctor 0854f718 / operator() 0854f788，外部定义） ----
class cMyTrace
{
public:
    cMyTrace(const char* func, int line, int flag);
    void operator()(const char* fmt, ...);

    char m_pad[16];
};

// ---- 跨类 asm-label extern（mangled 名与 ORIG 一致） ----
extern "C" unsigned int sub_PvP_Room_gen_timer_key(void* self, int msg)
    asm("_ZN8PvP_Room13gen_timer_keyE13TIMER_MESSAGE");
extern "C" int sub_PvP_Room_get_index(void* self)
    asm("_ZN8PvP_Room9get_indexEv");
extern "C" void* sub_G_TimerQueue()
    asm("_Z12G_TimerQueuev");
extern "C" void sub_TimerQueue_InsertTimer(
    void* self, int objType, int a, int msg, int b, unsigned int key, unsigned int c)
    asm("_ZN10TimerQueue11InsertTimerEN10TimerEntry8OBJ_TYPEEi13TIMER_MESSAGEiij");

// ---- std::pair 排序比较器（ORIG 085df55f T） ----
bool funDeathMatchKillCntCompare(const std::pair<int, int>& a,
                                 const std::pair<int, int>& b)
{
    return a.second > b.second;
}

// ---- CDeathMatchBattleMgr（0x44） ----
class CDeathMatchBattleMgr
{
public:
    void Reset();
    void SetParent(PvP_Room* room);
    void OnCreateRoom(int roomIdx);
    void OnStart();
    void IncreaseKillCount(int userIdx);
    void IncreaseDeathCount(int userIdx);
    int getKillCount(int userIdx);
    int getDeathCount(int userIdx);
    int getMaxKillCount();
    int getRanking(CUser** users, int* out) const;
    int checkGameOverForSinglePlay(int mode);
    void checkWinnerForSinglePlay(CUser** users, bool* win, PvpUserTable& table);
    int checkGameOverForTeamPlay(CUser** users, int teamCnt, unsigned char* teamWin);
    void checkWinnerForTeamPlay(CUser** users, bool* win, unsigned char* teamWin,
                                PvpUserTable& table);
    void InsertTimerKilledUserForRevive(int roomIdx, int a);
    void OnJoinRoom(int roomIdx);
    void OnLeaveRoom(int roomIdx);
    void OnReady(int roomIdx, bool ready);
    void OnChangeTeam(int team, int a);
    void OnFight();

    PvP_Room* m_pRoom;     // +0x00
    int m_killCount[8];    // +0x04
    int m_deathCount[8];   // +0x24
};

// ==== ORIG 085dee22 ====
void CDeathMatchBattleMgr::Reset()
{
    memset(m_killCount, 0, sizeof(m_killCount));
    memset(m_deathCount, 0, sizeof(m_deathCount));
}

// ==== ORIG 085df92c（弱符号） ====
void CDeathMatchBattleMgr::SetParent(PvP_Room* room)
{
    m_pRoom = room;
}

// ==== ORIG 085dee66 ====
void CDeathMatchBattleMgr::OnCreateRoom(int roomIdx)
{
    if (roomIdx < 0 || roomIdx > 7)
        return;
}

// ==== ORIG 085deeca ====
void CDeathMatchBattleMgr::OnStart()
{
    memset(m_killCount, 0, sizeof(m_killCount));
    memset(m_deathCount, 0, sizeof(m_deathCount));
}

// ==== ORIG 085def0e ====
void CDeathMatchBattleMgr::IncreaseKillCount(int userIdx)
{
    if (userIdx >= 0 && userIdx <= 7)
        ++m_killCount[userIdx];
}

// ==== ORIG 085def34 ====
void CDeathMatchBattleMgr::IncreaseDeathCount(int userIdx)
{
    if (userIdx >= 0 && userIdx <= 7)
        ++m_deathCount[userIdx];
}

// ==== ORIG 085df524 ====
int CDeathMatchBattleMgr::getKillCount(int userIdx)
{
    if (userIdx > 7)
        return 0;
    return m_killCount[userIdx];
}

// ==== ORIG 085df540 ====
int CDeathMatchBattleMgr::getDeathCount(int userIdx)
{
    if (userIdx > 7)
        return 0;
    return m_deathCount[userIdx];
}

// ==== ORIG 085defcc ====
int CDeathMatchBattleMgr::getMaxKillCount()
{
    int max = 0;
    for (int i = 0; i <= 7; ++i)
    {
        if (m_killCount[i] > max)
            max = m_killCount[i];
    }
    return max;
}

// ==== ORIG 085def60 ====
void CDeathMatchBattleMgr::InsertTimerKilledUserForRevive(int roomIdx, int a)
{
    unsigned int key = sub_PvP_Room_gen_timer_key(m_pRoom, a + 0x33);
    void* timerQueue = sub_G_TimerQueue();
    sub_TimerQueue_InsertTimer(timerQueue, 5, roomIdx, a + 0x33, 3, key, 0);
}

// ==== ORIG 085df012 ====
int CDeathMatchBattleMgr::checkGameOverForSinglePlay(int mode)
{
    if (mode == 0)
        return 1;
    switch (mode)
    {
    case 2:
        return getMaxKillCount() > 1;
    case 3:
        return getMaxKillCount() > 2;
    case 4:
        return getMaxKillCount() > 3;
    case 5:
        return getMaxKillCount() > 4;
    case 6:
        return getMaxKillCount() > 5;
    case 7:
    case 8:
        return getMaxKillCount() > 6;
    default:
        return 1;   // mode 1 与 >8 均判结束
    }
}

// ==== ORIG 085df126 ====
int CDeathMatchBattleMgr::checkGameOverForTeamPlay(
    CUser** users, int teamCnt, unsigned char* teamWin)
{
    if (teamCnt == -1)
        return 1;
    int killTeam1 = 0;
    int killTeam2 = 0;
    for (int i = 0; i <= 7; ++i)
    {
        if (users[i] != 0 || m_killCount[i] != 0)
        {
            if (teamWin[i] == 1)
                killTeam1 += m_killCount[i];
            else if (teamWin[i] == 2)
                killTeam2 += m_killCount[i];
            else
                LogManager::logFormat(
                    1, "pvp.cpp",
                    "bool CDeathMatchBattleMgr::checkGameOverForTeamPlay(CUser**, int, unsigned char*)",
                    0x157f,
                    "CDeathMatchBattleMgr::checkWinnerForTeamPlay , ERROR, bTeamState[%d] = %d",
                    i, (unsigned int)teamWin[i]);
        }
    }
    int maxKill = killTeam1 > killTeam2 ? killTeam1 : killTeam2;
    switch (teamCnt)
    {
    case 1:
        return maxKill > 1;
    case 2:
        return maxKill > 4;
    case 3:
        return maxKill > 7;
    case 4:
        return maxKill > 9;
    default:
        return 1;
    }
}

// ==== ORIG 085df48a ====
void CDeathMatchBattleMgr::checkWinnerForSinglePlay(
    CUser** users, bool* win, PvpUserTable& table)
{
    for (int i = 0; i <= 7; ++i)
    {
        if (users[i] == 0)
            m_killCount[i] = 0;
        win[i] = false;
    }
    int* pMax = std::max_element(m_killCount, m_killCount + 8);
    int winIdx = (int)(pMax - m_killCount);
    if (winIdx <= 7)
        win[winIdx] = true;
    table.SetWinTeam(winIdx);
}

// ==== ORIG 085df296 ====
void CDeathMatchBattleMgr::checkWinnerForTeamPlay(
    CUser** users, bool* win, unsigned char* teamWin, PvpUserTable& table)
{
    int killTeam1 = 0;
    int killTeam2 = 0;
    for (int i = 0; i <= 7; ++i)
    {
        if (users[i] != 0)
        {
            if (teamWin[i] == 1)
                killTeam1 += m_killCount[i];
            else if (teamWin[i] == 2)
                killTeam2 += m_killCount[i];
            else
                LogManager::logFormat(
                    1, "pvp.cpp",
                    "void CDeathMatchBattleMgr::checkWinnerForTeamPlay(CUser**, bool*, unsigned char*, PvpUserTable&)",
                    0x1617,
                    "CDeathMatchBattleMgr::checkWinnerForTeamPlay , ERROR, bTeamState[%d] = %d",
                    i, (unsigned int)teamWin[i]);
        }
    }
    int aliveTeam1 = 0;
    int aliveTeam2 = 0;
    for (int i = 0; i <= 7; ++i)
    {
        if (win[i] && users[i] != 0)
        {
            int t = teamWin[i] - 1;
            if (t >= 0 && t <= 1)
            {
                if (t == 0)
                    ++aliveTeam1;
                else
                    ++aliveTeam2;
            }
        }
    }
    unsigned char winTeam;
    if (killTeam1 < killTeam2)
        winTeam = 2;
    else if (killTeam1 > killTeam2)
        winTeam = 1;
    else if (aliveTeam1 > aliveTeam2)
        winTeam = 1;
    else if (aliveTeam2 > aliveTeam1)
        winTeam = 2;
    else
        winTeam = (rand() & 1) ? 2 : 1;
    memset(win, 0, 8);
    for (int i = 0; i <= 7; ++i)
    {
        if (teamWin[i] == winTeam)
            win[i] = true;
    }
    table.SetWinTeam((int)winTeam);
}

// ==== ORIG 085df576（const） ====
int CDeathMatchBattleMgr::getRanking(CUser** users, int* out) const
{
    int count = 0;
    std::pair<int, int> rank[8];
    memset(out, 0, 0x20);
    for (int i = 0; i <= 7; ++i)
    {
        if (users[i] != 0)
        {
            rank[i] = std::make_pair(i, m_killCount[i]);
            ++count;
        }
        else
        {
            rank[i] = std::make_pair(i, -1);
        }
    }
    std::sort(rank, rank + 8, funDeathMatchKillCntCompare);
    for (int j = 0; j < count; ++j)
    {
        out[j] = rank[j].first;
        cMyTrace trace("int CDeathMatchBattleMgr::getRanking(CUser**, int*) const",
                       0x1695, 0);
        trace("CDeathMatchBattleMgr::getRanking => Room(%d), Idx(%d), Rank(%d)",
              sub_PvP_Room_get_index(m_pRoom), j, out[j]);
    }
    return count;
}

// ==== ORIG 085dee78 ====
void CDeathMatchBattleMgr::OnJoinRoom(int roomIdx)
{
    if (roomIdx < 0 || roomIdx > 7)
        return;
}

// ==== ORIG 085dee8a ====
void CDeathMatchBattleMgr::OnLeaveRoom(int roomIdx)
{
    if (roomIdx < 0 || roomIdx > 7)
        return;
}

// ==== ORIG 085deeae ====
void CDeathMatchBattleMgr::OnReady(int roomIdx, bool ready)
{
    if (roomIdx < 0 || roomIdx > 7)
        return;
}

// ==== ORIG 085dee9c ====
void CDeathMatchBattleMgr::OnChangeTeam(int team, int a)
{
    if (team < 0 || team > 7)
        return;
}

// ==== ORIG 085defc6 ====
void CDeathMatchBattleMgr::OnFight()
{
}
