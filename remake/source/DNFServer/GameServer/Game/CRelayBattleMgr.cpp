// df_game_r CRelayBattleMgr（G2-4 PvP 链，接力战管理器，PvP_Room +0x620）还原。
// 逐函数对照 docs/class_func_reports/CRelayBattleMgr.md 与 ORIG 反汇编实现；
// 目标：编译产物与 ORIG 逐操作数一致（AE 口径）。
// PvP_Room / CRelayBattle / CRandomValue / CDescending 跨类方法按声明引用
// ORIG 真实符号（不新造桩；链接桩由 PvP_deps.cpp / 主 agent 提供）。
#include <algorithm>
#include <cstring>
#include <vector>

#include "CRelayBattleMgr.h"
#include "CSystemTime.h"
#include "GameTypes.h"
#include "InterfacePacketBuf.h"
#include "PvP_Room.h"
#include "TimerQueue.h"

// ============================================================================
// GlobalData（CUserCharacInfo.h 已声明 namespace GlobalData；补缺失成员）
// ============================================================================
namespace GlobalData
{
extern CSystemTime s_systemTime_;
}

// ============================================================================
// 依赖子对象 / 外部函数（PvP_Room.h / TimerQueue.h 权威头提供真实声明）
// ============================================================================

// ---- 随机排序辅助（ORIG 弱符号，定义归 PvP_deps.cpp / 主 agent） ----
class CRandomValue
{
public:
    CRandomValue(std::vector<int>& seq, int a, int b, int c);
    int operator()();

    std::vector<int>* m_pSeq;  // +0x00
    int m_end;                 // +0x04
    int m_start;               // +0x08
    int m_count;               // +0x0c
};

class CDescending
{
public:
    bool operator()(int a, int b);
};

// ---- PacketGuard（ORIG 0xc 字节：InterfacePacketBuf + int + char；
//      构造/析构由 GameStubs 提供单例定义） ----
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
// CRelayBattle（0xc，接力战单槽子对象；ORIG 0x085de01a）
// ============================================================================
void CRelayBattle::Reset()
{
    m_state = 0;
    m_seq = 0;
    m_team = 0;
    m_requestFight = 0;
    m_killCount = 0;
}

// ============================================================================
// 生命周期 / 房间钩子
// ============================================================================
void CRelayBattleMgr::Reset()
{
    for (int i = 0; i < 8; ++i)
    {
        m_battles[i].Reset();
    }
    ResetChangeSequenceCount();
    m_lastFightStartTime = 0;
    m_state = 0;
}

void CRelayBattleMgr::SetParent(PvP_Room* room)
{
    m_pRoom = room;
}

void CRelayBattleMgr::OnCreateRoom(int a)
{
    if (a < 0)
        return;
    if (a > 7)
        return;
}

void CRelayBattleMgr::OnJoinRoom(int a)
{
    if (a < 0)
        return;
    if (a > 7)
        return;
}

void CRelayBattleMgr::OnLeaveRoom(int seat)
{
    if (seat < 0)
        return;
    if (seat > 7)
        return;
    if (m_battles[seat].m_state == 2)
    {
        OnDiePlayer(seat, -1, true);
    }
    m_battles[seat].Reset();
}

void CRelayBattleMgr::OnDiePlayer(int victimSeat, int killerSeat, bool bTime)
{
    bool flag = bTime;
    if (victimSeat >= 0 && victimSeat < 8)
    {
        m_battles[victimSeat].m_state = 3;
    }
    if (killerSeat >= 0 && killerSeat < 8)
    {
        ++m_battles[killerSeat].m_killCount;
    }
    if (flag && m_state == 1)
    {
        m_state = 2;
        m_lastFightStartTime = GlobalData::s_systemTime_.getCurSec();
        int cur = m_lastFightStartTime;
        int roomIdx = m_pRoom->get_index();
        TimerQueue* tq = G_TimerQueue();
        tq->InsertTimerInMilisecond((TimerEntry::OBJ_TYPE)1, roomIdx,
                                    (TIMER_MESSAGE)0x63, 3000, cur, 0);
    }
}

