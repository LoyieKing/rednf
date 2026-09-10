// df_game_r 还原 —— village_attacked::CVillageMonsterMgr::OnKillVillageMonster
// 调用链（CBattle_Field.cpp 引用；独立 TU，避免与并行批次争文件）。
// 逐函数对照 ORIG df_game_r 反汇编（证据见各函数注释）；不编译，主 agent 统一验证。
//
// 【接线说明】CBattle_Field.cpp:887 以 4 参调用：
//     OnKillVillageMonster(m_villageMonsterCount, m_field130, (unsigned short)m_villageMonsterId, false)
//   ORIG 存在匹配重载 _ZN16village_attacked18CVillageMonsterMgr20OnKillVillageMonsterEiitb
//   （int,int,unsigned short,bool）@ 0x86b4964 —— 调用点正确，本文件实现该重载。
//   任务备注中的 0x86b4866 是另一重载 OnKillVillageMonster(CUser*,bool)（本工程未引用，不实现）。

#include <map>
#include <vector>

#include <cstring>

#include "CUser.h"
#include "CParty.h"
#include "TimerQueue.h"
#include "GlobalData.h"
#include "CServerProxyMgr.h"
#include "CMonitorServerProxy.h"
#include "PacketGuard.h"
#include "GameWorld.h"          // GameWorld::send_to_area / G_GameWorld
#include "CVillageMonsterMgr.h" // village_attacked 域唯一权威头

// ============================================================================
// 外部符号（真实头声明；不再使用 asm 标签）
// ============================================================================
TimerQueue* G_TimerQueue();

// ---- Packet_VillageMonsterFightResult（ORIG ctor 0x86b4b84，0x2b 字节） ----
// PacketHeader(0x1772, 0x2b) + [4](uid,characNo) @ +0x0a + flag byte @ +0x2a
// [b29-SymForceBatch] 定义移到文件全局作用域：ORIG mangled 为顶层
// _ZN32Packet_VillageMonsterFightResultC1Ev（W 0x086b4b84，无 namespace 前缀）；
// 原定义位于 namespace village_attacked 内 ⇒ 发明 _ZN16village_attacked32…C1/C2。
// 构造/字段语义逐字不变。
struct Packet_VillageMonsterFightResult
{
    Packet_VillageMonsterFightResult()   // ORIG W 0x86b4b84；P223 双成员索引对形
        : m_flag(0)
    {
        for (int i = 0; i <= 3; ++i)
        {
            m_uids[i] = 0;
            m_characNos[i] = 0;
        }
    }
    unsigned short m_header0;   // +0x00
    unsigned short m_header2;   // +0x02
    char m_pad4[6];             // +0x04
    unsigned int m_uids[4];     // +0x0a
    unsigned int m_characNos[4];// +0x1a
    unsigned char m_flag;       // +0x2a
} __attribute__((packed));      // ORIG ctor 0x86b4b84 紧凑 0x2b 布局（无 packed 会插 2B padding）

