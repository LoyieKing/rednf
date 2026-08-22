#include "GameRand.h"
// df_game_r 还原 —— CDungeon / CDungeon_TowerOfDespair / STDungeonScript
//（G2 副本链）。
// 逐函数对照 docs/class_func_reports/CDungeon.md、CDungeon_TowerOfDespair.md
// 与 ORIG df_game_r 反汇编实现；目标 identical 口径。
// 支撑类型（MazeScript/stDungeonAssignItem_t/STWarRoomData/...）的 ORIG W 符号
// 也在本 TU 内实现。
#include "CDungeon.h"

#include <string.h>
#include <stdlib.h>

#include <algorithm>

#include "CMap.h"
#include "GameTypes.h"
#include "LogManager.h"
#include "DNFFunctionLib.h"

// ============================================================================
// 支撑类型：GridScript / RidableObject / RidableScript / DungeonClearCondition
// ============================================================================

// ============================================================================
// CDungeon 弱访问器 / 自包含逻辑（ORIG 地址见各函数注释；
// ctor/dtor/set_dungeon 及 GameStubs.cpp 已有桩的方法不在此重复定义）
// ============================================================================

int CDungeon::GetType_DungeonInHeritance() const
{
    return m_flag4;
}

int CDungeon::getDungeonKind() const
{
    return m_670;
}

int CDungeon::isNoFatigueDungeon() const
{
    return m_6c8;
}

int CDungeon::get_blood_dungeon_type() const
{
    return m_89f;
}

int CDungeon::getQuestNpcDungeon() const
{
    return m_8fc;
}

int CDungeon::get_limit_party_count() const
{
    return m_3b;
}

int CDungeon::get_limit_Inout_count() const
{
    return m_3c;
}

int CDungeon::limitOfStackableItemInTower() const
{
    return m_6b4;
}

int CDungeon::getHeroesRequiredQuest() const
{
    return m_34;
}

int CDungeon::getAICharacterAppearRate() const
{
    return m_650;
}

int CDungeon::getRandomTowerMaxStage() const
{
    return m_654;
}

int CDungeon::getDeathTowerMaxStage() const
{
    return m_680;
}

int CDungeon::getDeathTowerMapIndex(unsigned int idx) const
{
    return m_vec674[idx];
}

int CDungeon::getTowerMaxClearRewardItemNum() const
{
    return m_6b8;
}

int CDungeon::getEventMonsterTotalProb() const
{
    return m_68c;
}

int CDungeon::getTryGenEventMonsterCnt() const
{
    return m_684;
}

int CDungeon::getTryGenEventMonsterProb() const
{
    return m_688;
}

int CDungeon::getSecondEventMonsterTotalProb() const
{
    return m_6a4;
}

int CDungeon::getTryGenSecondEventMonsterCnt() const
{
    return m_69c;
}

int CDungeon::getTryGenSecondEventMonsterProb() const
{
    return m_6a0;
}

int CDungeon::hasTowerItemDrop() const
{
    return m_6b5;
}

int CDungeon::GetDummyAppearCount() const
{
    return m_628;
}

int CDungeon::IsHaveDummyMap() const
{
    return m_624;
}

int CDungeon::isRiskDungeon() const
{
    return m_6c9;
}

bool CDungeon::IsOpenEvenEnemy() const
{
    return m_6d1;
}

int CDungeon::GetPartyMemberCoinLimit() const
{
    return m_6cc;
}

float CDungeon::get_exp_weight() const
{
    return m_14;
}
std::vector<std::pair<int, int> >* CDungeon::getDropItems() const
{
    return const_cast<std::vector<std::pair<int, int> >*>(&m_vec40);
}
std::vector<std::pair<int, int> >* CDungeon::getObjectDropItems() const
{
    return const_cast<std::vector<std::pair<int, int> >*>(&m_vec4c);
}

std::vector<stDungeonAssignItem_t>* CDungeon::getSpecialPassiveObjectItems()
    const
{
    return const_cast<std::vector<stDungeonAssignItem_t>*>(&m_vec58);
}

