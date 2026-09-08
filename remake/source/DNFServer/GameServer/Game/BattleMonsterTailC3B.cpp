// ============================================================================
// BattleMonsterTailC3B.cpp — CBattle_Field 血战收尾/访问图查询 + CDungeon
// ChooseRandomMap 剩余链接依赖（独占 TU，宿主登记 CMake；不改 GameStubs，
// 不重复 CBattle_Field_unresolved.cpp / BattleMonsterTailC3A.cpp /
// BattleMonsterDepsC2.cpp 既有定义）。
//
// 恢复符号（ORIG nm 全部实证，df_game_r）：
//   _ZN13CBattle_Field17add_blood_monsterER11map_monsterRiS2_St6vectorI19STAssignBloodSystemSaIS4_EES3_I22STAssignBloodPhaseTimeSaIS7_EE
//                                                        T 0x08304804
//   _ZN13CBattle_Field14is_visited_mapEii                T 0x08307c7a
//   _ZN13CBattle_Field19is_visited_map_gotoEii           T 0x08307cbe
//   _ZN13CBattle_Field18on_not_visited_mapEiiRb          T 0x08307d0c
//   _ZNK8CDungeon15ChooseRandomMapEiii                   T 0x0834bf56
//
// 报告依据（编辑前已完整读 ##汇编 + ##反编译 C 全文）：
//   docs/class_func_reports/CBattle_Field/{add_blood_monster,is_visited_map,
//     is_visited_map_goto,on_not_visited_map}.md
//   docs/class_func_reports/CDungeon/ChooseRandomMap.md
// 内联弱符号调用点（ORIG W，本 TU 仅调用不定义；定义点由宿主统一归属）：
//   CBattle_Field::GetCurrentMapInfo()            W 0x0822cbcc（getCurPosXY→GetMapInfoFromPos(x,y,false)）
//   CBattle_Field::GetMapInfoFromPos(int,int,bool) W 0x0822cc48（m_mapInfo(+0x78) find/insert）
//   CBattle_Field::swap_curpos_xy(int,int)         T 0x0830878e
//   CDataManager::find_map(int) const              T 0x0835fa14 / find_monster(int) const T 0x0835fd84
//   MapInfo::Add_Mob(map_monster)                  W 0x08151612（按值）
//   CBattle_Field::CBloodRound::setBloodLastSmallRoundOrder(int) W 0x0830eb96
//   CMap::get_map_type() const W 0x0830e5c0 / CMap::get_index() const W 0x08134776
//   get_rand_int(int) 0x086b1b87 / rand() @plt / LogManager::logFormat 0x08ad3c0a
// 常量：add_blood_monster 中 flds 0x8c23d70 = float 1000.0（objdump 实测，
//   mob.m_bloodPhaseTime(+0x33c) = (short)(1000.0f * phaseTime.m_time)）。
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

// ---------------------------------------------------------------------------
// CBattle_Field::is_visited_map — ORIG 0x08307c7a
// GetMapInfoFromPos(x,y,false) 后取 MapInfo+0x01 访问标记。
// ---------------------------------------------------------------------------
bool CBattle_Field::is_visited_map(int x, int y)
{
    MapInfo* info = GetMapInfoFromPos(x, y, false);
    if (info->m_visited != 0)
        return true;
    return false;
}

// ---------------------------------------------------------------------------
// CBattle_Field::is_visited_map_goto — ORIG 0x08307cbe
// 访问标记(+0x01)或 goto 标记(+0x02)任一置位即为真。
// ---------------------------------------------------------------------------
bool CBattle_Field::is_visited_map_goto(int x, int y)
{
    MapInfo* info = GetMapInfoFromPos(x, y, false);
    return info->m_visited != 0 || info->m_visitedGoto != 0;
}

