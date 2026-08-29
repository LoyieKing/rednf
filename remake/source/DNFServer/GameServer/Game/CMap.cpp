 #include <algorithm>
 #include <new>

 // cElection<int,V,C> 的唯一实现归属地图/选举 TU（声明点 PvPTypes.h，ORIG 布局）。
 // cParty.cpp 的 cElection<int,4,4>（m_padElection@+0xd8）与 PvP_Room.cpp 的
 // cElection<int,8,8>（+0x12c）经本 TU 提供真实符号。
 #include "PvPTypes.h"
// GetElected 共同语义（ORIG 0x85dff78 / 0x85c03a8）：对票矩阵按列求和，
// 返回票数最大列下标；全零票返回 -1。以宏展开避免提前隐式实例化。
#define CELECTION_GET_ELECTED(V, C)                                          \
    do                                                                       \
    {                                                                        \
        int tally[C] = { 0 };                                                \
        for (int i = 0; i <= (V) - 1; ++i)                                   \
            for (int j = 0; j <= (C) - 1; ++j)                               \
                tally[j] += m_voters[i].m_votes[j];                          \
        int best = -1;                                                       \
        int bestVal = 0;                                                     \
        for (int i = 0; i <= (C) - 1; ++i)                                   \
        {                                                                    \
            if (tally[i] > bestVal)                                          \
            {                                                                \
                bestVal = tally[i];                                          \
                best = i;                                                    \
            }                                                                \
        }                                                                    \
        return best;                                                         \
    } while (0)
// 先声明全部显式特化，避免定义顺序触发隐式实例化在前（GCC 4.4）。
template <> void cElection<int, 8, 8>::cVoter::Init();
template <> cElection<int, 8, 8>::cVoter::cVoter();
template <> cElection<int, 8, 8>::cElection();
template <> void cElection<int, 8, 8>::Init();
template <> void cElection<int, 8, 8>::Vote(int voter, int candidate);
template <> void cElection<int, 8, 8>::CancelVoter(int voter);
template <> void cElection<int, 8, 8>::CancelCandidate(int candidate);
template <> int cElection<int, 8, 8>::GetElected() const;
template <> void cElection<int, 4, 4>::cVoter::Init();
template <> cElection<int, 4, 4>::cVoter::cVoter();
template <> cElection<int, 4, 4>::cElection();
template <> void cElection<int, 4, 4>::Init();
template <> void cElection<int, 4, 4>::Vote(int voter, int candidate);
template <> void cElection<int, 4, 4>::CancelVoter(int voter);
template <> void cElection<int, 4, 4>::CancelCandidate(int candidate);
template <> int cElection<int, 4, 4>::GetElected() const;


template <> void cElection<int, 8, 8>::cVoter::Init()
{
    std::fill(m_votes, m_votes + 8, 0);   // ORIG 0x85e0304
}
template <> cElection<int, 8, 8>::cVoter::cVoter() { Init(); }
template <> cElection<int, 8, 8>::cElection()
{
    for (int i = 7; i >= 0; --i)          // ORIG 0x85dff3a：倒序逐 voter 构造
        new (&m_voters[i]) cVoter();
    Init();                               // ORIG 0x85e02d2
}
template <> void cElection<int, 8, 8>::Init()
{
    for (int i = 0; i <= 7; ++i)
        m_voters[i].Init();
}
template <> void cElection<int, 8, 8>::Vote(int voter, int candidate)
{
    m_voters[voter].m_votes[candidate] = 1;
}
template <> void cElection<int, 8, 8>::CancelVoter(int voter)
{
    m_voters[voter].Init();
}
template <> void cElection<int, 8, 8>::CancelCandidate(int candidate)
{
    for (int i = 0; i <= 7; ++i)
        m_voters[i].m_votes[candidate] = 0;
}
template <> int cElection<int, 8, 8>::GetElected() const   // ORIG 0x85dff78
{
    CELECTION_GET_ELECTED(8, 8);
}

