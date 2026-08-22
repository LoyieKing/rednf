// ============================================================================
// df_game_r MapInfo 还原（G2-5 批次，尺寸 0xec）。
// 无独立报告，按 ORIG 反汇编逐函数转录：
//   C1/C2 08151394 / copy 08235d40 / D1/D2 081518bc / operator= 0830eea2
//   Clear 081514ae / Add_Mob(map_monster) 08151612 / Add_Item(map_item) 081517e0
//   SelectDonsterItemDropLimit 0830e800
// 布局以 CBattle_Field.h:176 权威布局为准（map/multimap 0x18、vector 0xc、
// killChecker 0x18、playInfo 0x10）。
// map_monster（0x34c）/map_item（0x54）为全局结构，本 TU 内最小复刻：
// 复制/析构等由 ORIG 对应 TU 提供（跨类调用保持同名外部符号）。
// ============================================================================

#include <algorithm>
#include <map>
#include <utility>
#include <vector>
#include <cstring>
#include "WarField.h"
#include "LogManager.h"
// 日志：使用真实 LogManager.h 的 5 参 logFormat(level,file,func,line,fmt,...)。
// 原 ORIG 4 参符号（_ZN10LogManager9logFormatEiPKciS1_z）为旧签名，桥接已随迁移删除。


// ============================================================================
// map_item / map_monster 生命周期（ORIG 081512cc / 081512dc / 081511c8 /
// 081515ae / 08151aea / 08151d3a；布局以 WarField.h 为权威）
// ============================================================================

map_item::map_item()
    : m_count(0), m_itemIndex(0), m_dropIndex(0), m_createTick(0),
      m_item(), m_ownerId(0) {}

map_item::map_item(const map_item& other)
    : m_count(other.m_count), m_itemIndex(other.m_itemIndex),
      m_dropIndex(other.m_dropIndex), m_createTick(other.m_createTick),
      m_item(other.m_item), m_ownerId(other.m_ownerId) {}

map_item::~map_item() {}

map_item& map_item::operator=(const map_item& other)
{
    if (this != &other)
    {
        m_count = other.m_count;
        m_itemIndex = other.m_itemIndex;
        m_dropIndex = other.m_dropIndex;
        m_createTick = other.m_createTick;
        m_item = other.m_item;
        m_ownerId = other.m_ownerId;
    }
    return *this;
}

map_monster::map_monster()
    : m_mobId(0), m_instId(0), m_roleType(0), m_mobIndex(0), m_level(0),
      m_field11(0), m_field13(0), m_field14(0), m_field15(0), m_field16(0),
      m_field17(0), m_field18(0), m_field1c(0), m_field20(0), m_field24(0),
      m_field28(0), m_dropCnt(0), m_dropRate(100), m_flag34(0), m_active(),
      m_mobList(), m_hellPartyGroupIdx(0), m_hellPartyGroupOrder(0),
      m_hellPartyScriptIdx(0), m_pad339(0), m_bloodSmallRoundOrder(0),
      m_bloodPhaseTime(0), m_bloodRoundOrder(0), m_bloodSpawnIdx(0),
      m_bloodPhaseIdx(0), m_bloodKillFlag(0), m_bloodDifficulty(0),
      m_bloodRoundOrder2(0), m_bloodSpawned(0), m_isBoss(0), m_bloodRound(0),
      m_bloodRoundFlag(0) {}

