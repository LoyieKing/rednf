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
#include "InterfacePacketBuf.h"
#include "TimerQueue.h"

// ============================================================================
// 外部符号（asm-label 直引；避免引入 GlobalData.h/CMonitorServerProxy.h 等头）
// ============================================================================
extern "C" void* GlobalData_s_villageMonsterMgr
    asm("_ZN10GlobalData19s_villageMonsterMgrE");   // 0941f77c
extern "C" void* GlobalData_s_monitor_proxy_mgr
    asm("_ZN10GlobalData19s_monitor_proxy_mgrE");   // 0940be28
extern "C" void* sub_CServerProxyMgr_GetServerProxy_Monitor(void* self, int group)
    asm("_ZN15CServerProxyMgrI19CMonitorServerProxyE14GetServerProxyE17ENUM_SERVER_GROUP");
extern "C" void* sub_G_GameWorld(void) asm("_Z11G_GameWorldv");
// ORIG 该符号返回 bool(m_field04 != 0)；GameStubs_remaining.cpp 当前为 void stub
// （TODO G3），此处按 ORIG 调用语义声明为 char。
extern "C" char sub_village_Mgr_OnEvent(void* self)
    asm("_ZN16village_attacked18CVillageMonsterMgr7OnEventEv");

TimerQueue* G_TimerQueue();

// PacketGuard（与 CBattle_Field.cpp 同布局 0xc 字节；ctor/dtor 由 CInventory.cpp 提供）
class PacketGuard : public InterfacePacketBuf
{
public:
    PacketGuard();
    ~PacketGuard();

private:
    int m_nField4;
    char m_cField8;
};

struct STAttackedMonster { char m_pad[0x20]; };
struct MapArea { char m_pad[0x10]; };

namespace village_attacked
{

// ---- Zone（ORIG ctor 0x86b4b3c：两个 unsigned char；map key） ----
struct Zone
{
    Zone(unsigned char a, unsigned char b) : m_a(a), m_b(b) {}
    unsigned char m_a;
    unsigned char m_b;
};

inline bool operator<(const Zone& x, const Zone& y)
{
    if (x.m_a != y.m_a)
        return x.m_a < y.m_a;
    return x.m_b < y.m_b;
}

// ORIG 枚举 tag：_ZN16village_attacked26ENUM_VILLAGE_MONSTER_STATEE
enum ENUM_VILLAGE_MONSTER_STATE
{
    ENUM_VILLAGE_MONSTER_STATE_0 = 0,
    ENUM_VILLAGE_MONSTER_STATE_1 = 1,
    ENUM_VILLAGE_MONSTER_STATE_2 = 2
};

struct STAttackedMonster
{
    char m_pad[0x20];
};

class CVillageMonsterArea;

// ---- CVillageMonster（布局 ORIG ctor 0x86b3098/0x86b3004 实证，0x28 字节） ----
class CVillageMonster
{
public:
    unsigned short m_id;                 // +0x00
    unsigned short m_zone;               // +0x02
    int m_field04;                       // +0x04
    int m_field08;                       // +0x08
    int m_field0c;                       // +0x0c
    char m_field10;                      // +0x10
    int m_state;                         // +0x14
    int m_partyIndex;                    // +0x18
    CVillageMonsterArea* m_area;         // +0x1c
    int m_field20;                       // +0x20
    int m_field24;                       // +0x24

    int OnKillVillageMonster(CUser* user, int a, int b, int c, bool flag);
    void OnChangeState(ENUM_VILLAGE_MONSTER_STATE state);
    void SendVillageMonsterFightResult(CUser* user, bool flag);
};

// ---- CVillageMonsterArea（布局 ORIG ctor 0x86b3752 实证，0x3c 字节） ----
class CVillageMonsterArea
{
public:
    std::map<unsigned short, CVillageMonster> m_monsters;  // +0x00 (0x18)
    void* m_areaList;                 // +0x18（ctor 参数 6：vector<MapArea>*）
    std::vector<STAttackedMonster> m_monsterList;  // +0x1c (0x14)
    int m_field28;                    // +0x28
    int m_field2c;                    // +0x2c
    int m_field30;                    // +0x30
    int m_field34;                    // +0x34
    int m_field38;                    // +0x38

    int OnKillVillageMonster(CUser* user, unsigned short zone, bool flag);
    void SendPacket(PacketGuard& packet);
};

// ---- CVillageMonsterMgr（布局 ORIG ctor 0x86b4a4c 实证，0x24 字节） ----
class CVillageMonsterMgr
{
public:
    int m_field00;                     // +0x00
    int m_field04;                     // +0x04
    std::map<Zone, CVillageMonsterArea> m_areas;  // +0x08 (0x18)
    int m_field20;                     // +0x20

    int OnKillVillageMonster(int a, int b, unsigned short c, bool d);
    void OnMoveArea(CUser*);
    void OnEvent();
    void InsertVillageMonster(int, int, int, int, std::vector<STAttackedMonster>*, std::vector<MapArea>*);
};

// ============================================================================
// CVillageMonsterMgr::OnKillVillageMonster(int,int,unsigned short,bool)
//   —— ORIG 0x86b4964
//   objdump: Zone zone((char)a,(char)b); m_areas.find(zone);
//            it == end -> return 0x15
//            return it->second.OnKillVillageMonster(0, c, d);
// ============================================================================
void CVillageMonsterMgr::OnMoveArea(CUser*) {}
void CVillageMonsterMgr::OnEvent() {}
void CVillageMonsterMgr::InsertVillageMonster(int, int, int, int, std::vector<STAttackedMonster>*, std::vector<MapArea>*) {}

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
    sub_GameWorld_send_to_area(sub_G_GameWorld(), m_field28, m_field2c, &packet);
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
            if (sub_village_Mgr_OnEvent(GlobalData_s_villageMonsterMgr) && flag)
                member->IncVillageAttackCount();
        }
    }
    pkt.m_flag = flag;

    void* monitorProxy = sub_CServerProxyMgr_GetServerProxy_Monitor(
        GlobalData_s_monitor_proxy_mgr, user->GetServerGroup());
    sub_CMonitorServerProxy_SendTcpPacket(monitorProxy, (char*)&pkt, 0x2b);

    PacketGuard pg;
    pg.clear();
    pg.put_header(0, 0xf9);
    pg.put_byte(flag);
    pg.finalize(true);
    party->send_to_party(pg);
}

}  // namespace village_attacked