template <> void cElection<int, 4, 4>::cVoter::Init()
{
    std::fill(m_votes, m_votes + 4, 0);   // ORIG 0x82beb62
}
template <> cElection<int, 4, 4>::cVoter::cVoter() { Init(); }
template <> cElection<int, 4, 4>::cElection()
{
    for (int i = 3; i >= 0; --i)
        new (&m_voters[i]) cVoter();
    Init();
}
template <> void cElection<int, 4, 4>::Init()
{
    for (int i = 0; i <= 3; ++i)
        m_voters[i].Init();
}
template <> void cElection<int, 4, 4>::Vote(int voter, int candidate)
{
    m_voters[voter].m_votes[candidate] = 1;   // ORIG 0x85c038c
}
template <> void cElection<int, 4, 4>::CancelVoter(int voter)
{
    m_voters[voter].Init();
}
template <> void cElection<int, 4, 4>::CancelCandidate(int candidate)  // ORIG 0x85c047c
{
    for (int i = 0; i <= 3; ++i)
        m_voters[i].m_votes[candidate] = 0;
}
template <> int cElection<int, 4, 4>::GetElected() const   // ORIG 0x85c03a8
{
    CELECTION_GET_ELECTED(4, 4);
}
 // df_game_r Game/ CMap 实现（G2 地图链）。
 // 逻辑依据 docs/class_func_reports/CMap.md；成员访问统一使用语义字段。
 #include <cstring>
 #include "CMap.h"
 #include "CDataManager.h"   // G_CDataManager()->m_stageTimeLine（set_map）
// isInArea / makePathArea / isInPathArea —— ORIG 全局函数（mangled _Z8isInAreaiiP7MapArea
// _Z12makePathAreaP7MapAreaPA2_KiS3_ _Z12isInPathAreaiiPA2_KiS1_）。
// 语义（报告 08891c18 / 0889203b / 08891f20）：
//   makePathArea(areas, area1(坐标), area2(宽高))：宽/高取自 area2（≤0 用默认值），
//   坐标取 area1 并裁剪到区域内；isInArea 为严格半开区间 [x0,x0+w)×[y0,y0+h)。
// 默认为第 0/1 区 w=0x78 h=0x6a；第 2/3 区 w=0x8c h=0x28。
// 实现地点：CMap.cpp（本域）；MapArea 权威定义见 CVillageMonsterMgr.h。

int isInArea(int x, int y, MapArea* area)
{
    if (x < area->m_x)
        return 0;
    if (x >= area->m_x + area->m_w)
        return 0;
    if (y < area->m_y)
        return 0;
    if (y >= area->m_y + area->m_h)
        return 0;
    return 1;
}

void makePathArea(MapArea* areas, const int area1[][2], const int area2[][2])
{
    for (int i = 0; i < 4; ++i)
    {
        if (i == 0 || i == 1)
        {
            areas[i].m_w = area2[i][0] > 0 ? area2[i][0] : 0x78;
            areas[i].m_h = area2[i][1] > 0 ? area2[i][1] : 0x6a;
            if (i == 0)
                areas[i].m_x = area1[i][0];
            else
                areas[i].m_x = area1[i][0] - areas[i].m_w;
            areas[i].m_y = area1[i][1] - areas[i].m_h / 2;
        }
        else
        {
            areas[i].m_w = area2[i][0] > 0 ? area2[i][0] : 0x8c;
            areas[i].m_h = area2[i][1] > 0 ? area2[i][1] : 0x28;
            areas[i].m_x = area1[i][0] - areas[i].m_w / 2;
            areas[i].m_y = area1[i][1] - areas[i].m_h / 2;
        }
    }
}

int isInPathArea(int x, int y, const int (*area1)[2], const int (*area2)[2])
{
    MapArea areas[4];
    makePathArea(areas, area1, area2);
    for (int i = 0; i < 4; ++i)
    {
        if (isInArea(x, y, &areas[i]))
            return 1;
    }
    return 0;
}
// _mapItem / _mapMonster 生命周期 —— 唯一定义点（声明见 CMap.h）。
// ============================================================================

