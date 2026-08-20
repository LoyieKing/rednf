#include "GameRand.h"

// cElection<int,8,8> 的唯一实现归属地图/选举 TU。
template <typename T, int A, int B> class cElection { public: cElection(); int GetElected() const; void Vote(int, int); void CancelVoter(int); void CancelCandidate(int); private: int m_elected; };
template <> cElection<int,8,8>::cElection() : m_elected(-1) {}
template <> int cElection<int,8,8>::GetElected() const { return m_elected; }
template <> void cElection<int,8,8>::Vote(int, int candidate) { m_elected = candidate; }
template <> void cElection<int,8,8>::CancelVoter(int) {}
template <> void cElection<int,8,8>::CancelCandidate(int candidate) { if (m_elected == candidate) m_elected = -1; }

// df_game_r Game/ CMap 实现（G2 地图链）。
// 逻辑依据 docs/class_func_reports/CMap.md；成员访问统一使用语义字段。
#include <cstring>
#include "CMap.h"

struct CMapPathArea
{
    int m_x;
    int m_y;
    int m_width;
    int m_height;
};

static bool isInArea(int x, int y, const CMapPathArea& area)
{
    return x > area.m_x && x <= area.m_x + area.m_width &&
           y > area.m_y && y <= area.m_y + area.m_height;
}

static void makePathArea(CMapPathArea areas[4],
                         const int area1[][2], const int area2[][2])
{
    static const int defaults[4] = { 0x78, 0x6a, 0x8c, 0x14 };
    for (int i = 0; i < 4; ++i)
    {
        const bool firstHalf = i < 2;
        const int defaultWidth = firstHalf ? defaults[0] : defaults[2];
        const int defaultHeight = firstHalf ? defaults[1] : defaults[3];
        areas[i].m_width = area1[i][0] > 0 ? area1[i][0] : defaultWidth;
        areas[i].m_height = area1[i][1] > 0 ? area1[i][1] : defaultHeight;
        if (firstHalf)
        {
            areas[i].m_x = i == 0 ? area2[i][0]
                                  : area2[i][0] - areas[i].m_width;
            areas[i].m_y = area2[i][1] > 0
                ? area2[i][1] - areas[i].m_height / 2
                : defaultHeight;
        }
        else
        {
            areas[i].m_x = area2[i][0] - areas[i].m_width / 2;
            areas[i].m_y = area2[i][1] > 0
                ? area2[i][1] - areas[i].m_height / 2
                : 0x28;
        }
    }
}

int isInPathArea(int x, int y, const int (*area1)[2], const int (*area2)[2])
{
    CMapPathArea areas[4];
    makePathArea(areas, area1, area2);
    for (int i = 0; i < 4; ++i)
    {
        if (isInArea(x, y, areas[i]))
            return 1;
    }
    return 0;
}
namespace
{
int monsterLevel(const STAssignMonster& monster)
{
    return monster.m_14 / 10;
}

void makeMapMonster(_mapMonster& out, const STAssignMonster& in)
{
    out.m_0 = in.m_0;
    out.m_4 = in.m_4;
    out.m_8 = in.m_8;
    out.m_20 = in.m_20;
    out.m_1c = in.m_1c;
    out.m_18 = in.m_18;
    out.m_14 = in.m_14;
    out.m_24 = monsterLevel(in);
    out.m_28 = in.m_4c;
    out.m_2c = in.m_50;
    out.m_30 = in.m_54;
    out.m_vec = in.m_vec28;
}
}

_mapMonster::_mapMonster()
    : m_mobId(0), m_flag04(0), m_level(0), m_spawnType(0),
      m_levelAdd(0), m_field14(0), m_roleType(0), m_champion(0),
      m_posX(0), m_pad24(0), m_hpPct(0), m_orderIdx(0)
{
}

_mapMonster::~_mapMonster()
{
}

CMap::CMap()
    : m_bossCount(0), m_assignedNpcCount(0)
{
}

int CMap::getAssignedNPCCount() const
{
    return m_assignedNpcCount;
}

int CMap::GetBossCount() const
{
    return m_bossCount;
}

std::list<_mapMonster> CMap::get_monster_list() const
{
    return m_monsterList;
}

std::list<_mapItem> CMap::get_item_list() const
{
    return m_itemList;
}

CMap::~CMap()
{
    destroy();
}

void CMap::destroy()
{
    m_monsterList.clear();
    m_itemList.clear();
    m_bossCount = 0;
    m_pathObjectList.clear();
    m_rivalData2.clear();
}

void CMap::destory()
{
    destroy();
}