namespace village_attacked
{

// ============================================================================
// CVillageMonsterMgr::OnKillVillageMonster(int,int,unsigned short,bool)
//   —— ORIG 0x86b4964
//   objdump: Zone zone((char)a,(char)b); m_areas.find(zone);
//            it == end -> return 0x15
//            return it->second.OnKillVillageMonster(0, c, d);
// ============================================================================

// OnMoveArea / OnEvent / InsertVillageMonster 由已登记的 CVillageMonsterMgr.cpp
// 提供定义（本 TU 不再重复定义，避免多重定义）。

// [round2-L30 复核·N82/P200] 残余：ORIG `sub $0x38` 无 callee-saved vs 我方
//   `push %esi; push %ebx; sub $0x30`（我方多 2 push，跨 find 保活 c/d）+ 槽整体浅 4
//   （ORIG -0x18/-0x12/-0x10 vs 我方 -0x14/-0xe/-0xc）⇒ N82 帧/槽分区族；ORIG 0x086b4964。
int CVillageMonsterMgr::OnKillVillageMonster(int a, int b, unsigned short c, bool d)
{
    Zone zone((unsigned char)a, (unsigned char)b);
    std::map<Zone, CVillageMonsterArea>::iterator it = m_areas.find(zone);
    if (it == m_areas.end())
        return 0x15;
    return it->second.OnKillVillageMonster(0, c, d);
}

// ============================================================================
// CVillageMonsterArea::OnKillVillageMonster(CUser*,unsigned short,bool)
//   —— ORIG 0x86b4070
//   objdump: m_monsters.find(zone); it != end 时:
//            ret = it->second.OnKillVillageMonster(user, +0x28, +0x2c, +0x38, flag);
//            if (flag && ret == 0) m_monsters.erase(it);
//            return ret;  否则 return 0
// ============================================================================
int CVillageMonsterArea::OnKillVillageMonster(CUser* user, unsigned short zone, bool flag)
{
    std::map<unsigned short, CVillageMonster>::iterator it = m_monsters.find(zone);
    if (it != m_monsters.end())
    {
        CVillageMonster* monster = &it->second;
        int ret = monster->OnKillVillageMonster(user, m_field28, m_field2c, m_field38, flag);
        if (flag && ret == 0)
            m_monsters.erase(it);
        return ret;
    }
    return 0;
}

// ============================================================================
// CVillageMonster::OnKillVillageMonster(CUser*,int,int,int,bool)
//   —— ORIG 0x86b34a0
//   objdump: if (m_state != 1) return 0x13;
//            if (user && user->GetPartyIndex() != m_partyIndex) return 0x13;
//            if (flag) { OnChangeState(2); TimerQueue::InsertTimer(2,0,0x86,c,a,b);
//                        SendVillageMonsterFightResult(user, true); }
//            else     { OnChangeState(0); SendVillageMonsterFightResult(user, false); }
//            return 0;
// ============================================================================
// [round2-L30 修] 否定极性 + 分支交换：ORIG 0x086b34ef `movzbl -0xc(%ebp),%eax;
//   xor $0x1,%eax; test %al,%al; je <+0x8e>`（!flag 为真则落 fall-through 的
//   OnChangeState(0)/SendFight(false) 段，+0x8e 为 true 段）⇒ 源形必写
//   `if (!flag) { …0… } else { …2… }`（P222/P214①；正极性 `if (flag)` 4.4 产直判
//   `cmpb; je` 且块序相反）。
int CVillageMonster::OnKillVillageMonster(CUser* user, int a, int b, int c, bool flag)
{
    if (m_state != 1)
        return 0x13;
    if (user != 0)
    {
        if (user->GetPartyIndex() != m_partyIndex)
            return 0x13;
    }
    if (!flag)
    {
        OnChangeState(ENUM_VILLAGE_MONSTER_STATE_0);
        // ORIG 0x086b350d/0x086b357b：两分支都 `movzbl -0xc(%ebp),%eax;
        //   mov %eax,0x8(%esp)` ⇒ 实参是 flag 变量本身（非 false/true 常量）。
        SendVillageMonsterFightResult(user, flag);
        return 0;                       // ORIG 0x086b3527 `mov $0x0,%eax; jmp`（分支内独立设值）
    }
    else
    {
        OnChangeState(ENUM_VILLAGE_MONSTER_STATE_2);
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                    (TIMER_MESSAGE)0x86, c, a, b);
        SendVillageMonsterFightResult(user, flag);
        return 0;                       // ORIG 0x086b3595 `mov $0x0,%eax`
    }
}