// _mapItem ctor —— ORIG 0x0815088e：仅清零 +0x04/+0x08/+0x0c，不初始化 m_objectIdx。
_mapItem::_mapItem()
{
    m_field04 = 0;
    m_field08 = 0;
    m_field0c = 0;
}
// ORIG 无 _mapItem dtor 符号：成员全为 int，平凡析构由编译器内联生成，勿显式定义。

// _mapMonster ctor —— ORIG 0x0830e4fc：vector 默认构造后逐字段赋值；
// m_level(+0x24) 按 ORIG 保持未初始化。
_mapMonster::_mapMonster()
{
    m_mobId = -1;
    m_flag04 = 1;
    m_field08 = 0;
    m_spawnType = 0;
    m_levelAdd = 0;
    m_field14 = 0;
    m_roleType = 0;
    m_champion = 0;
    m_posX = 0;
    m_hpPct = 100;
    m_orderIdx = 0;
    m_flag30 = 0;
}

// _mapMonster 拷贝构造 —— ORIG 0x0813d708：逐成员拷贝 +0x00..+0x30
// （不含 +0x31 pad），再拷贝构造 +0x34 vector。
_mapMonster::_mapMonster(const _mapMonster& other)
    : m_mobId(other.m_mobId),
      m_flag04(other.m_flag04),
      m_field08(other.m_field08),
      m_spawnType(other.m_spawnType),
      m_levelAdd(other.m_levelAdd),
      m_field14(other.m_field14),
      m_roleType(other.m_roleType),
      m_champion(other.m_champion),
      m_posX(other.m_posX),
      m_level(other.m_level),
      m_hpPct(other.m_hpPct),
      m_orderIdx(other.m_orderIdx),
      m_flag30(other.m_flag30),
      m_mobList(other.m_mobList)
{
}

// _mapMonster dtor —— ORIG 0x081398c6：仅析构 +0x34 vector，无其它动作。
_mapMonster::~_mapMonster() {}


CMap::CMap()
{
    // ORIG 0834dcda：容器构造后清 this+0(m_bossCount) 与 this+0x7c(m_assignedNpcCount)。
    // 写在 body 使其位于容器构造之后（匹配 ORIG movl $0,(%eax)/movl $0,0x7c(%eax) 顺序）。
    m_bossCount = 0;
    m_assignedNpcCount = 0;
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
    // ORIG 0834ed1e：空体（push/mov/pop/ret，仅返回）。
}

bool CMap::checkFitInPathObject(int x, int y) const
{
    return isInPathArea(x, y,
                        reinterpret_cast<const int (*)[2]>(m_pathArea1),
                        reinterpret_cast<const int (*)[2]>(m_pathArea2));
}

int CMap::insert_item(int idx, int level, int prob, int cond)
{
    _mapItem item;
    item.m_objectIdx = idx;
    item.m_field04 = level;
    item.m_field08 = prob;
    item.m_field0c = cond;
    m_itemList.push_back(item);
    return 1;
}

int CMap::insert_monster(STAssignMonster monster, int area)
{
    // ORIG 0x0834e9d4：逐字段拷贝（顺序：mobId/flag04/field08/spawnType(m_20)/
    // levelAdd(m_1c)/field14(m_18)/roleType(m_24)/level(m_14/10)/orderIdx(m_50)/
    // hpPct(m_4c)/flag30(m_54)/mobList(m_vec28)）；area!=-1 时 m_champion=1,
    // m_posX=area（0834ea80/0834ea84）。
    _mapMonster item;
    item.m_mobId = monster.m_0;
    item.m_flag04 = monster.m_4;
    item.m_field08 = monster.m_8;
    item.m_spawnType = monster.m_20;
    item.m_levelAdd = monster.m_1c;
    item.m_field14 = monster.m_18;
    item.m_roleType = monster.m_24;
    item.m_level = monster.m_14 / 10;
    item.m_orderIdx = monster.m_50;
    item.m_hpPct = monster.m_4c;
    item.m_flag30 = monster.m_54;
    item.m_mobList = monster.m_vec28;
    if (area != -1)
    {
        item.m_champion = 1;
        item.m_posX = area;
    }
    m_monsterList.push_back(item);
    return 1;
}

