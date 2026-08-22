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
int CVillageMonster::OnKillVillageMonster(CUser* user, int a, int b, int c, bool flag)
{
    if (m_state != 1)
        return 0x13;
    if (user != 0)
    {
        if (user->GetPartyIndex() != m_partyIndex)
            return 0x13;
    }
    if (flag)
    {
        OnChangeState(ENUM_VILLAGE_MONSTER_STATE_2);
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                    (TIMER_MESSAGE)0x86, c, a, b);
        SendVillageMonsterFightResult(user, true);
    }
    else
    {
        OnChangeState(ENUM_VILLAGE_MONSTER_STATE_0);
        SendVillageMonsterFightResult(user, false);
    }
    return 0;
}

// ============================================================================
// CVillageMonster::OnChangeState(ENUM_VILLAGE_MONSTER_STATE)
//   —— ORIG 0x86b35a0
//   objdump: m_state = state; PacketGuard 打包：put_header(0,0xf0); put_byte(m_state);
//            put_short(m_id); finalize(true); m_area->SendPacket(pg);
// ============================================================================
void CVillageMonster::OnChangeState(ENUM_VILLAGE_MONSTER_STATE state)
{
    m_state = state;
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

// ---- Packet_VillageMonsterFightResult（ORIG ctor 0x86b4b84，0x2b 字节） ----
// PacketHeader(0x1772, 0x2b) + [4](uid,characNo) @ +0x0a + flag byte @ +0x2a
struct Packet_VillageMonsterFightResult
{
    unsigned short m_header0;   // +0x00
    unsigned short m_header2;   // +0x02
    char m_pad4[6];             // +0x04
    unsigned int m_uids[4];     // +0x0a
    unsigned int m_characNos[4];// +0x1a
    unsigned char m_flag;       // +0x2a
};

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
// ============================================================================
void CVillageMonster::SendVillageMonsterFightResult(CUser* user, bool flag)
{
    if (user == 0)
        return;
    CParty* party = (CParty*)user->GetParty();
    if (party == 0)
        return;

    Packet_VillageMonsterFightResult pkt;
    pkt.m_header0 = 0x1772;
    pkt.m_header2 = 0x2b;
    memset(pkt.m_pad4, 0, sizeof(pkt.m_pad4));
    for (int i = 0; i <= 3; ++i)
    {
        pkt.m_uids[i] = 0;
        pkt.m_characNos[i] = 0;
    }
    pkt.m_flag = 0;

    for (int i = 0; i <= 3; ++i)
    {
        CUser* member = party->get_user(i);
        if (member != 0)
        {
            pkt.m_uids[i] = member->GetUID();
            pkt.m_characNos[i] = member->get_charac_no(-1);
            if (GlobalData::s_villageMonsterMgr->OnEvent() && flag)
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

}  // namespace village_attacked