// ============================================================================
// CVillageMonster::OnChangeState(ENUM_VILLAGE_MONSTER_STATE)
//   —— ORIG 0x86b35a0
//   objdump: m_state = state; PacketGuard 打包：put_header(0,0xf0); put_byte(m_state);
//            put_short(m_id); finalize(true); m_area->SendPacket(pg);
// ============================================================================
void CVillageMonster::OnChangeState(ENUM_VILLAGE_MONSTER_STATE state)
{
    // P95：ORIG 0x86b35a0 首句 store 先载 this 后载 state；4.4 直成员赋值先备值。
    // 具名视图复现（仅表达布局；m_state = CVillageMonster +0x14）。
    struct CVMonsterStateView
    {
        char m_head[0x14];
        int  m_state;
    };
    ((CVMonsterStateView*)this)->m_state = state;
    PacketGuard pg;
    pg.clear();
    pg.put_header(0, 0xf0);
    pg.put_byte(m_state);
    pg.put_short(m_id);
    pg.finalize(true);
    m_area->SendPacket(pg);
}

// ============================================================================
// CVillageMonsterArea::SendPacket(PacketGuard&)
//   —— ORIG 0x86b3b9c
//   objdump: G_GameWorld()->send_to_area(+0x28, +0x2c, packet);
// ============================================================================
void CVillageMonsterArea::SendPacket(PacketGuard& packet)
{
    G_GameWorld()->send_to_area(m_field28, m_field2c, packet);
}

// ============================================================================
// CVillageMonster::SendVillageMonsterFightResult(CUser*,bool)
//   —— ORIG 0x86b330a
//   objdump: user==0 -> return; party=user->GetParty(); party==0 -> return;
//            for i in 0..3: member=party->get_user(i); member!=0 时:
//               pkt.m_uids[i]=member->GetUID();
//               pkt.m_characNos[i]=member->get_charac_no(-1);
//               if (s_villageMonsterMgr->OnEvent() && flag)
//                   member->IncVillageAttackCount();
//            pkt.m_flag=flag;
//            s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup())
//                ->SendTcpPacket((char*)&pkt, 0x2b);
//            PacketGuard pg: clear; put_header(0,0xf9); put_byte(flag);
//                finalize(true); party->send_to_party(pg);
// [B24D判词 N] 语义正确：pkt 全部清零 + header 0x1772/0x2b + 循环填 uid/characNo(4) +
//   OnEvent()&&flag→IncVillageAttackCount + m_flag=flag + SendTcpPacket 0x2b + PacketGuard
//   0xf9 发送。差异=ORIG 用 ctor W 0x86b4b84 一次性 memset 全结构，我方显式
//   memset(m_pad4)+双循环清0（多 ~27 条），纯 lowering 表示差异，语义等价。
// ============================================================================
// [round2-L30 复核·N/B24D] 残余 3 条 + 整帧平移 4：ORIG ctor W 0x86b4b84 一次性 memset
//   全结构 vs 我方显式清零 lowering（语义等价）；ORIG 0x086b330a。
void CVillageMonster::SendVillageMonsterFightResult(CUser* user, bool flag)
{
    if (user == 0)
        return;
    CParty* party = (CParty*)user->GetParty();
    if (party == 0)
        return;

    Packet_VillageMonsterFightResult pkt;
    // ORIG ctor 0x86b4b84：PacketHeader(0x1772,0x2b) + m_flag=0 + m_keys[8] 清零。

    for (int i = 0; i <= 3; ++i)
    {
        CUser* member = party->get_user(i);
        if (member != 0)
        {
            pkt.m_uids[i] = member->GetUID();
            pkt.m_characNos[i] = member->get_charac_no(-1);
            if (GlobalData::s_villageMonsterMgr->OnEvent())
                if (flag)
                member->IncVillageAttackCount();
        }
    }
    pkt.m_flag = flag;

    CMonitorServerProxy* monitorProxy =
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
    monitorProxy->SendTcpPacket((char*)&pkt, 0x2b);

    PacketGuard pg;
    pg.clear();
    pg.put_header(0, 0xf9);
    pg.put_byte(flag);
    pg.finalize(true);
    party->send_to_party(pg);
}

