// df_game_r 还原 —— village_attacked::CVillageMonsterMgr
// 独立 TU。OnKillVillageMonster 由 VillageMonster_helpers.cpp 提供（不入本批）。
// OnMoveArea / InsertVillageArea 语义依据 ORIG 0x86b45bc / 0x86b4144；
// CVillageMonsterArea ctor/OnMoveArea 依据 ORIG 0x86b3752 / 0x86b3abe。

#include "CVillageMonsterMgr.h"
#include "CUser.h"
#include "CUserCharacInfo.h"

namespace village_attacked
{

CVillageMonsterArea::CVillageMonsterArea(int a, int b, int c, int d,
                                         std::vector<STAttackedMonster>* monsters,
                                         std::vector<MapArea>* areas)
{
    // ORIG 0x86b3752：map/vector 默认构造后按偏移赋值。
    m_field28 = a;                       // +0x28
    m_field2c = b;                       // +0x2c
    m_monsterList = *monsters;           // +0x1c vector 拷贝
    m_areaList = areas;                  // +0x18
    m_field34 = c;                       // +0x34
    m_field38 = d;                       // +0x38
    m_field30 = 0;                       // +0x30
}

void CVillageMonsterArea::OnMoveArea(CUser* user)
{
    if (!m_monsters.empty())
        SendVillageMonsterList(user);    // ORIG 0x86b3abe（列表发送后续批次实现）
}

void CVillageMonsterMgr::OnMoveArea(CUser* user)
{
    // ORIG 0x86b45bc：Zone(curVill, curArea) 查找，命中则转交区域处理。
    unsigned char area = (unsigned char)user->get_area(false);
    unsigned char vill = user->getCurCharacVill();
    Zone zone(vill, area);
    std::map<Zone, CVillageMonsterArea>::iterator it = m_areas.find(zone);
    if (it != m_areas.end())
        it->second.OnMoveArea(user);
}

bool CVillageMonsterMgr::OnEvent()
{
    // ORIG 0x82f0f44：返回 bool(m_field04 != 0)
    return m_field04 != 0;
}

void CVillageMonsterMgr::InsertVillageMonster(int a, int b, int c, int d,
                                              std::vector<STAttackedMonster>* monsters,
                                              std::vector<MapArea>* areas)
{
    // ORIG 0x86b4144：构造区域后以 Zone(a,b) 为键插入 m_areas。
    CVillageMonsterArea area(a, b, c, d, monsters, areas);
    Zone zone((unsigned char)a, (unsigned char)b);
    m_areas.insert(std::make_pair(zone, area));
}

}  // namespace village_attacked