std::vector<stEventMonster_t>* CDungeon::getEventMonsters() const
{
    return const_cast<std::vector<stEventMonster_t>*>(&m_eventMonsters);
}

std::vector<stSecondEventMonster_t>* CDungeon::getSecondEventMonsters() const
{
    return const_cast<std::vector<stSecondEventMonster_t>*>(&m_secondEventMonsters);
}

std::map<int, std::vector<RandomList> >* CDungeon::getRandomTowerMapIndexes()
    const
{
    return const_cast<std::map<int, std::vector<RandomList> >*>(&m_map658);
}

int CDungeon::get_dimension_min_partymem() const
{
    return m_3a;
}

std::vector<MazeScript>* CDungeon::GetQuestMazeScriptVector(
    ENUM_MAZE_QUEST_TYPE mazeQuestType) const
{
    return const_cast<std::vector<MazeScript>*>(&m_mazeList0[mazeQuestType]);
}

// ---- 迁移自 GameStubs.cpp 的 CDungeon 桩方法（ORIG 弱符号，签名已核对）----
CDungeon::CDungeon()
    : m_flag4(0),
      m_6c9(0),
      m_6cc(0),
      m_6d0(0),
      m_6d1(0)
{
    // ORIG 0x08373f3c：初始化列表按成员声明位置赋值（vtable 后立即 m_flag4，
    // 0x6bc vector 构造后 0x6c9/0x6cc/0x6d0/0x6d1），其它 POD 不初始化。
}

CDungeon::~CDungeon()
{
    // ORIG 0x08376fd8：析构 std 容器成员（自动）。
}

int CDungeon::get_index() const
{
    // ORIG 0x080fdcf0：return m_index（+0x08）
    return m_index;
}

int CDungeon::get_min_level() const
{
    // ORIG 0x0814559a：return m_c（+0x0c）
    return m_c;
}

int CDungeon::get_standard_level() const
{
    // ORIG 0x080f9810：return m_10（+0x10）
    return m_10;
}

const char* CDungeon::GetDungeonName() const
{
    // ORIG 0x081455a6：return m_name.c_str()（m_name +0x18）
    return m_name.c_str();
}

int CDungeon::get_dimension_possible() const
{
    // ORIG 0x0826b938：return m_38（movzbl 0x38）
    return m_38;
}

bool CDungeon::isTowerOfDespairDungeon() const
{
    // ORIG 0x0822b4fa：return GetType_DungeonInHeritance() == 1
    return GetType_DungeonInHeritance() == 1;
}

bool CDungeon::isTournamentDungeon() const
{
    // ORIG 0x0836520a：+0x8a8 位标志（1=普通赛, 2=特殊）
    return m_8a8 != 0 && m_8a8 <= 1;
}

bool CDungeon::IsEnterEachMap() const
{
    // ORIG 0x08365278：return (bool)m_6d0（movzbl 0x6d0）
    return m_6d0;
}

// ORIG _ZN12CDataManager18set_dimensionInoutEic / _ZN12CDataManager21set_limit_inout_countEc15ENUM_BLOOD_TYPE
// 真实 C++ 成员实现（权威声明在 CDataManager.h），语义按 ORIG 反汇编：
//   set_dimensionInout：idx∈[0,5] 写 m_dimensionInout[idx]（080eed82 W）
//   set_limit_inout_count：写 m_limitInoutCount[type]（08374c82 W）
void CDataManager::set_dimensionInout(int idx, char value)
{
    if (idx >= 0 && idx <= 5)
        m_dimensionInout[idx] = value;
}

void CDataManager::set_limit_inout_count(char value, ENUM_BLOOD_TYPE type)
{
    m_limitInoutCount[type] = value;
}