int CMap::checkFitInPathObject(int x, int y) const
{
    return isInPathArea(x, y,
                        reinterpret_cast<const int (*)[2]>(m_pathArea1),
                        reinterpret_cast<const int (*)[2]>(m_pathArea2));
}

int CMap::insert_item(int idx, int level, int prob, int cond)
{
    _mapItem item;
    item.m_0 = idx;
    item.m_4 = level;
    item.m_8 = prob;
    item.m_c = cond;
    m_itemList.push_back(item);
    return 1;
}

int CMap::insert_monster(STAssignMonster monster, int area)
{
    _mapMonster item;
    makeMapMonster(item, monster);
    if (area != -1)
    {
        item.m_30 = 1;
        item.m_2c = area;
    }
    m_monsterList.push_back(item);
    return 1;
}

void CMap::insert_condition_monster(STAssignMonster monster)
{
    _mapMonster item;
    item.m_0 = monster.m_0;
    item.m_4 = monster.m_4;
    item.m_8 = monster.m_8;
    item.m_20 = monster.m_20;
    item.m_1c = monster.m_1c;
    item.m_18 = monster.m_18;
    item.m_14 = monster.m_14;
    item.m_24 = monsterLevel(monster);
    item.m_2c = monster.m_50;
    m_conditionMonsterList.push_back(item);
}

int CMap::box_item_index(std::list<std::pair<int, int> > itemList) const
{
    int threshold = get_rand_int(100) + 1;
    int accumulated = 0;
    std::list<std::pair<int, int> >::const_iterator it = itemList.begin();
    for (; it != itemList.end(); ++it)
    {
        accumulated += it->second;
        if (threshold < accumulated)
            return it->first;
    }
    return -2;
}

int CMap::CheckEntrance(int flag) const
{
    for (unsigned int i = 0; i < m_entranceVec.size(); ++i)
    {
        if ((m_entranceVec[i].second & flag) != 0)
            return 1;
    }
    return 0;
}
int CMap::set_map(STMapScript script)
{
    m_4 = script.m_0;
    m_8 = script.m_c;
    m_80 = script.m_4c;
    m_entranceVec = script.m_vec10;
    m_pathObjectList = script.m_list4;
    m_bloodSystem = script.m_vec288;
    m_bloodPhaseTime = script.m_vec294;
    m_b4 = script.m_284;
    std::memcpy(m_pathArea1, script.m_pathArea1, sizeof(m_pathArea1));
    std::memcpy(m_pathArea2, script.m_pathArea2, sizeof(m_pathArea2));

    for (std::vector<STAssignMonster>::const_iterator it = script.m_vec34.begin();
         it != script.m_vec34.end(); ++it)
    {
        STAssignMonster monster = *it;
        if (m_8 == 2 && monster.m_14 == 3)
            monster.m_14 = 1;
        if (monster.m_14 == 3)
            ++m_bossCount;
        insert_monster(monster, -1);
    }

    for (std::vector<STAssignMonster>::const_iterator it = script.m_vec40.begin();
         it != script.m_vec40.end(); ++it)
    {
        insert_condition_monster(*it);
    }

    int area = 0;
    for (std::list<STAssignSpecialPassiveObj>::const_iterator obj = script.m_list58.begin();
         obj != script.m_list58.end(); ++obj, ++area)
    {
        for (std::vector<SPOBJ_ACTION_DATAS>::const_iterator action = obj->m_vec10.begin();
             action != obj->m_vec10.end(); ++action)
        {
            if (action->m_type == 0)
            {
                if (action->m_10 != 0 &&
                    get_rand_int(100) <= (action->m_c * 100) / action->m_10 &&
                    action->m_8 > 0 && action->m_8 < 0xc9)
                {
                    STAssignMonster monster;
                    monster.m_0 = action->m_4;
                    monster.m_14 = action->m_8;
                    insert_monster(monster, area);
                }
            }
            else if (action->m_type == 2)
            {
                insert_item(action->m_4, action->m_8, action->m_c, obj->m_1c);
            }
        }
    }

    m_rivalData1 = script.m_vec250;
    m_rivalData2 = script.m_vec278;
    m_78 = script.m_268;
    m_aiCharacters = script.m_vec25c;
    m_assignedNpcCount = static_cast<int>(script.m_vec60.size());
    for (std::list<STAssignPassiveObject>::const_iterator it = script.m_list2c.begin();
         it != script.m_list2c.end(); ++it)
    {
        m_passiveObjectList.push_back(it->m_0);
    }
    m_ec = script.m_324;
    m_f0 = script.m_328;
    m_timeLine = script.m_timeLine330;
    m_110 = script.m_178;
    m_tournamentMap = script.m_map2b8;
    m_12c = script.m_36e;
    return 1;
}