void CMap::insert_condition_monster(STAssignMonster monster)
{
    // ORIG 0834ead4：仅拷贝 mobId/flag04/field08/spawnType(m_20)/levelAdd(m_1c)/
    // field14(m_18)/roleType(m_24)/level(m_14/10)/orderIdx(m_50)。
    // 不拷贝 m_hpPct/m_flag30/m_mobList/m_champion/m_posX（保持 ctor 默认）。
    _mapMonster item;
    item.m_mobId = monster.m_0;
    item.m_flag04 = monster.m_4;
    item.m_field08 = monster.m_8;
    item.m_spawnType = monster.m_20;
    item.m_levelAdd = monster.m_1c;
    item.m_field14 = monster.m_18;
    item.m_roleType = monster.m_24;
    item.m_level = monster.m_14 / 10;
    item.m_orderIdx = monster.m_50;
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
    m_4 = script.m_index;
    m_8 = script.m_c;
    m_80 = script.m_championRate;
    m_entranceVec = script.m_recoveryObjects;
    m_pathObjectList = script.m_list4;
    m_bloodSystem = script.m_vecBloodSystem;
    m_bloodPhaseTime = script.m_vecBloodPhaseTime;
    m_b4 = script.m_maxMonsterDropException;
    std::memcpy(m_pathArea1, script.m_pathgatePos, sizeof(m_pathArea1));
    std::memcpy(m_pathArea2, script.m_pathgateRange, sizeof(m_pathArea2));

    for (std::vector<STAssignMonster>::const_iterator it = script.m_vecMonster.begin();
         it != script.m_vecMonster.end(); ++it)
    {
        STAssignMonster monster = *it;
        if (m_8 == 2 && monster.m_14 == 3)
            monster.m_14 = 1;
        if (monster.m_14 == 3)
            ++m_bossCount;
        insert_monster(monster, -1);
    }

    for (std::vector<STAssignMonster>::const_iterator it = script.m_vecConditionMonster.begin();
         it != script.m_vecConditionMonster.end(); ++it)
    {
        insert_condition_monster(*it);
    }

    int area = 0;
    for (std::list<STAssignSpecialPassiveObj>::const_iterator obj = script.m_listSpecialPassive.begin();
         obj != script.m_listSpecialPassive.end(); ++obj, ++area)
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

    m_rivalData1 = script.m_vecMonsterSpawnPos;
    m_rivalData2 = script.m_vecEventMonsterPos;
    m_78 = script.m_apcRandomPoint;
    m_aiCharacters = script.m_vecAICharacter;
    m_assignedNpcCount = static_cast<int>(script.m_vecNpc.size());
    for (std::list<STAssignPassiveObject>::const_iterator it = script.m_listPassiveObject.begin();
         it != script.m_listPassiveObject.end(); ++it)
    {
        m_passiveObjectList.push_back(it->m_0);
    }
    m_ec = script.m_maxMonsterCountException;
    m_f0 = script.m_328;
    m_timeLine = script.m_timeLine;
    m_110 = script.m_178;
    // ORIG 0x081e1a2a：G_CDataManager()->m_stageTimeLine.checkAndSetActionDefine(
    //   m_timeLine, m_stageTimeLine.m_actionMap)；返回值未使用。
    G_CDataManager()->m_stageTimeLine.checkAndSetActionDefine(
        m_timeLine, G_CDataManager()->m_stageTimeLine.m_actionMap);
    m_tournamentMap = script.m_tournamentMonsters;
    m_12c = script.m_fixChampion;
    return 1;
}