void CDungeon::set_dungeon(STDungeonScript& script)
{
    // ORIG 0x0834acc4：从脚本复制全部字段（顺序按 ORIG 汇编）
    m_index = script.m_18;
    m_10 = script.m_ac;
    m_c = script.m_78;
    m_14 = script.m_b0;

    char nameBuf[500] = {0};
    strcpy(nameBuf, script.m_str1c.c_str());
    if (!CodePage::script2Database((char*)script.m_str1c.c_str(), nameBuf))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x58a, 5)(
            "Error!!! %s->%s [%s][%s][%d]",
            CodePage::script(), CodePage::database(),
            script.m_str1c.c_str(), __PRETTY_FUNCTION__, 0x58a);
    }
    m_name = nameBuf;

    for (int i = 0; i < 5; ++i)
        m_6e0[i] = script.m_b4[i];

    m_34 = script.m_14;
    m_38 = script.m_304;
    m_39 = script.m_305;
    m_3a = script.m_306;
    if (m_38 != 0)
    {
        G_CDataManager()->set_dimensionInout((int)(char)m_38 - 1, (char)m_39);
        G_CDataManager()->set_original_dimensionInout(
            (int)(char)m_38 - 1, (char)m_39);
    }
    m_3b = script.m_324;
    m_3c = script.m_325;
    if (m_index == 0x2aff)
    {
        G_CDataManager()->set_limit_inout_count((char)m_3c, (ENUM_BLOOD_TYPE)2);
    }
    if (m_index == 0x2afe)
    {
        G_CDataManager()->set_limit_inout_count((char)m_3c, (ENUM_BLOOD_TYPE)1);
    }

    for (std::vector<MazeScript>::iterator mit = script.m_mazeList170.begin();
         mit != script.m_mazeList170.end(); ++mit)
    {
        MazeScript* maze = &(*mit);
        if (maze->m_4c == 0)
        {
            m_mazeList0[0].push_back(*maze);
        }
        else
        {
            if (maze->m_4c == 1)
            {
                m_mazeList0[1].push_back(*maze);
            }
            else if (maze->m_4c == -1)
            {
                m_mazeList.push_back(*maze);
            }
        }
    }

    int totalRate = 0;
    m_clearItemMap.clear();
    std::vector<std::pair<int, int> >::const_iterator cit =
        script.m_vecc8.begin();
    std::pair<int, int> p;
    do
    {
        do
        {
            if (cit == script.m_vecc8.end())
                goto copy_rest;
            p = *cit;
        } while (p.first == 0);
        totalRate = p.second + totalRate;
        m_clearItemMap.insert(std::make_pair(totalRate, p.first));
        if (totalRate > 100)
        {
            LogManager::logFormat(
                1, "data_manager.cpp",
                "void CDungeon::set_dungeon(STDungeonScript&)", 0x5e1,
                "total_rate(%d) > 100", totalRate);
            goto copy_rest;
        }
        ++cit;
    } while (true);

