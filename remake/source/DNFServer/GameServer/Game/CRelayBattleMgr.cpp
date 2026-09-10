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
#include "PacketGuard.h"

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
    if (victimSeat >= 0 && victimSeat < 8)
    {
        m_battles[victimSeat].m_state = 3;
    }
    if (killerSeat >= 0 && killerSeat < 8)
    {
        ++m_battles[killerSeat].m_killCount;
    }
    if (bTime && m_state == 1)
    {
        m_state = 2;
        m_lastFightStartTime = GlobalData::s_systemTime_.getCurSec();
        G_TimerQueue()->InsertTimerInMilisecond((TimerEntry::OBJ_TYPE)1,
                                                m_pRoom->get_index(),
                                                (TIMER_MESSAGE)0x63, 3000,
                                                m_lastFightStartTime, 0);
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
    if (seat < 0)
        return;
    if (seat > 7)
        return;
    m_battles[seat].m_state = ready ? 1 : 0;
}

// ORIG 0x085de18c：条件读形 `movzbl 0x7(%eax,%ecx,1),%eax; xor $0x1,%eax;
// test %al,%al; je`。旧判词 N96+N18a（"寄存器物化无绑定不可达"）已作废——
// 正解 = 字段 m_requestFight **bool 化**（P248 档②）+ 裸 if(!f)，零布局影响。
void CRelayBattleMgr::OnRequestFight(int seat)
{
    if (seat < 0)
        return;
    if (seat > 7)
        return;
    // [b29 P248档②] 字段 bool 化后裸形即复现 ORIG `movzbl; xor$1; test`（P0 合规）。
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
    G_TimerQueue()->InsertTimerInMilisecond((TimerEntry::OBJ_TYPE)1,
                                            m_pRoom->get_index(),
                                            (TIMER_MESSAGE)0x63, 7000,
                                            m_lastFightStartTime, 0);
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
    // ORIG 0x085de474：`cmpl $0; jle; cmpl $2; jle` 两 guard 共享尾部 return 0
    // （Ghidra 合并短路 `(team<1)||(team>2)`）。独立双 return 使 4.4 生两套
    // mov$0;jmp 块（+2 指令）。合并短路由 4.4 复现 ORIG jle 链。
    if (team <= 0 || team > 2)
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
        // [b28c-L18] P0 清绑 + P227 全表达式临时链：ORIG 0x085de4dc 先以 4 条
        // 地址链把 m_changeSeqCount[team] 单段直落 %ebx（`mov (%eax,%edx,4),%ebx`）
        // 跨 cMyTrace ctor 保活，op() 处 `mov %ebx,0x8(%esp)`——具名 trace 对象+
        // 绑定形违规；P227 实证该形 4.4 可在 ctor 前右起求 varargs 并以 %ebx 跨
        // ctor 保活（0x0840215e 先例）。
        cMyTrace(__PRETTY_FUNCTION__, 0x13de, 0)(
            "ERROR ChangeSequenceCount overflow (%d)", m_changeSeqCount[team]);
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
        // [b28c-L18] 同上 P227 全表达式临时链（清 register asm("ebx") 绑定）。
        cMyTrace(__PRETTY_FUNCTION__, 0x13ea, 0)(
            "ERROR ChangeSequenceCount underflow (%d)", m_changeSeqCount[team]);
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
    // b28d-31: ORIG 0x085e8e1c 块序 = else 体（count>=1 随机段）在前、
    // count<1 递减段在后（P222/P272 块序交换）。
    if (count >= 1)
    {
        CRandomValue rv(seq, 1, 100, 10);
        std::generate(seq.begin(), seq.end(), rv);
        std::sort(seq.begin(), seq.end(), CDescending());
        std::random_shuffle(seq.begin(), seq.begin() + count);
    }
    else
    {
        n = (int)seq.size();
        val = n;
        for (i = 0; i < n; ++i)
        {
            seq[i] = val;
            --val;
        }
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

// [b29 P248档② 实修] ORIG 0x085de83e 条件读形 `movzbl 0x7(%eax,%ecx,1),%eax;
// xor $0x1,%eax; test %al,%al; je` ⇒ m_requestFight bool 化 + 裸 if(!f)。
// 旧判词 N96+N18a（"寄存器物化无绑定不可达"）作废：fresh 改后 DIFF→AE（62=62）。
void CRelayBattleMgr::ArrangeNoChangeSequence(std::vector<int>& seq, int team,
                                              int& outIdx)
{
    for (int i = 0; i < 8; ++i)
    {
        if (m_battles[i].m_team == team && m_battles[i].m_state == 1)
        {
            // [b29 P248档②] 同 OnRequestFight：bool 字段 + 裸形。
            if (!m_battles[i].m_requestFight)
            {
                m_battles[i].m_seq = (short)seq[outIdx++];
            }
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
            switch (m_battles[i].m_state)
            {
            case 2:
                return -1;
            case 1:
                if (bestSeq < m_battles[i].m_seq)
                {
                    bestSeq = m_battles[i].m_seq;
                    best = i;
                }
                break;
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

// [b29-R28 实修] ORIG 0x085dead6：150/150（gap=0）⇒ 本 TU 现 ae（near=0）。
// 修法 = P171 形态复原：`guard.put_byte(0)` → `guard.put_byte(count)`，ORIG
// 0x085deb71/0x085deb74 为「movl $0x0,-0x10(%ebp) → mov -0x10(%ebp),%eax →
// mov %eax,0x4(%esp)」= 读 count 槽而非立即数 0（原写法少 1 条且形态错）。
// 备注（非本件判定项）：OURS 槽 guard=-0x28/index=-0x1c vs ORIG guard=-0x24/
// index=-0x28 系 4.4「聚合优先占低槽、标量分区」分区差（N82附）；删具名 index
// 改内联实参已实测反劣（破坏 ORIG get_index 调用序 + 编译失败）⇒ 保留。
int CRelayBattleMgr::TurnPlayer()
{
    if (m_pRoom->IsEndPvpBattle())
    {
        return 1;
    }
    PacketGuard guard;
    int index;   // ORIG -0x28（比 guard -0x24 深；4.4 把聚合放最深 ⇒ NEAR 槽互换，N82附）
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
        guard.put_byte(count);   // ORIG 0x085deb71/0x085deb74：读 count 槽（非立即数）
        if (m_changeSeqCount[team] > 0)
        {
            for (i = 0; i < 8; ++i)
            {
                if (m_battles[i].m_requestFight &&
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
