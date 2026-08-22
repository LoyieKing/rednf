// df_game_r 还原 —— village_attacked 域权威头（从 GameStubs.cpp / VillageMonster_helpers.cpp 合并）
// 唯一声明点：CVillageMonsterMgr.cpp / VillageMonster_helpers.cpp / Area.cpp / CBattle_Field 域共用。
// 注意：STAttackedMonster 与 MapArea 必须在全局作用域声明（ORIG mangled InsertVillageMonster
// 使用非限定的 17STAttackedMonsterE / 7MapAreaE），不能放进 village_attacked 命名空间。

#ifndef VILLAGE_ATTACKED_CVILLAGEMONSTERMGR_H
#define VILLAGE_ATTACKED_CVILLAGEMONSTERMGR_H

#include <map>
#include <vector>

class CUser;
class PacketGuard;

// ORIG 反汇编关联：STAttackedMonster 0x20 字节。
struct STAttackedMonster
{
    char m_pad[0x20];
};

// ORIG 反汇编关联：MapArea 0x10 字节（int m_x/m_y/m_w/m_h；Area 的传送区域矩形）。
struct MapArea
{
    int m_x;     // +0x00
    int m_y;     // +0x04
    int m_w;     // +0x08
    int m_h;     // +0x0c
};

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

    int  OnKillVillageMonster(int a, int b, unsigned short c, bool d);
                                                      // _ZN16village_attacked18CVillageMonsterMgr20OnKillVillageMonsterEiitb
                                                      // ORIG 0x086b4964（调用点 CBattle_Field::reset_field）；
                                                      // 定义见 VillageMonster_helpers.cpp
    void OnMoveArea(CUser* user);                    // _ZN16village_attacked18CVillageMonsterMgr10OnMoveAreaEP5CUser (0x86b45bc)
    bool OnEvent();                                   // _ZN16village_attacked18CVillageMonsterMgr7OnEventEv (0x82f0f44)
                                                      // ORIG 0x82f0f44 反汇编：返回 bool(m_field04 != 0)
    void InsertVillageMonster(int a, int b, int c, int d,
                              std::vector<STAttackedMonster>* monsters,
                              std::vector<MapArea>* areas);  // _ZN16village_attacked18CVillageMonsterMgr20InsertVillageMonsterEiiiiPSt6vectorI17STAttackedMonsterSaIS2_EEPS1_I7MapAreaSaIS6_EE (0x86b4144)
};

}  // namespace village_attacked

#endif  // VILLAGE_ATTACKED_CVILLAGEMONSTERMGR_H