copy_rest:
    m_vec40 = script.m_vecd4;
    m_vec4c = script.m_vece0;
    m_vec58 = script.m_vecec;
    script.m_warRoom18c.copy(&m_warRoomData);
    m_requiredItem1 = script.m_requiredItem7c;
    m_85c = script.m_88;
    m_requiredItem2 = script.m_requiredItem8c;
    m_vec86c = script.m_vec98;
    m_878 = script.m_314;
    m_879 = script.m_315;
    m_87b = script.m_317;
    if (script.m_316)
        m_87a = 1;
    else
        m_87a = 0;
    m_87c = script.m_a4;
    m_bossRoom90c.m_0 = script.m_bossRoom328.m_0;
    m_bossRoom90c.m_vec4 = script.m_bossRoom328.m_vec4;
    std::copy(script.m_128, script.m_128 + 5, m_880);
    m_894 = script.m_13c;
    m_898 = script.m_168;
    m_89c = script.m_28c;
    m_89d = script.m_140;
    m_89e = script.m_28d;
    m_89f = script.m_28e;
    m_800 = script.m_2ab;
    m_804 = script.m_2ac;
    memcpy(m_808, script.m_2b0, sizeof(m_808));
    memcpy(m_820, script.m_2bc, sizeof(m_820));
    m_vec814 = script.m_vec2c8;
    m_clearReward = script.m_clearReward2d4;
    m_survivalReward = script.m_survival2f8;
    m_624 = 0;
    m_628 = script.m_16c;
    m_650 = script.m_338;
    m_670 = script.m_358;
    m_680 = script.m_368;
    if (m_680 != 0)
    {
        m_vec674.reserve(m_680 + 1);
        for (unsigned int i = 0; i < script.m_vec35c.size(); ++i)
            m_vec674[script.m_vec35c.at(i).first] =
                script.m_vec35c.at(i).second;
    }
    m_654 = script.m_33c;
    if (m_654 != 0)
    {
        m_map658.clear();
        for (std::map<int, std::vector<RandomList> >::const_iterator rit =
                 script.m_map340.begin();
             rit != script.m_map340.end(); ++rit)
        {
            m_map658.insert(std::make_pair(rit->first, rit->second));
        }
    }
    m_8a0 = script.m_3c4;
    m_vec8f0 = script.m_vec3c8;
    m_684 = script.m_3d4;
    m_688 = script.m_3d8;
    m_68c = script.m_3dc;
    m_eventMonsters = script.m_vec3e0;
    m_69c = script.m_3ec;
    m_6a0 = script.m_3f0;
    m_6a4 = script.m_3f4;
    m_secondEventMonsters = script.m_vec3f8;
    m_6b4 = (script.m_380 == 1);
    m_6b5 = (script.m_3b4 == 1);
    m_6b8 = script.m_3b8;
    m_vec900 = script.m_vec404;
    m_6c8 = script.m_48c;
    if (script.m_2a9 != 0)
    {
        m_flag4 = 1;
    }
    m_8fc = script.m_2aa;
    m_8a8 = script.m_494;
    m_8a4 = script.m_498;
    m_itemRate8c4 = script.m_itemRate4b4;
    m_map8ac = script.m_map49c;
    m_itemRate8c4.m_10 = script.m_4c4;
    m_map8d8 = script.m_map4c8;
    m_map91c = script.m_map4e4;
    m_dimensionPartyCount.swap(script.m_dimension308);
    m_6c9 = script.m_4e0;
    m_6cc = script.m_4fc;
    m_6d0 = script.m_504;
    m_6d1 = script.m_505;
}

// ORIG _ZN12CDataManager18set_dimensionInoutEic：m_dimensionInout[idx]=value（idx 0..5）
// 注：CDataManager.h union 布局暂差 0x18（+0xaa5c vs ORIG +0xaa74），
// 用局部镜像结构按 ORIG 偏移直接寻址（不修改 CDataManager.h）。
// ---- CDungeon_TowerOfDespair（迁移自 GameStubs.cpp；ORIG W 0x08377606）----
CDungeon_TowerOfDespair::CDungeon_TowerOfDespair()
{
}

CDungeon_TowerOfDespair::~CDungeon_TowerOfDespair()
{
}

int CDungeon_TowerOfDespair::_do_after_dungeon_start(CUser* user) const
{
    // ORIG 0x0834d8ec：进入绝望之塔时递增 TOD_UserState 进入计数。
    // 依赖 TOD_UserState::IncreaseEnterCount（未实现），此处保留 null 检查 + 成功返回，
    // 待 TOD_UserState 补齐后续细化。
    if (user == 0)
        return 0;
    return 1;
}
int CDungeon::_do_after_dungeon_start(CUser* user) const
{
    return 1;
}

int CDungeon::GetMaze(int idx) const
{
    if ((unsigned int)idx >= m_mazeList.size())
        return 0;
    return (int)&m_mazeList[idx];
}

void CDungeon::GetRandMaze(int& out) const
{
    out = rand() % m_mazeList.size();
    (void)m_mazeList[out];
}