// ============================================================================
// CVillageMonster::MakeVillageMonsterListPacket(PacketGuard&)
//   —— ORIG 0x86b3666（0x80 字节）
//   objdump: put_short(m_id); put_short(m_zone); put_byte(m_state);
//            put_short(m_field04); put_short(m_field08); return 1。
// ============================================================================
// [b28d-L8] AE | 修法：m_state int→ENUM_VILLAGE_MONSTER_STATE（P254 enum 字段两段装载 mov 0x14(%eax),%eax;mov %eax,%edx，ORIG 0x86b3666）；旧 N18a 判词作废
bool CVillageMonster::MakeVillageMonsterListPacket(PacketGuard& packet)
{
    // [b28-L11 判 N·N18a] ORIG put_short(m_field04) 实参装载为 `mov 0x14(%eax),%eax;
    // mov %eax,%edx` 两段死 regmove（4.1 冗余产物）；4.4 恒折单段直落 %edx，
    // 「反向亦不可造」判词在册（负例表 N18a）。ndiff 3/gap 1 封顶。
    packet.put_short(m_id);
    packet.put_short(m_zone);
    packet.put_byte(m_state);
    packet.put_short(m_field04);
    packet.put_short(m_field08);
    return true;
}

// ============================================================================
// CVillageMonsterArea::SendVillageMonsterList(CUser*) —— ORIG 0x86b37fa（0x168）
//   objdump: PacketGuard; clear; put_header(0,0xee); idx=get_index();
//            put_byte(0 占位); 遍历 m_monsters：MakeVillageMonsterListPacket
//            成功则 count++；put_byte(idx,count)；finalize(true)；
//            user==0 → SendPacket(packet)，否则 user->Send(packet)。
// ============================================================================
// [round2-L30 复核·N126] 残余=整帧槽号常数平移 4（ORIG packet -0x20/countIdx -0x24/
//   count -0x10 vs 我方 -0x1c/-0x20/-0xc，间距全等；帧同为 sub $0x30）⇒ 判据⑪ (b)
//   类，声明重排无效（ORIG 0x086b37fa）。已修：占位字节改传具名 count（ORIG
//   0x086b3814 `mov -0x10(%ebp),%eax; mov %eax,0x4(%esp)`，非立即数 0）。
void CVillageMonsterArea::SendVillageMonsterList(CUser* user)
{
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0xee);
    int countIdx = packet.get_index();
    int count = 0;
    // ORIG 0x086b37ff：占位字节传的是**具名 count 变量**（0x81 `mov -0x10(%ebp),%eax;
    //   mov %eax,0x4(%esp)`），非立即数 0 ⇒ 源形 `put_byte(count)`（此时 count==0）。
    packet.put_byte(count);
    for (std::map<unsigned short, CVillageMonster>::iterator it =
             m_monsters.begin();
         it != m_monsters.end(); ++it)
    {
        if (it->second.MakeVillageMonsterListPacket(packet))
            ++count;
    }
    packet.put_byte(countIdx, count);
    packet.finalize(true);
    if (user == 0)
        SendPacket(packet);
    else
        user->Send(packet);
}

}  // namespace village_attacked

// ORIG W 0x084ed3f8：return m_field20（当前奖励类型）。
int village_attacked::CVillageMonsterMgr::GetRewardType()
{
    return m_field20;
}

// ORIG W 0x0847dd44：m_field20 = reward（单赋值）。
void village_attacked::CVillageMonsterMgr::SetRewardType(ENUM_VILLAGE_ATTACKED_REWARD reward)
{
    // P95：ORIG 先载 this 后载 reward；具名视图复现（m_field20 = +0x20）。
    struct CVMgrRewardView
    {
        char m_head[0x20];
        int  m_field20;
    };
    ((CVMgrRewardView*)this)->m_field20 = reward;
}