void CRelayBattleMgr::OnChangeTeam(int seat, int team)
{
    if (seat < 0)
        return;
    if (seat > 7)
        return;
    m_battles[seat].m_team = (char)team;
}

void CRelayBattleMgr::OnReady(int seat, bool ready)
{
    bool flag = ready;
    if (seat < 0)
        return;
    if (seat > 7)
        return;
    m_battles[seat].m_state = flag ? 1 : 0;
}

void CRelayBattleMgr::OnRequestFight(int seat)
{
    if (seat < 0)
        return;
    if (seat > 7)
        return;
    if (!m_battles[seat].m_requestFight)
    {
        m_battles[seat].m_requestFight = 1;
        IncreaseChangeSequenceCount(m_battles[seat].m_team);
    }
    else
    {
        m_battles[seat].m_requestFight = 0;
        DecreaseChangeSequenceCount(m_battles[seat].m_team);
    }
    SendRequestFight(seat);
}

void CRelayBattleMgr::OnStart()
{
    m_lastFightStartTime = GlobalData::s_systemTime_.getCurSec();
    m_readyCount = 0;
    for (int i = 0; i < 8; ++i)
    {
        if (m_battles[i].m_state == 1)
        {
            ++m_readyCount;
        }
    }
    int cur = m_lastFightStartTime;
    int roomIdx = m_pRoom->get_index();
    TimerQueue* tq = G_TimerQueue();
    tq->InsertTimerInMilisecond((TimerEntry::OBJ_TYPE)1, roomIdx,
                                (TIMER_MESSAGE)0x63, 7000, cur, 0);
}

void CRelayBattleMgr::OnFight()
{
    m_lastFightStartTime = 0;
    ArrangeMatchSequence();
    TurnPlayer();
    m_state = 1;
}

// ============================================================================
// 访问器
// ============================================================================
int CRelayBattleMgr::GetKillCount(int seat)
{
    return m_battles[seat].m_killCount;
}

int CRelayBattleMgr::GetTimerKey()
{
    return m_lastFightStartTime;
}

int CRelayBattleMgr::GetChangeSequenceCount(int team)
{
    if (team <= 0)
        return 0;
    if (team > 2)
        return 0;
    return m_changeSeqCount[team];
}

int CRelayBattleMgr::GetAllKillUserIndex(int* out)
{
    for (int i = 0; i < 8; ++i)
    {
        if (m_battles[i].m_killCount > 1 &&
            (signed char)m_battles[i].m_killCount >= m_readyCount / 2 &&
            m_battles[i].m_state == 2)
        {
            if (out != 0)
            {
                *out = m_battles[i].m_team;
            }
            return i;
        }
    }
    return -1;
}

// ============================================================================
// 顺序编排
// ============================================================================
void CRelayBattleMgr::IncreaseChangeSequenceCount(int team)
{
    if (team <= 0)
        return;
    if (team > 2)
        return;
    ++m_changeSeqCount[team];
    if (m_changeSeqCount[team] > 3)
    {
        int v = m_changeSeqCount[team];
        cMyTrace trace("void CRelayBattleMgr::IncreaseChangeSequenceCount(int)",
                       0x13de, 0);
        trace("ERROR ChangeSequenceCount overflow (%d)", v);
        m_changeSeqCount[team] = 4;
    }
}

void CRelayBattleMgr::DecreaseChangeSequenceCount(int team)
{
    if (team <= 0)
        return;
    if (team > 2)
        return;
    --m_changeSeqCount[team];
    if (m_changeSeqCount[team] < 0)
    {
        int v = m_changeSeqCount[team];
        cMyTrace trace("void CRelayBattleMgr::DecreaseChangeSequenceCount(int)",
                       0x13ea, 0);
        trace("ERROR ChangeSequenceCount underflow (%d)", v);
        m_changeSeqCount[team] = 0;
    }
}