int CDungeon::GetRandMazebyPartyCount(int partyCount) const
{
    if (m_mazeList.size() == 1)
        return (int)&m_mazeList[0];

    std::vector<MazeScript*> candidates;
    unsigned int i = 0;
    while (i < m_mazeList.size())
    {
        MazeScript* maze = const_cast<MazeScript*>(&m_mazeList[i]);
        if (maze->m_68 <= partyCount && partyCount <= maze->m_6c)
        {
            MazeScript* mazeTmp = maze;
            candidates.push_back(mazeTmp);
        }
        ++i;
    }
    unsigned int r = 0;
    if (!candidates.empty())
    {
        if (candidates.size() == 1)
            return (int)candidates[0];
        r = rand() % candidates.size();
        return (int)candidates[r];
    }
    else
    {
        r = rand() % m_mazeList.size();
        return (int)&m_mazeList[r];
    }
}

int CDungeon::GetStartMap(int direction) const
{
    if (direction < 0x10 && direction >= 0)
    {
        if (m_maplist[direction].size() == 0)
        {
            LogManager::logFormat(
                1, "data_manager.cpp", "int CDungeon::GetStartMap(int) const",
                0x79a,
                "CDungeon::GetStartMap() : direction(%d), dungeon index(%d), "
                "start_maplist_[direction].size() == 0, May be Script Error!",
                direction, m_index);
            return -1;
        }
        int r = get_rand_int(m_maplist[direction].size());
        __gnu_cxx::hash_map<int, CMap*>::const_iterator it =
            m_maplist[direction].begin();
        for (int i = 0; i < r; ++i)
        {
            ++it;
            if (it == m_maplist[direction].end())
                return -1;
        }
        return it->first;
    }
    return -1;
}

int CDungeon::get_clear_item() const
{
    // ORIG 0x0834c8c0：ret 先置栈、迭代器默认构造、lower_bound 到临时再赋值
    int ret = 0;
    std::map<int, int>::const_iterator it;
    int key = get_rand_int(100) + 1;
    it = m_clearItemMap.lower_bound(key);
    if (it == m_clearItemMap.end())
        ret = -2;
    else
        ret = it->second;
    return ret;
}

int CDungeon::get_clear_item(int level) const
{
    // ORIG 0x0834c95c：同 get_clear_item()，key = level + 1
    int ret = 0;
    std::map<int, int>::const_iterator it;
    int key = level + 1;
    it = m_clearItemMap.lower_bound(key);
    if (it == m_clearItemMap.end())
        ret = -2;
    else
        ret = it->second;
    return ret;
}

int CDungeon::getLimitCoinDiff(int diff, int& out) const
{
    if (m_map91c.size() == 0)
        return 0;
    std::map<int, UseCoinDungeonDiff>::const_iterator it = m_map91c.find(diff);
    if (it == m_map91c.end())
        return 0;
    out = it->second.m_0;
    return 1;
}

bool CDungeon::checkFreeRevivalCondition(int diff) const
{
    std::map<int, UseCoinDungeonDiff>::const_iterator it = m_map91c.find(diff);
    if (it != m_map91c.end() && it->second.m_4 != 1)
        return true;
    return false;
}

int CDungeon::getTournamentRoundFatigue() const
{
    if (m_8a4 == 0)
        return 1;
    return m_8a4;
}

bool CDungeon::get_dimension_member_count(DimensionPartyCount& out) const
{
    // ORIG 0x0834d220：std::find 匹配 m_0/m_1，命中复制 m_2 返回 true
    std::vector<DimensionPartyCount>::const_iterator it = std::find(
        m_dimensionPartyCount.begin(), m_dimensionPartyCount.end(), out);
    if (it != m_dimensionPartyCount.end())
    {
        out.m_2 = it->m_2;
        return true;
    }
    return false;
}

void GridScript::clear()
{
    m_gridValue = 0;
    m_mapCandidates.clear();
    m_secondCandidates.clear();
    m_layeredMapIndexes.clear();
}

GridScript::GridScript()
{
    clear();
}

GridScript::GridScript(const GridScript& other)
{
    m_gridValue = other.m_gridValue;
    m_mapCandidates = other.m_mapCandidates;
    m_secondCandidates = other.m_secondCandidates;
    m_layeredMapIndexes = other.m_layeredMapIndexes;
}

GridScript::~GridScript()
{
}