map_monster::map_monster(const map_monster& other)
    : m_mobId(other.m_mobId), m_instId(other.m_instId),
      m_roleType(other.m_roleType), m_mobIndex(other.m_mobIndex),
      m_level(other.m_level), m_field11(other.m_field11),
      m_field13(other.m_field13), m_field14(other.m_field14),
      m_field15(other.m_field15), m_field16(other.m_field16),
      m_field17(other.m_field17), m_field18(other.m_field18),
      m_field1c(other.m_field1c), m_field20(other.m_field20),
      m_field24(other.m_field24), m_field28(other.m_field28),
      m_dropCnt(other.m_dropCnt), m_dropRate(other.m_dropRate),
      m_flag34(other.m_flag34), m_active(other.m_active),
      m_mobList(other.m_mobList),
      m_hellPartyGroupIdx(other.m_hellPartyGroupIdx),
      m_hellPartyGroupOrder(other.m_hellPartyGroupOrder),
      m_hellPartyScriptIdx(other.m_hellPartyScriptIdx),
      m_pad339(other.m_pad339),
      m_bloodSmallRoundOrder(other.m_bloodSmallRoundOrder),
      m_bloodPhaseTime(other.m_bloodPhaseTime),
      m_bloodRoundOrder(other.m_bloodRoundOrder),
      m_bloodSpawnIdx(other.m_bloodSpawnIdx),
      m_bloodPhaseIdx(other.m_bloodPhaseIdx),
      m_bloodKillFlag(other.m_bloodKillFlag),
      m_bloodDifficulty(other.m_bloodDifficulty),
      m_bloodRoundOrder2(other.m_bloodRoundOrder2),
      m_bloodSpawned(other.m_bloodSpawned), m_isBoss(other.m_isBoss),
      m_bloodRound(other.m_bloodRound), m_bloodRoundFlag(other.m_bloodRoundFlag) {}

map_monster::~map_monster() {}

map_monster& map_monster::operator=(const map_monster& other)
{
    if (this != &other)
    {
        m_mobId = other.m_mobId;
        m_instId = other.m_instId;
        m_roleType = other.m_roleType;
        m_mobIndex = other.m_mobIndex;
        m_level = other.m_level;
        m_field11 = other.m_field11;
        m_field13 = other.m_field13;
        m_field14 = other.m_field14;
        m_field15 = other.m_field15;
        m_field16 = other.m_field16;
        m_field17 = other.m_field17;
        m_field18 = other.m_field18;
        m_field1c = other.m_field1c;
        m_field20 = other.m_field20;
        m_field24 = other.m_field24;
        m_field28 = other.m_field28;
        m_dropCnt = other.m_dropCnt;
        m_dropRate = other.m_dropRate;
        m_flag34 = other.m_flag34;
        m_active = other.m_active;
        m_mobList = other.m_mobList;
        m_hellPartyGroupIdx = other.m_hellPartyGroupIdx;
        m_hellPartyGroupOrder = other.m_hellPartyGroupOrder;
        m_hellPartyScriptIdx = other.m_hellPartyScriptIdx;
        m_pad339 = other.m_pad339;
        m_bloodSmallRoundOrder = other.m_bloodSmallRoundOrder;
        m_bloodPhaseTime = other.m_bloodPhaseTime;
        m_bloodRoundOrder = other.m_bloodRoundOrder;
        m_bloodSpawnIdx = other.m_bloodSpawnIdx;
        m_bloodPhaseIdx = other.m_bloodPhaseIdx;
        m_bloodKillFlag = other.m_bloodKillFlag;
        m_bloodDifficulty = other.m_bloodDifficulty;
        m_bloodRoundOrder2 = other.m_bloodRoundOrder2;
        m_bloodSpawned = other.m_bloodSpawned;
        m_isBoss = other.m_isBoss;
        m_bloodRound = other.m_bloodRound;
        m_bloodRoundFlag = other.m_bloodRoundFlag;
    }
    return *this;
}

MapInfo::MapInfo()
{
    Clear();
}

MapInfo::MapInfo(const MapInfo& other)
    : m_monsterMap(other.m_monsterMap),
      m_itemMap(other.m_itemMap),
      m_hellPartyGroup(other.m_hellPartyGroup),
      m_mapA8(other.m_mapA8),
      m_vecC0(other.m_vecC0),
      m_tournamentMob(other.m_tournamentMob)
{
    m_key = other.m_key;
    m_visited = other.m_visited;
    m_visitedGoto = other.m_visitedGoto;
    m_mapIndex = other.m_mapIndex;
    m_gridValue = other.m_gridValue;
    m_assignItemIdx = other.m_assignItemIdx;
    m_dropItemCnt = other.m_dropItemCnt;
    m_mapType = other.m_mapType;
    m_dummyBossMap = other.m_dummyBossMap;
    m_mapBasisLevel = other.m_mapBasisLevel;
    m_killChecker1 = other.m_killChecker1;
    m_killChecker2 = other.m_killChecker2;
    m_playInfo = other.m_playInfo;
    m_assignNPCByPotion = other.m_assignNPCByPotion;
    m_cc = other.m_cc;
    m_layeredMap = other.m_layeredMap;
}