void CRelayBattleMgr::ResetChangeSequenceCount()
{
    for (int team = 1; team < 3; ++team)
    {
        m_changeSeqCount[team] = 0;
    }
}

void CRelayBattleMgr::CreateRandomSequenceVector(std::vector<int>& seq,
                                                 int count)
{
    int n;
    int val;
    int i;
    if ((unsigned int)count > seq.size())
    {
        count = (int)seq.size();
    }
    if (count < 1)
    {
        n = (int)seq.size();
        val = n;
        for (i = 0; i < n; ++i)
        {
            seq[i] = val;
            --val;
        }
    }
    else
    {
        CRandomValue rv(seq, 1, 100, 10);
        std::generate(seq.begin(), seq.end(), rv);
        std::sort(seq.begin(), seq.end(), CDescending());
        std::random_shuffle(seq.begin(), seq.begin() + count);
    }
}

void CRelayBattleMgr::ArrangeChangeSequence(std::vector<int>& seq, int team,
                                            int& outIdx)
{
    for (int i = 0; i < 8; ++i)
    {
        if (m_battles[i].m_team == team && m_battles[i].m_state == 1 &&
            m_battles[i].m_requestFight)
        {
            m_battles[i].m_seq = (short)seq[outIdx++];
        }
    }
}

void CRelayBattleMgr::ArrangeNoChangeSequence(std::vector<int>& seq, int team,
                                              int& outIdx)
{
    for (int i = 0; i < 8; ++i)
    {
        if (m_battles[i].m_team == team && m_battles[i].m_state == 1 &&
            !m_battles[i].m_requestFight)
        {
            m_battles[i].m_seq = (short)seq[outIdx++];
        }
    }
}

void CRelayBattleMgr::ArrangeMatchSequence()
{
    int team;
    int count;
    for (team = 1; team < 3; ++team)
    {
        int outIdx = 0;
        std::vector<int> seq(4u, 0);
        count = GetChangeSequenceCount(team);
        CreateRandomSequenceVector(seq, count);
        ArrangeChangeSequence(seq, team, outIdx);
        ArrangeNoChangeSequence(seq, team, outIdx);
    }
}

int CRelayBattleMgr::GetNextPlayer(int team)
{
    int best = -1;
    int bestSeq = 0;
    int i = 0;
    while (i <= 7)
    {
        if (m_battles[i].m_team == team)
        {
            if (m_battles[i].m_state == 1)
            {
                if (bestSeq < m_battles[i].m_seq)
                {
                    bestSeq = m_battles[i].m_seq;
                    best = i;
                }
            }
            else if (m_battles[i].m_state == 2)
            {
                return -1;
            }
        }
        ++i;
    }
    m_battles[best].m_state = 2;
    return best;
}

int CRelayBattleMgr::SendRequestFight(int seat)
{
    PacketGuard guard;
    guard.put_header(0, 0x71);
    guard.put_byte(seat);
    guard.finalize(true);
    m_pRoom->send_to_pvp(guard);
    return 1;
}

int CRelayBattleMgr::TurnPlayer()
{
    if (m_pRoom->IsEndPvpBattle())
    {
        return 1;
    }
    int index;
    PacketGuard guard;
    int seat;
    int team;
    int count;
    int i;
    guard.put_header(0, 0x70);
    guard.put_byte(2);
    for (team = 1; team < 3; ++team)
    {
        seat = GetNextPlayer(team);
        guard.put_byte(seat);
        index = guard.get_index();
        count = 0;
        guard.put_byte(0);
        if (m_changeSeqCount[team] > 0)
        {
            for (i = 0; i < 8; ++i)
            {
                if (m_battles[i].m_requestFight != 0 &&
                    m_battles[i].m_team == team)
                {
                    guard.put_byte(i);
                    guard.put_short(m_battles[i].m_seq);
                    ++count;
                    m_battles[i].m_requestFight = 0;
                }
            }
        }
        guard.put_byte(index, count);
    }
    guard.finalize(true);
    m_pRoom->send_to_pvp(guard);
    ResetChangeSequenceCount();
    return 1;
}