GridScript& GridScript::operator=(const GridScript& other)
{
    m_gridValue = other.m_gridValue;
    m_mapCandidates = other.m_mapCandidates;
    m_secondCandidates = other.m_secondCandidates;
    m_layeredMapIndexes = other.m_layeredMapIndexes;
    return *this;
}

void RidableObject::clear()
{
    m_pad[0] = 0;
}

RidableObject::RidableObject()
{
    clear();
}

RidableObject::~RidableObject()
{
}

void RidableScript::clear()
{
    m_0 = 0;
    m_4 = 0;
    m_objects.clear();
}

RidableScript::RidableScript()
{
    clear();
}

RidableScript::RidableScript(const RidableScript& other)
{
    m_0 = other.m_0;
    m_4 = other.m_4;
    m_objects = other.m_objects;
}

RidableScript::~RidableScript()
{
}

RidableScript& RidableScript::operator=(const RidableScript& other)
{
    m_0 = other.m_0;
    m_4 = other.m_4;
    m_objects = other.m_objects;
    return *this;
}

void DungeonClearCondition::clear()
{
    m_0 = 0;
    m_4 = 0;
    m_8 = 0;
}

DungeonClearCondition::DungeonClearCondition()
{
    clear();
}

DungeonClearCondition::~DungeonClearCondition()
{
}

// ============================================================================
// MazeScript（0x74 字节）
// ============================================================================

void MazeScript::clear()
{
    m_width = 0;
    m_height = 0;
    m_2c = 0;
    m_30 = 0;
    m_34 = 0;
    m_4c = 0;
    m_50 = 0;
    m_54 = 0;
    m_64 = 0;
    m_68 = 0;
    m_6c = 0;
    m_70 = 0;
}

MazeScript::MazeScript()
{
    clear();
}

MazeScript::MazeScript(const MazeScript& other)
{
    m_width = other.m_width;
    m_height = other.m_height;
    m_grids = other.m_grids;
    m_vec14 = other.m_vec14;
    m_vec20 = other.m_vec20;
    m_2c = other.m_2c;
    m_30 = other.m_30;
    m_34 = other.m_34;
    m_ridable = other.m_ridable;
    m_4c = other.m_4c;
    m_50 = other.m_50;
    m_54 = other.m_54;
    m_clearCondition = other.m_clearCondition;
    m_64 = other.m_64;
    m_68 = other.m_68;
    m_6c = other.m_6c;
    m_70 = other.m_70;
}

MazeScript::~MazeScript()
{
}

MazeScript& MazeScript::operator=(const MazeScript& other)
{
    m_width = other.m_width;
    m_height = other.m_height;
    m_grids = other.m_grids;
    m_vec14 = other.m_vec14;
    m_vec20 = other.m_vec20;
    m_2c = other.m_2c;
    m_30 = other.m_30;
    m_34 = other.m_34;
    m_ridable = other.m_ridable;
    m_4c = other.m_4c;
    m_50 = other.m_50;
    m_54 = other.m_54;
    m_clearCondition = other.m_clearCondition;
    m_64 = other.m_64;
    m_68 = other.m_68;
    m_6c = other.m_6c;
    m_70 = other.m_70;
    return *this;
}

int MazeScript::getGrid(int x, int y)
{
    return m_grids[m_width * y + x].m_gridValue;
}

// ============================================================================
// stDungeonAssignItem_t / RandomList / 事件怪结构
// ============================================================================

void stDungeonAssignItem_t::clear()
{
    m_assignItemIdx = -1;
    m_list.clear();
}

stDungeonAssignItem_t::stDungeonAssignItem_t()
{
    clear();
}

stDungeonAssignItem_t::stDungeonAssignItem_t(
    const stDungeonAssignItem_t& other)
{
    m_assignItemIdx = other.m_assignItemIdx;
    m_list = other.m_list;
}

stDungeonAssignItem_t::~stDungeonAssignItem_t()
{
}

stDungeonAssignItem_t& stDungeonAssignItem_t::operator=(
    const stDungeonAssignItem_t& other)
{
    m_assignItemIdx = other.m_assignItemIdx;
    m_list = other.m_list;
    return *this;
}

