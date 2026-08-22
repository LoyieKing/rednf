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
    return *(float*)&m_14;
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
    return &((std::vector<MazeScript>*)&m_mazeList0)[mazeQuestType];
}

// ---- 迁移自 GameStubs.cpp 的 CDungeon 桩方法（ORIG 弱符号，签名已核对）----
CDungeon::CDungeon()
{
    // ORIG 0x08373f3c：构造容器/初始化 POD 字段；std 成员自动构造。
    m_flag4 = 0;
    m_index = 0;
    m_c = 0;
    m_10 = 0;
    m_14 = 0;
    m_34 = 0;
    m_38 = 0;
    m_39 = 0;
    m_3a = 0;
    m_3b = 0;
    m_3c = 0;
    m_624 = 0;
    m_628 = 0;
    m_650 = 0;
    m_654 = 0;
    m_670 = 0;
    m_680 = 0;
    m_684 = 0;
    m_688 = 0;
    m_68c = 0;
    m_69c = 0;
    m_6a0 = 0;
    m_6a4 = 0;
    m_6b4 = 0;
    m_6b5 = 0;
    m_6b8 = 0;
    m_6c8 = 0;
    m_6c9 = 0;
    m_6cc = 0;
    m_6d0 = 0;
    m_6d1 = 0;
    m_804 = 0;
    m_878 = 0;
    m_879 = 0;
    m_87a = 0;
    m_87b = 0;
    m_87c = 0;
    m_894 = 0;
    m_898 = 0;
    m_89c = 0;
    m_89d = 0;
    m_89e = 0;
    m_89f = 0;
    m_8a0 = 0;
    m_8a4 = 0;
    m_8a8 = 0;
    m_8fc = 0;
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
    // ORIG 0x0826b938：return (bool)m_38（movzbl 0x38）
    return (unsigned char)m_38;
}

int CDungeon::isTowerOfDespairDungeon() const
{
    // ORIG 0x0822b4fa：return GetType_DungeonInHeritance() == 1
    return GetType_DungeonInHeritance() == 1;
}

int CDungeon::isTournamentDungeon() const
{
    // ORIG 0x0836520a：返回 +0x8a8 位标志（1=普通赛, 2=特殊）
    return m_8a8;
}

bool CDungeon::IsEnterEachMap() const
{
    // ORIG 0x08365278：return (bool)m_6d0（movzbl 0x6d0）
    return (bool)(unsigned char)m_6d0;
}

void CDungeon::set_dungeon(STDungeonScript& script)
{
    // ORIG 0x0834acc4：从脚本复制字段到副本
    m_index = script.m_18;
    m_10 = script.m_ac;
    m_c = script.m_78;
    m_14 = script.m_b0;
    m_name = script.m_str1c;
}

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
    for (size_t i = 0; i < m_mazeList.size(); ++i)
    {
        if (m_mazeList[i].m_68 <= partyCount &&
            partyCount <= m_mazeList[i].m_6c)
            candidates.push_back(const_cast<MazeScript*>(&m_mazeList[i]));
    }
    if (!candidates.empty())
    {
        if (candidates.size() == 1)
            return (int)candidates[0];
        return (int)candidates[rand() % candidates.size()];
    }
    return (int)&m_mazeList[rand() % m_mazeList.size()];
}

int CDungeon::GetStartMap(int direction) const
{
    if (direction < 0 || direction >= 0x10)
        return -1;
    const __gnu_cxx::hash_map<int, CMap*>& ml = m_maplist[direction];
    if (ml.size() == 0)
    {
        LogManager::logFormat(
            1, "data_manager.cpp", "int CDungeon::GetStartMap(int) const",
            0x79a,
            "CDungeon::GetStartMap() : direction(%d), dungeon index(%d), "
            "start_maplist_[direction].size() == 0, May be Script Error!",
            direction, m_index);
        return -1;
    }
    int r = get_rand_int(ml.size());
    __gnu_cxx::hash_map<int, CMap*>::const_iterator it = ml.begin();
    for (int i = 0; i < r; ++i, ++it)
    {
        if (it == ml.end())
            return -1;
    }
    return it->first;
}

int CDungeon::get_clear_item() const
{
    int key = get_rand_int(100) + 1;
    std::map<int, int>::const_iterator it = m_clearItemMap.lower_bound(key);
    if (it == m_clearItemMap.end())
        return -2;
    return it->second;
}

int CDungeon::get_clear_item(int level) const
{
    int key = level + 1;
    std::map<int, int>::const_iterator it = m_clearItemMap.lower_bound(key);
    if (it == m_clearItemMap.end())
        return -2;
    return it->second;
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
    for (size_t i = 0; i < m_dimensionPartyCount.size(); ++i)
    {
        if (m_dimensionPartyCount[i] == out)
        {
            out.m_2 = m_dimensionPartyCount[i].m_2;
            return true;
        }
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

