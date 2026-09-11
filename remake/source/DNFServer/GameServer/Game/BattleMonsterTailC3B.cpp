// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 BattleMonsterTailC3B/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <cstdlib>
#include <map>
#include <utility>
#include <vector>

#include "CBattle_Field.h"
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h      // CBattle_Field + WarField.h(MapInfo/map_monster) +
                                // CMap.h(STAssignBloodSystem/STAssignBloodPhaseTime/CMap) +
                                // CDungeon.h(CDungeon/m_index/m_dummyMap*/m_map1a4)
#include "CDataManager.h"       // G_CDataManager()/find_map/find_monster
#include "GameRand.h"           // get_rand_int
#include "LogManager.h"         // LogManager::logFormat

#include "BattleMonsterTailC3B/is_visited_map.inc"


#include "BattleMonsterTailC3B/is_visited_map_goto.inc"


#include "BattleMonsterTailC3B/on_not_visited_map.inc"


#include "BattleMonsterTailC3B/add_blood_monster.inc"


#include "BattleMonsterTailC3B/ChooseRandomMap.inc"
{
    typedef std::multimap<int, CMap*> MapMultiMap;

    if (mapType < 0 || direction < 0 || entrance < 0)
    {
        LogManager::logFormat(
            1, "data_manager.cpp",
            __PRETTY_FUNCTION__, 0x71b,
            "CDungeon::ChooseRandomMap [1](Dungeon Index(%d))(map_type(%d) < 0 || "
            "direction(%d) < 0 || entrance(%d) < 0)==false",
            m_index, mapType, direction, entrance);
        return -1;
    }

    // ORIG 局部形态：p_map 置空、range 默认构造、迭代器默认构造、计数清零。
    CMap* pMap = NULL;
    std::pair<MapMultiMap::const_iterator, MapMultiMap::const_iterator> range;
    MapMultiMap::const_iterator it;
    int count = 0;

    if (mapType == 1)
    {
        if (entrance == 0)
        {
            if (m_dummyMap324[direction].size() == 0)
            {
                LogManager::logFormat(
                    1, "data_manager.cpp",
                    __PRETTY_FUNCTION__, 0x72c,
                    "CDungeon::ChooseRandomMap "
                    "(boss_maplist_[direction].size()==0)(Dungeon Index(%d))"
                    "(map_type(%d),direction(%d),entrance(%d)",
                    m_index, mapType, direction, entrance);
                return -1;
            }
            it = m_dummyMap324[direction].begin();
            for (unsigned int step = 0;
                 static_cast<unsigned int>(rand()) %
                         m_dummyMap324[direction].size() > step;
                 ++step)
                it++;
            pMap = it->second;
            if (pMap->get_index() == -1)
            {
                LogManager::logFormat(
                    1, "data_manager.cpp",
                    __PRETTY_FUNCTION__, 0x736,
                    "CDungeon::ChooseRandomMap "
                    "(p_map->get_index()==-1)[1](Dungeon Index(%d))"
                    "(map_type(%d),direction(%d),entrance(%d)",
                    m_index, mapType, direction, entrance);
            }
            return pMap->get_index();
        }
        range = m_dummyMap324[direction].equal_range(entrance);
        count = m_dummyMap324[direction].count(entrance);
    }
    else if (mapType == 2)
    {
        if (entrance == 0)
        {
            if (m_dummyMap4a4[direction].size() == 0)
            {
                LogManager::logFormat(
                    1, "data_manager.cpp",
                    __PRETTY_FUNCTION__, 0x745,
                    "CDungeon::ChooseRandomMap "
                    "(dummy_maplist_[direction].size()==0)(Dungeon Index(%d))"
                    "(map_type(%d),direction(%d),entrance(%d)",
                    m_index, mapType, direction, entrance);
                return -1;
            }
            it = m_dummyMap4a4[direction].begin();
            for (unsigned int step = 0;
                 static_cast<unsigned int>(rand()) %
                         m_dummyMap4a4[direction].size() > step;
                 ++step)
                it++;
            pMap = it->second;
            if (pMap->get_index() == -1)
            {
                LogManager::logFormat(
                    1, "data_manager.cpp",
                    __PRETTY_FUNCTION__, 0x74f,
                    "CDungeon::ChooseRandomMap "
                    "(p_map->get_index()==-1)[1](Dungeon Index(%d))"
                    "(map_type(%d),direction(%d),entrance(%d)",
                    m_index, mapType, direction, entrance);
            }
            return pMap->get_index();
        }
        range = m_dummyMap4a4[direction].equal_range(entrance);
        count = m_dummyMap4a4[direction].count(entrance);
    }
    else
    {
        if (entrance == 0)
        {
            if (m_map1a4[direction].size() == 0)
            {
                LogManager::logFormat(
                    1, "data_manager.cpp",
                    __PRETTY_FUNCTION__, 0x75e,
                    "CDungeon::ChooseRandomMap "
                    "(maplist_[direction].size()==0)(Dungeon Index(%d))"
                    "(map_type(%d),direction(%d),entrance(%d)",
                    m_index, mapType, direction, entrance);
                return -1;
            }
            it = m_map1a4[direction].begin();
            for (unsigned int step = 0;
                 static_cast<unsigned int>(rand()) % m_map1a4[direction].size() >
                     step;
                 ++step)
                it++;
            pMap = it->second;
            if (pMap->get_index() == -1)
            {
                LogManager::logFormat(
                    1, "data_manager.cpp",
                    __PRETTY_FUNCTION__, 0x768,
                    "CDungeon::ChooseRandomMap "
                    "(p_map->get_index()==-1)[2](Dungeon Index(%d))"
                    "(map_type(%d),direction(%d),entrance(%d)",
                    m_index, mapType, direction, entrance);
            }
            return pMap->get_index();
        }
        range = m_map1a4[direction].equal_range(entrance);
        count = m_map1a4[direction].count(entrance);
    }

    if (count == 0)
    {
        LogManager::logFormat(
            1, "data_manager.cpp",
            __PRETTY_FUNCTION__, 0x77b,
            "CDungeon::ChooseRandomMap "
            "(Key count is zero! : plz check map script)(Dungeon Index(%d))"
            "(map_type(%d),direction(%d),entrance(%d)",
            m_index, mapType, direction, entrance);
        return -1;
    }

    MapMultiMap::const_iterator itRange = range.first;
    for (int step = 0; rand() % count > step; ++step)
        itRange++;
    pMap = itRange->second;
    if (pMap->get_index() == -1)
    {
        LogManager::logFormat(
            1, "data_manager.cpp",
            __PRETTY_FUNCTION__, 0x786,
            "CDungeon::ChooseRandomMap "
            "(p_map->get_index()==-1)[3](Dungeon Index(%d))"
            "(map_type(%d),direction(%d),entrance(%d)",
            m_index, mapType, direction, entrance);
    }
    return pMap->get_index();
}