RandomList::~RandomList()
{
}

stEventMonster_t::~stEventMonster_t()
{
}

stSecondEventMonster_t::~stSecondEventMonster_t()
{
}

// ============================================================================
// STMonsterTypeSpawnProb / STDeadTowerData / STWarRoomData / RequiredItem
// ============================================================================

void STMonsterTypeSpawnProb::clear()
{
    memset(m_pad, 0, sizeof(m_pad));
}

STMonsterTypeSpawnProb::STMonsterTypeSpawnProb()
{
    clear();
}

void STDeadTowerData::clear()
{
    m_vec.clear();
}

STDeadTowerData::STDeadTowerData()
{
    clear();
}

STDeadTowerData::~STDeadTowerData()
{
}

void STWarRoomData::clear()
{
    memset(&m_0, 0, 0xc);
    m_c = 0;
    m_10 = -1;
    m_vec14.clear();
    m_vec20.clear();
    m_vec2c.clear();
    m_vec38.clear();
    m_vec44.clear();
    m_50 = 0;
    m_54 = 0;
    m_58 = 0;
    memset(m_5c, 0, sizeof(m_5c));
    m_vec6c.clear();
    m_vec78.clear();
    m_vec84.clear();
    m_vec90.clear();
    memset(m_9c, 0, sizeof(m_9c));
    m_ac = 0;
    m_spawnProb.clear();
    m_bc = 0;
    m_c0 = 0;
    memset(m_c4, 0, sizeof(m_c4));
    m_d4 = 0;
    m_d8 = 0;
    memset(m_dc, 0, sizeof(m_dc));
    m_ec = 0;
    m_f0 = 0;
    m_f4 = 0;
    m_f8 = 0;
    m_fc = 0;
}

void STWarRoomData::copy(STWarRoomData* other)
{
    memcpy(other, this, 0xc);
    other->m_c = m_c;
    other->m_10 = m_10;
    other->m_vec14.reserve(m_vec14.size());
    for (size_t i = 0; i < m_vec14.size(); ++i)
        other->m_vec14.push_back(m_vec14[i]);
    other->m_vec20.reserve(m_vec20.size());
    for (size_t i = 0; i < m_vec20.size(); ++i)
        other->m_vec20.push_back(m_vec20[i]);
    other->m_vec2c.reserve(m_vec2c.size());
    for (size_t i = 0; i < m_vec2c.size(); ++i)
        other->m_vec2c.push_back(m_vec2c[i]);
    other->m_vec38.reserve(m_vec38.size());
    for (size_t i = 0; i < m_vec38.size(); ++i)
        other->m_vec38.push_back(m_vec38[i]);
    other->m_vec44.reserve(m_vec44.size());
    for (size_t i = 0; i < m_vec44.size(); ++i)
        other->m_vec44.push_back(m_vec44[i]);
    other->m_50 = m_50;
    other->m_54 = m_54;
    other->m_58 = m_58;
    memcpy(other->m_5c, m_5c, sizeof(m_5c));
    other->m_vec6c.reserve(m_vec6c.size());
    for (size_t i = 0; i < m_vec6c.size(); ++i)
        other->m_vec6c.push_back(m_vec6c[i]);
    other->m_vec78.reserve(m_vec78.size());
    for (size_t i = 0; i < m_vec78.size(); ++i)
        other->m_vec78.push_back(m_vec78[i]);
    other->m_vec84.reserve(m_vec84.size());
    for (size_t i = 0; i < m_vec84.size(); ++i)
        other->m_vec84.push_back(m_vec84[i]);
    other->m_vec90.reserve(m_vec90.size());
    for (size_t i = 0; i < m_vec90.size(); ++i)
        other->m_vec90.push_back(m_vec90[i]);
    memcpy(other->m_9c, m_9c, sizeof(m_9c));
    other->m_ac = m_ac;
    other->m_spawnProb.reserve(m_spawnProb.size());
    for (size_t i = 0; i < m_spawnProb.size(); ++i)
        other->m_spawnProb.push_back(m_spawnProb[i]);
    other->m_bc = m_bc;
    other->m_c0 = m_c0;
    memcpy(other->m_c4, m_c4, sizeof(m_c4));
    other->m_d4 = m_d4;
    other->m_d8 = m_d8;
    memcpy(other->m_dc, m_dc, sizeof(m_dc));
    other->m_ec = m_ec;
    other->m_f0 = m_f0;
    other->m_f4 = m_f4;
    other->m_f8 = m_f8;
    other->m_fc = m_fc;
}