// ---------------------------------------------------------------------------
// CBattle_Field::on_not_visited_map — ORIG 0x08307d0c
// 置访问标记 → swap_curpos_xy 换位 → 以新当前图索引自 CDataManager 取图并更新。
// ---------------------------------------------------------------------------
void CBattle_Field::on_not_visited_map(int x, int y, bool& flag)
{
    flag = true;
    GetCurrentMapInfo()->m_visited = 1;              // movb $0x1,0x1(%eax)
    swap_curpos_xy(x, y);
    // ORIG：先求 GetCurrentMapInfo()->m_mapIndex（驻留 ebx），再取数据管理器，
    // 最后调 find_map —— 以单条嵌套表达式复现该求值顺序。
    CMap* map = static_cast<CMap*>(
        G_CDataManager()->find_map(GetCurrentMapInfo()->m_mapIndex));
    if (map != NULL)
        m_pMap = map;                                // this+0x108
}

// ---------------------------------------------------------------------------
// CBattle_Field::add_blood_monster — ORIG 0x08304804
// 血战按阶段时间表逐阶段生成怪物：
//   阶段 m_phaseIdx==-1 时随机取系统序号并记录到 m_bloodPhaseOrder(+0x258)；
//   每阶段先 push false 到 m_bloodPhaseClear(+0x264)；
//   命中随机疯狂阶段(crazyRound==阶段键)时掷 100 决定疯狂怪物类型并写
//   m_bloodSpawnCntCur(+0x2a4)/m_bloodCrazyRound(+0x2a8)/m_bloodCrazyType(+0x2aa)；
//   内层按累计数量生成 map_monster 并 Add_Mob 到当前图，mobId 在数据管理器
//   找不到时整体返回 false；全部阶段完成后写 lastSmallRoundOrder。
// 局部布局按 ORIG 栈槽逐一对位（含两个从不读取的死局部 -0x42/-0x40）。
// ---------------------------------------------------------------------------
// [b28d-D43] N82/N89 大函数（384/382）（ORIG 0x08304804）。
bool CBattle_Field::add_blood_monster(map_monster& mob, int& uid, int& seq,
                                      std::vector<STAssignBloodSystem> systems,
                                      std::vector<STAssignBloodPhaseTime> phaseTimes)
{
    if (phaseTimes.empty() || systems.empty())
        return false;

    int slot48;    // 栈槽对位（ORIG -0x48 槽；未初始化，无指令）
    short lastPhaseKey = -1;                         // -0x44（word）
    short field42 = 0;                               // -0x42（word，ORIG 死局部）
    int field40 = 0;                                 // -0x40（ORIG 死局部）
    short smallRoundOrder = 0;                       // -0x3a（word）
    int phaseKey = 0;                                // -0x38
    int spawnTotal = 0;                              // -0x34
    int systemIdx = 0;                               // -0x30
    int spawnCount = 0;                              // -0x2c
    const int systemCount = systems.size();          // -0x28
    short crazyRound = 0;                            // -0x24（word）
    if (systemCount == 0)
        return false;
    char crazyType = 0x13;                           // -0x21（size 校验后置初值）

    if (m_pMap != NULL)                              // this+0x108 判空后取图种类
    {
        crazyRound = (m_pMap->get_map_type() == 1) ? get_rand_int(4)
                                                   : get_rand_int(5);
    }
    if (m_pDungeon->m_bloodDungeonType == 2)                      // CDungeon+0x89f（无判空，忠实 ORIG）
        crazyRound = -1;

    for (std::vector<STAssignBloodPhaseTime>::iterator it = phaseTimes.begin();
         it != phaseTimes.end(); it++)
    {
        phaseKey = (*it).m_smallRoundOrder;          // 阶段键 +0x00
        systemIdx = (*it).m_phaseIdx;                // 系统序号 +0x04
        if ((*it).m_phaseIdx == -1)
        {
            systemIdx = get_rand_int(systemCount - 1);
            spawnCount = systemIdx;
            m_bloodPhaseOrder.push_back(static_cast<short>(spawnCount));
        }

        // 生成总数：startCnt 起步，每轮 cntInc*j 累加 totalCnt 次。
        int total = 0;
        int acc = 0;
        spawnTotal = systems[systemIdx].m_startCnt;  // STAssignBloodSystem+0x1c
        for (int j = 0; systems[systemIdx].m_totalCnt > j; ++j)   // +0x24
        {
            total = systems[systemIdx].m_cntInc * j + spawnTotal; // +0x20 * j
            acc += total;
        }
        spawnTotal = acc;

        m_bloodPhaseClear.push_back(false);          // vector<bool> +0x264

        if (lastPhaseKey != phaseKey)
        {
            if (crazyRound == phaseKey)
            {
                const int roll = get_rand_int(100);
                if (roll <= 0x27)
                    crazyType = get_rand_int(5);
                else if (roll <= 0x45)
                    crazyType = get_rand_int(6) + 5;
                else if (roll <= 0x54)
                    crazyType = get_rand_int(4) + 0xb;
                else if (roll <= 0x5e)
                    crazyType = get_rand_int(3) + 0xf;
                else
                    crazyType = 0x12;

                m_bloodCrazyRound = smallRoundOrder;             // +0x2a8（short）
                m_bloodSpawnCntCur = systems[systemIdx].m_id;    // +0x2a4 ← 系统+0x00
                m_bloodCrazyType = crazyType;                    // +0x2aa（byte）
            }
            lastPhaseKey = phaseKey;
        }

        for (int k = 0; k < spawnTotal; k++)
        {
            mob.m_mobId = seq++;                             // seq 为第二 int&
            mob.m_mobIndex = systems[systemIdx].m_id;        // +0x0c ← 系统+0x00
            if (m_pDungeon->m_bloodDungeonType == 2)
                mob.m_level = 0x46;                          // +0x10 byte
            else
                mob.m_level = 0x3c;
            mob.m_roleType = 0;                              // +0x08
            mob.m_bloodDifficulty = crazyType;               // +0x345
            mob.m_instId = uid++;                            // uid 为第一 int&
            mob.m_dropCnt = 100;                             // +0x2c
            mob.m_field11 = 0;                               // +0x11..+0x12
            mob.m_field13 = 0;                               // +0x13
            mob.m_field14 = 0;                               // +0x14
            mob.m_bloodSmallRoundOrder = smallRoundOrder;    // +0x33a ← 阶段序
            mob.m_bloodPhaseTime = 1000.0f * (*it).m_time;   // +0x33c（常量 0x8c23d70=1000.0f）
            mob.m_bloodRoundOrder = systems[systemIdx].m_phase;   // +0x33e ← 系统+0x08
            mob.m_bloodSpawnIdx = systems[systemIdx].m_field0c;   // +0x340 ← 系统+0x0c
            mob.m_bloodPhaseIdx = systems[systemIdx].m_field10;   // +0x342 ← 系统+0x10
            mob.m_bloodKillFlag = 0;                         // +0x344
            mob.m_bloodRoundOrder2 = (*it).m_smallRoundOrder;     // +0x346 ← 阶段+0x00
            mob.m_bloodSpawned = (*it).m_difficulty;              // +0x348 ← 阶段+0x18

            GetCurrentMapInfo()->Add_Mob(mob);               // 实参槽直接 copy-ctor
            crazyType = 0x13;                                // 复位默认类型

        }

        if (static_cast<CMonster*>(G_CDataManager()->find_monster(mob.m_mobIndex)) ==
            NULL)
            return false;

        smallRoundOrder++;
    }

    m_bloodRound.setBloodLastSmallRoundOrder(smallRoundOrder - 1);  // +0x2ac
    return true;
}

// ---------------------------------------------------------------------------
// CDungeon::ChooseRandomMap — ORIG 0x0834bf56（const）
// 按 map_type 选容器：1 → boss_maplist_[dir](+0x324)、2 → dummy_maplist_[dir]
// （+0x4a4）、其余 → maplist_[dir](+0x1a4)；entrance==0 全表随机游走取
// second->get_index()，否则 equal_range(entrance)/count(entrance) 后同型游走。
// 各失败路径按 ORIG 行号/文案记日志（map_type 实参始终传形参本尊）。
// 容器映射与 InsertMap（BattleDungeonMapA2）的 get_map_type 分支一致。
// ---------------------------------------------------------------------------
int CDungeon::ChooseRandomMap(int mapType, int direction, int entrance) const
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
