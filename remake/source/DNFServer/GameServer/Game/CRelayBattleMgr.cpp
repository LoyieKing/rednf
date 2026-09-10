// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CRelayBattleMgr/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
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
#include "CRelayBattleMgr/Reset.inc"
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
#include "CRelayBattleMgr/Reset_1.inc"
{
    for (int i = 0; i < 8; ++i)
    {
        m_battles[i].Reset();
    }
    ResetChangeSequenceCount();
    m_lastFightStartTime = 0;
    m_state = 0;
}

#include "CRelayBattleMgr/SetParent.inc"
{
    m_pRoom = room;
}

#include "CRelayBattleMgr/OnCreateRoom.inc"
{
    if (a < 0)
        return;
    if (a > 7)
        return;
}

#include "CRelayBattleMgr/OnJoinRoom.inc"
{
    if (a < 0)
        return;
    if (a > 7)
        return;
}

#include "CRelayBattleMgr/OnLeaveRoom.inc"
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

#include "CRelayBattleMgr/OnDiePlayer.inc"
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

#include "CRelayBattleMgr/OnChangeTeam.inc"
{
    if (seat < 0)
        return;
    if (seat > 7)
        return;
    m_battles[seat].m_team = (char)team;
}

#include "CRelayBattleMgr/OnReady.inc"
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
#include "CRelayBattleMgr/OnRequestFight.inc"
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

#include "CRelayBattleMgr/OnStart.inc"
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

#include "CRelayBattleMgr/OnFight.inc"
{
    m_lastFightStartTime = 0;
    ArrangeMatchSequence();
    TurnPlayer();
    m_state = 1;
}

// ============================================================================
// 访问器
// ============================================================================
#include "CRelayBattleMgr/GetKillCount.inc"
{
    return m_battles[seat].m_killCount;
}

#include "CRelayBattleMgr/GetTimerKey.inc"
{
    return m_lastFightStartTime;
}

#include "CRelayBattleMgr/GetChangeSequenceCount.inc"
{
    // ORIG 0x085de474：`cmpl $0; jle; cmpl $2; jle` 两 guard 共享尾部 return 0
    // （Ghidra 合并短路 `(team<1)||(team>2)`）。独立双 return 使 4.4 生两套
    // mov$0;jmp 块（+2 指令）。合并短路由 4.4 复现 ORIG jle 链。
    if (team <= 0 || team > 2)
        return 0;
    return m_changeSeqCount[team];
}

#include "CRelayBattleMgr/GetAllKillUserIndex.inc"
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
#include "CRelayBattleMgr/IncreaseChangeSequenceCount.inc"
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

#include "CRelayBattleMgr/DecreaseChangeSequenceCount.inc"
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

#include "CRelayBattleMgr/ResetChangeSequenceCount.inc"
{
    for (int team = 1; team < 3; ++team)
    {
        m_changeSeqCount[team] = 0;
    }
}

#include "CRelayBattleMgr/CreateRandomSequenceVector.inc"
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

#include "CRelayBattleMgr/ArrangeChangeSequence.inc"
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
#include "CRelayBattleMgr/ArrangeNoChangeSequence.inc"
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

#include "CRelayBattleMgr/ArrangeMatchSequence.inc"
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

#include "CRelayBattleMgr/GetNextPlayer.inc"
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

#include "CRelayBattleMgr/SendRequestFight.inc"
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
#include "CRelayBattleMgr/TurnPlayer.inc"
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