MapInfo::~MapInfo()
{
}

MapInfo& MapInfo::operator=(const MapInfo& other)
{
    m_key = other.m_key;
    m_visited = other.m_visited;
    m_visitedGoto = other.m_visitedGoto;
    m_mapIndex = other.m_mapIndex;
    m_gridValue = other.m_gridValue;
    m_monsterMap = other.m_monsterMap;
    m_itemMap = other.m_itemMap;
    m_assignItemIdx = other.m_assignItemIdx;
    m_dropItemCnt = other.m_dropItemCnt;
    m_mapType = other.m_mapType;
    m_dummyBossMap = other.m_dummyBossMap;
    m_mapBasisLevel = other.m_mapBasisLevel;
    m_killChecker1 = other.m_killChecker1;
    m_killChecker2 = other.m_killChecker2;
    m_playInfo = other.m_playInfo;
    m_hellPartyGroup = other.m_hellPartyGroup;
    m_assignNPCByPotion = other.m_assignNPCByPotion;
    m_mapA8 = other.m_mapA8;
    m_vecC0 = other.m_vecC0;
    m_cc = other.m_cc;
    m_tournamentMob = other.m_tournamentMob;
    m_layeredMap = other.m_layeredMap;
    return *this;
}

void MapInfo::Clear()
{
    m_key = 0;
    m_visited = 0;
    m_visitedGoto = 0;
    m_mapIndex = 0;
    m_gridValue = 0;
    m_monsterMap.clear();
    m_itemMap.clear();
    m_assignItemIdx = 0;
    m_dropItemCnt = 0;
    m_mapType = 1;
    m_dummyBossMap = 0;
    m_mapBasisLevel = 0;
    m_killChecker1.init();
    m_killChecker2.init();
    m_playInfo.init();
    m_hellPartyGroup.clear();
    m_assignNPCByPotion = 0;
    m_mapA8.clear();
    m_vecC0.clear();
    m_cc = 0;
    m_tournamentMob.clear();
    m_layeredMap = 0;
}

void MapInfo::Add_Mob(map_monster mob)
{
    if (m_monsterMap.find(mob.m_instId) != m_monsterMap.end())
    {
        LogManager::logFormat(1, "MapInfo.cpp", "MapInfo::Add_Mob", 0x1f2,
                              "duplicated mob instance", mob.m_instId);
        return;
    }

    m_monsterMap.insert(
        std::pair<const int, map_monster>(mob.m_instId, mob));
    m_mapA8.insert(std::pair<const int, int>(mob.m_instId, mob.m_mobIndex));
    m_vecC0.push_back(mob.m_instId);

    if (mob.m_bloodRound != 0)
    {
        ++m_cc;
    }
}

void MapInfo::Add_Item(map_item item)
{
    if (m_itemMap.find(item.m_itemIndex) != m_itemMap.end())
    {
        LogManager::logFormat(1, "MapInfo.cpp", "MapInfo::Add_Item", 0x206,
                              "duplicated item index", item.m_itemIndex);
        return;
    }

    m_itemMap.insert(
        std::pair<const int, map_item>(item.m_itemIndex, item));
}

void MapInfo::SelectDonsterItemDropLimit(unsigned int limit)
{
    if (limit == 0)
    {
        return;
    }

    if (m_vecC0.size() - (unsigned int)(unsigned short)m_cc <= limit)
    {
        return;
    }

    std::random_shuffle(m_vecC0.begin(), m_vecC0.end());

    for (std::map<int, map_monster>::iterator it = m_monsterMap.begin();
         it != m_monsterMap.end(); ++it)
    {
        if (it->second.m_bloodRound != 1)
        {
            it->second.m_isBoss = 1;
        }
    }

    for (unsigned int i = 0; i < limit; ++i)
    {
        std::map<int, map_monster>::iterator it =
            m_monsterMap.find(m_vecC0.at(i));
        if (it != m_monsterMap.end())
        {
            if (it->second.m_bloodRound != 0)
            {
                ++limit;
            }
            it->second.m_isBoss = 0;
        }
    }
}