STWarRoomData::STWarRoomData()
{
    clear();
}

STWarRoomData::~STWarRoomData()
{
}

void RequiredItem::clear()
{
    m_0 = -1;
    m_4 = 0;
    m_8 = 0;
}

RequiredItem::RequiredItem()
{
    clear();
}

// ============================================================================
// advancealtar::ClearRewardData / SurvivalRewardData / 其它奖励类型
// ============================================================================

void advancealtar::ClearRewardData::clear()
{
    m_vec0.clear();
    m_vecc.clear();
}

advancealtar::ClearRewardData::ClearRewardData()
{
    clear();
}

advancealtar::ClearRewardData::~ClearRewardData()
{
}

advancealtar::ClearRewardData& advancealtar::ClearRewardData::operator=(
    const ClearRewardData& other)
{
    m_vec0 = other.m_vec0;
    m_vecc = other.m_vecc;
    return *this;
}

advancealtar::SurvivalRewardData::SurvivalRewardData()
{
}

advancealtar::SurvivalRewardData::~SurvivalRewardData()
{
}

advancealtar::SurvivalRewardData&
advancealtar::SurvivalRewardData::operator=(const SurvivalRewardData& other)
{
    m_vec = other.m_vec;
    return *this;
}

void TournamentRewardItemRate::Clear()
{
    m_c = 0;
    m_items.clear();
}

TournamentRewardItemRate::TournamentRewardItemRate()
{
    Clear();
}

TournamentRewardItemRate::~TournamentRewardItemRate()
{
}

TournamentRewardItemRate& TournamentRewardItemRate::operator=(
    const TournamentRewardItemRate& other)
{
    m_items = other.m_items;
    m_c = other.m_c;
    return *this;
}

void stConditionEnterBossRoom_t::clear()
{
    m_0 = 0;
    m_vec4.clear();
}

stConditionEnterBossRoom_t::stConditionEnterBossRoom_t()
{
    clear();
}

stConditionEnterBossRoom_t::~stConditionEnterBossRoom_t()
{
}

RecoveryStairInfo::RecoveryStairInfo()
{
}

RecoveryStairInfo::RecoveryStairInfo(const RecoveryStairInfo& other)
{
    m_vec = other.m_vec;
}

RecoveryStairInfo::~RecoveryStairInfo()
{
}

RecoveryStairInfo& RecoveryStairInfo::operator=(
    const RecoveryStairInfo& other)
{
    m_vec = other.m_vec;
    return *this;
}

void STTowerDialog::clear()
{
    m_0 = 0;
    m_str4.clear();
    m_8 = 0;
    m_9 = 0;
    m_a = 0;
}

STTowerDialog::STTowerDialog()
{
    clear();
}

STTowerDialog::STTowerDialog(const STTowerDialog& other)
{
    m_0 = other.m_0;
    m_str4 = other.m_str4;
    m_8 = other.m_8;
    m_9 = other.m_9;
    m_a = other.m_a;
}

STTowerDialog::~STTowerDialog()
{
}

STTowerDialog& STTowerDialog::operator=(const STTowerDialog& other)
{
    m_0 = other.m_0;
    m_str4 = other.m_str4;
    m_8 = other.m_8;
    m_9 = other.m_9;
    m_a = other.m_a;
    return *this;
}

void STTowerFullDialog::clear()
{
    m_vec0.clear();
    m_vecc.clear();
    m_vec18.clear();
}

STTowerFullDialog::STTowerFullDialog()
{
    clear();
}

STTowerFullDialog::~STTowerFullDialog()
{
}

