// ============================================================================
// BattleDepsB2.cpp — CBattle_Field / CRidable 剩余链接方法（独占 TU，宿主登记 CMake）。
//
// 恢复符号（ORIG nm 全部实证，df_game_r）：
//   _ZN13CBattle_Field23CheckHellPartyConditionEv        T 0x082ff988
//   _ZN13CBattle_Field10ConsistMapEv                     T 0x083031d2
//   _ZNK13CBattle_Field18GetAppropriateMazeE23ENUM_DUNGEON_DIFFICULTY T 0x082ff128
//   _ZN13CBattle_Field22GetHellPartyDifficultyE17ENUM_DUNGEON_TYPE    T 0x082ffa2e
//   _ZN13CBattle_Field21GetHellPartyEventTypeEc          T 0x082ffb06
//   _ZN13CBattle_Field7MoveMapEiiRb                      T 0x08308ae6
//   _ZN13CBattle_Field11SetGridPathEP10MazeScripti       T 0x082ffb30
//   _ZN13CBattle_Field15checkLayeredMapEii               T 0x0830e32e
//   _ZN13CBattle_Field9onUseItemEP5CUserRK10Inven_Item   T 0x0830dbde
//   _ZN13CBattle_Field16setQuestMazeInfoEv               T 0x0830e170
//
// 报告依据（编辑前已完整读 ##汇编 + ##反编译 C）：
//   docs/class_func_reports/CBattle_Field/{CheckHellPartyCondition,ConsistMap,
//     GetAppropriateMaze,GetHellPartyDifficulty,GetHellPartyEventType,MoveMap,
//     SetGridPath,checkLayeredMap,onUseItem,setQuestMazeInfo}.md
//   CRidable::GetObjectCnt 无函数报告 → 已先读同类报告（CRidable/README、
//     SetRidableObjectState）与调用方（BattleDungeonMapA2.cpp
//     MakePacket 段）后按规则 objdump 0x830ea0a 转写（= m_objects.size()）。
//
// 布局核对：CBattle_Field(0xcd0)/CRidable/CParty/BattleData(+0x7e0)/CMap/
//   MapInfo/MazeScript/GridScript/stHellParty_t 均用权威头具名成员；
//   仅 CDataManager+0x49d0 一处 ORIG 未建模字段保持直接读取（见 ConsistMap 注释）。
// ============================================================================

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <list>
#include <utility>
#include <vector>

#include "BattleData.h"           // BattleData（CParty+0x32c 子对象，m_hellPartyValueTotal）
#include "CBattle_Field.h"
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h        // CBattle_Field + CMap/CDungeon/CParty/WarField 权威类型
#include "CGuildServerProxy.h"    // CEventBase::IsEventing / CEventManager::GetRepeatEvent
#include "DataManagerInitDepends.h"  // stHellParty_t（get_hellparty_script_values 返回值）
#include "Quest.h"               // Quest 完整类型（m_field120）
#include "GlobalData.h"           // GlobalData::s_systemTime_ / s_event_manager

// ---- MONSTER_POS（ORIG 全局类型：W operator== 0x830e6e9 双字节比较；
//      SetGridPath 局部 std::list<MONSTER_POS> 去重表。全源码此前无定义点。）----
struct MONSTER_POS
{
    char m_x;   // +0x00
    char m_y;   // +0x01
};

inline bool operator==(const MONSTER_POS& a, const MONSTER_POS& b)
{
    return a.m_x == b.m_x && a.m_y == b.m_y;
}

// ============================ CRidable ============================

// ORIG 0x0830ea0a：直接返回 m_objects.size()（call _ZNKSt3mapIi20DungeonRidableObject...4sizeEv）。
int CRidable::GetObjectCnt()
{
    return (int)m_objects.size();
}

// ============================ CBattle_Field ============================

// ORIG 0x082ff988 CheckHellPartyCondition：
//   m_party 为空 → false；rate=(int)party->BattleData.m_hellPartyValueTotal(+0xb0c)；
//   rate >= get_rand_int(100) 且 rate >= stHellParty_t.m_guideLineValue(+0x90) → true。
bool CBattle_Field::CheckHellPartyCondition()
{
    if (m_party == NULL)
        return false;

    // ORIG 对 party+0xb0c 的 float 两次独立读取转换（flds/fldcw/fistpl ×2）。
    BattleData* bd = &m_party->m_battleData;

    bool ok = false;
    if ((int)bd->m_hellPartyValueTotal >= get_rand_int(100))
    {
        stHellParty_t* vals =
            static_cast<stHellParty_t*>(G_CDataManager()->get_hellparty_script_values());
        if ((int)bd->m_hellPartyValueTotal >= vals->m_guideLineValue)  // +0x90
            ok = true;
    }
    return ok;
}

// ORIG 0x083031d2 ConsistMap：当前图装配怪物/物品链（逐调用与报告一致）。
void CBattle_Field::ConsistMap()
{
    if (m_party == NULL)
        return;

    MapInfo* mapInfo = GetCurrentMapInfo();
    m_pMap = getMap(mapInfo->m_mapIndex);        // 实参 = mapInfo->m_mapIndex(+0x04)
    if (m_pMap == NULL)
        return;

    setDungeonClearTrace(mapInfo->m_mapIndex, m_pMap->GetBossCount(), 0, 0);

    CDungeon* dungeon = G_CDataManager()->find_dungeon(m_dungeonIndex);
    if (dungeon == NULL)
    {
        LogManager::logFormat(1, "battle_field.cpp",
                              __PRETTY_FUNCTION__, 0x7f2,
                              "G_CDataManager()->find_dungeon( %d) fail", m_dungeonIndex);
        return;
    }

    int addChampionCnt = getAddChampionMobCnt();
    if (addChampionCnt < 0)
        return;

    std::list<_mapMonster> mobList;
    if (!get_monster_list(mobList))
        return;

    int curX = -1;
    int curY = -1;
    getCurPosXY(curX, curY);
    add_monster_list_bossroom_condition(mobList, curX, curY);

    int monsterCnt = 0;                          // -0x28
    map_monster mob;                             // -0x390（默认构造 081511c8）
    std::vector<int> championList;               // -0x34
    int uid = get_rand_int(60000);               // -0x3c
    int seq = 0;                                 // -0x38
    add_monsters_current_map(mobList, championList, mob, monsterCnt, uid, seq);

    bool killedAll = false;                      // -0x3d
    add_monster_hell_party(mob, killedAll, uid, seq);

    if (add_monster_blood(mob, uid, seq))
    {
        m_dungeonClearTracer.SetMaxMonsterCount(monsterCnt);
        add_monster_APC_AI(uid);
        add_monster_APC_AI_by_rate(mob, uid, curX, curY);
        add_monster_additional_champion(championList, addChampionCnt);
        add_monster_event(uid, curX, curY);
        set_monster_ability();
        insert_tournamentRoundMonster();

        int itemUid = get_rand_int(60000);       // -0x44
        add_item_curr_map(itemUid, uid);
        mapInfo->m_dropItemCnt = itemUid;        // +0x40

        // ORIG 083034ed..0830351c：m_pMap->m_ec(+0xec)==0 时取
        // *(uint*)(G_CDataManager()+0x49d0) 作 SelectDonsterItemDropLimit 上限。
        // 即 CDataManager::m_serverParameter.m_4968（CDataManager+0x68+0x4968）。
        unsigned int dropLimit;
        if (m_pMap->m_ec == 0)
        {
            CDataManager* dm = G_CDataManager();
            dropLimit = static_cast<unsigned int>(dm->m_serverParameter.m_4968);
        }
        else
        {
            dropLimit = (unsigned int)m_pMap->m_ec;
        }
        mapInfo->SelectDonsterItemDropLimit(dropLimit);
    }
}

// ORIG 0x082ff128 GetAppropriateMaze(diff)：
//   按 questType 0/1 取任务迷宫向量；过滤血战态/hell 标记/任务连接/单人格挡；
//   questType==0 额外要求 maze->m_54 <= diff；候选 1 个直取、多个 rand()%n，
//   两轮无候选则回退 CDungeon::GetRandMazebyPartyCount(成员数)。
MazeScript* CBattle_Field::GetAppropriateMaze(ENUM_DUNGEON_DIFFICULTY diff) const
{
    for (int questType = 0; questType < 2; ++questType)
    {
        std::vector<MazeScript>* mazes =
            m_pDungeon->GetQuestMazeScriptVector((ENUM_MAZE_QUEST_TYPE)questType);
        if (mazes == NULL || mazes->empty())
            continue;

        MazeScript* picked = NULL;
        bool found = false;
        {
            std::vector<MazeScript*> candidates;
            for (std::vector<MazeScript>::iterator it = mazes->begin();
                 it != mazes->end(); ++it)
            {
                MazeScript* maze = &*it;
                if ((int)m_bloodState == 1)               // +0x1b4
                    continue;
                if ((char)m_hellPartyActive != 0 && maze->m_70 != 0)
                    continue;

                bool connected = false;
                if (m_party != NULL &&
                    m_party->CheckQuestConnection(maze->m_50,
                                                  (ENUM_MAZE_QUEST_TYPE)questType))
                    connected = true;
                if (!connected)
                    continue;

                Quest* quest = G_CDataManager()->find_quest(maze->m_50);
                if (quest == NULL)
                    continue;
                if (quest->m_field120 == 1 &&             // +0x120 单人任务
                    m_party->get_member_count() > 1)
                    continue;

                if (questType == 0)
                {
                    if (maze->m_54 <= (int)diff)
                        candidates.push_back(maze);
                }
                else
                {
                    candidates.push_back(maze);
                }
            }

            size_t cnt = candidates.size();
            if (cnt == 1)
            {
                picked = candidates[0];
                found = true;
            }
            else if (cnt >= 2)
            {
                picked = candidates[::rand() % cnt];
                found = true;
            }
        }
        if (found)
            return picked;
    }
    return (MazeScript*)m_pDungeon->GetRandMazebyPartyCount(m_party->get_member_count());
}

// ORIG 0x082ffa2e GetHellPartyDifficulty(type)：
//   六字节难度比例取自 stHellParty_t（type==1 → m_field84[6]，否则 m_field7e[6]
//   且非空队伍经 increase_hell_party_ratio 提升），rnd=get_rand_int(100) 落区间
//   [i]=1..5 返回 i，否则 0（下标 0 槽不参与）。
int CBattle_Field::GetHellPartyDifficulty(ENUM_DUNGEON_TYPE type)
{
    // ORIG 0x082ffa2e：ratios `{0}` 单零聚合标量（P202① movl+movw 两写）+
    // get_hellparty_script_values 各分支独立重调（P48）。
    char ratios[6] = { 0 };
    if ((int)type == 1)
    {
        memcpy(ratios, static_cast<stHellParty_t*>(
                   G_CDataManager()->get_hellparty_script_values())->m_field84, 6);
    }
    else
    {
        memcpy(ratios, static_cast<stHellParty_t*>(
                   G_CDataManager()->get_hellparty_script_values())->m_field7e, 6);
        if (m_party != NULL)
            m_party->increase_hell_party_ratio(ratios);
    }

    // ORIG 0x082ffa2e：for(i=1;i<=5;i++) 双入口（进入直接落底部守卫 +0xc6，
    //   setle/test/jne）+ `if(acc>=rnd) return i`（mov acc;cmp rnd;jl continue，
    //   与 ORIG +0xb5/+0xbb 同形）；守卫落空 return 0（P0 具名局部 acc/rnd/i）。
    int acc = 0;
    int rnd = get_rand_int(100);
    int i;
    for (i = 1; i <= 5; i++)
    {
        acc += ratios[i];                        // movsbl 有符号字节累加
        if (acc >= rnd)
            return i;
    }
    return 0;
}

// ORIG 0x082ffb06 GetHellPartyEventType(diff)：查 stHellParty_t.m_field8a[diff]。
int CBattle_Field::GetHellPartyEventType(char diff)
{
    return (signed char)static_cast<stHellParty_t*>(
        G_CDataManager()->get_hellparty_script_values())->m_field8a[(int)diff];
}

// ORIG 0x08308ae6 MoveMap(x, y, firstVisit)：换图主流程（错误码 0/1/0x12/0x14/0x15 或检查函数码）。
int CBattle_Field::MoveMap(int x, int y, bool& firstVisit)
{
    MapInfo* mapInfo = GetCurrentMapInfo();

    if (m_pDungeon->IsEnterEachMap() || check_grid_clear())
    {   // 每图进入型副本 / 网格未清：拒绝
        moveMap_1_Log(x, y);
        return 0x14;
    }

    if (m_pDungeon != NULL && (char)m_pDungeon->m_bloodDungeonType > 0)
    {   // 血战副本换图时重置血轮
        reset_blood_values();
        m_bloodRound.reset_blood_values();
        m_bloodRound.setBloodRoundOrder();
    }

    if (x < 0 || x > 0x31 || y < 0 || y > 0x31)
        return 1;

    if (mapInfo->m_layeredMap != 1)
    {   // 非层叠图：原地不动判定
        int curX = -1;
        int curY = -1;
        getCurPosXY(curX, curY);
        if (x == curX && y == curY)
            return 0x12;
    }

    if (m_gridMaze == NULL)
    {
        moveMap_2_Log(x, y);
        return 0x15;
    }

    if (!(x < m_gridMaze->m_width && y < m_gridMaze->m_height))
        return 0x15;

    int err = check_boss_map(x, y);
    if (err != 0)
        return err;

    err = check_movemap_range(x, y);
    if (err != 0)
        return err;

    const GridScript* grid = &m_gridMaze->getGridR(x, y);
    if (grid == NULL)
    {
        moveMap_3_Log(x, y);
        return 0x15;
    }

    if (check_visited_map(x, y, firstVisit))
        return 0;

    int dir = get_invade_direction(x, y);
    CDungeon* dungeon = G_CDataManager()->find_dungeon(m_dungeonIndex);
    if (dungeon == NULL)
    {
        moveMap_4_Log(x, y, grid);
        return 1;
    }

    int newMapId = -1;
    bool dummyFlag = true;
    choose_random_bossmap_from_dummymaps(dungeon, grid, newMapId, dummyFlag, x, y, dir);

    std::vector<int> candidator;
    m_visitedGotoFlag = 0;                       // +0x1c8 byte 清零

    err = get_candidator_map(dungeon, grid, newMapId, candidator, x, y, dir);
    if (err != 0)
        return err;

    set_visited_map(x, y, dummyFlag);
    swap_curpos_xy(x, y);
    set_visited_map_goto(x, y);
    GetCurrentMapInfo()->m_gridValue = (char)grid->m_gridValue;   // +0x08 ← 网格值低字节

    err = set_map_id(grid, newMapId, candidator);
    if (err != 0)
        return err;

    ConsistMap();
    if (m_party != NULL)
        m_party->m_killMobCounter.clear();   // +0x1844
    return 0;
}

// ORIG 0x082ffb30 SetGridPath(maze, dummyAppearCnt)：
//   记录迷宫宽高与起点/boss 槽位链、dummy 图序列、hell 三字段、ridable 对象；
//   bossRoom90c==1 时装配进 boss 房条件组；事件怪两轮生成（0x24 重复事件判活 +
//   MONSTER_POS 去重 / 第二事件不去重）。
void CBattle_Field::SetGridPath(MazeScript* maze, int dummyAppearCnt)
{
    m_gridMaze = maze;                           // +0x74
    m_mapWidth = maze->m_width;                  // +0x18c ← maze+0x00
    m_mapHeight = maze->m_height;                // +0x190 ← maze+0x04
    if (m_mapWidth + m_mapHeight < 4)
    {
        m_hellPartyActive = 0;                   // +0x1b8
        m_field1cb = 1;                          // +0x1cb
    }

    if (maze->m_vec14.empty())                   // 起点槽
        return;
    std::pair<int, int> startPos = maze->m_vec14[::rand() % maze->m_vec14.size()];

    if (maze->m_vec20.empty())                   // boss 槽
        return;
    int bossIdx = (int)(::rand() % maze->m_vec20.size());
    std::pair<int, int> bossPos = maze->m_vec20[bossIdx];

    std::pair<int, int> dummyPos;
    if (dummyAppearCnt > 0)
    {
        int prevIdx = bossIdx;
        m_dummyMapPosIdx = 0;                    // +0xcc
        m_dummyMapPos.clear();                   // +0xc0
        for (int i = 0; i < dummyAppearCnt; ++i)
        {
            if (maze->m_vec20.size() > 1)
            {   // 与上次不同下标（同值跳过再 +1）
                int idx = (int)(::rand() % (maze->m_vec20.size() - 1));
                if (prevIdx <= idx)
                    idx += 1;
                dummyPos = maze->m_vec20[idx];
                m_dummyMapPos.push_back(dummyPos);
                prevIdx = idx;
            }
        }
    }

    std::pair<int, int> adjBossPos;              // 相邻 boss 槽（优先下一格）
    if (bossIdx - 1 >= 0)
        adjBossPos = maze->m_vec20[bossIdx - 1];
    if (bossIdx + 1 <= (int)maze->m_vec20.size() - 1)
        adjBossPos = maze->m_vec20[bossIdx + 1];

    // 位置链（ORIG +0x2f0..+0x338）：注意 ORIG 存向 —— +4=起点.second、+8=起点.first。
    m_startX = startPos.second;
    m_startY = startPos.first;
    m_curX = m_startX;
    m_curY = m_startY;
    m_lastX = m_curX;
    m_lastY = m_curY;

    if (m_pDungeon != NULL && m_pDungeon->IsEnterEachMap())
    {
        for (int i = 0; i < 4; ++i)
        {
            m_pos[2 + i] = std::make_pair(startPos.second, startPos.first);
            m_pos[6 + i] = std::make_pair(startPos.second, startPos.first);
        }
    }

    m_pos[0] = bossPos;                          // +0x0c 终点槽
    m_pos[1] = adjBossPos;                       // +0x14 相邻槽
    m_hellPosX = maze->m_30;                     // +0x1c0
    m_hellPosY = maze->m_34;                     // +0x1c4
    m_hellMapIdx = maze->m_2c;                   // +0x1bc
    m_ridable.SetRidableObject(maze->m_ridable);// +0x38 RidableScript

    if (m_pDungeon->m_bossRoom90c.m_0 == 1)      // dungeon+0x90c
    {
        // —— boss 房条件候选格：非空网格且其候选地图均有条件怪 ——
        std::vector<std::pair<int, int> > candidates;
        for (int row = 0; row < maze->m_height; ++row)
        {
            for (int col = 0; col < maze->m_width; ++col)
            {
                if ((m_pos[0].first == col && m_pos[0].second == row) ||
                    (m_startX == col && m_startY == row))
                    continue;

                const GridScript* grid = &maze->getGridR(col, row);
                if (grid->m_mapCandidates.empty())
                    continue;

                bool ok = true;
                for (std::vector<int>::const_iterator cit =
                         grid->m_mapCandidates.begin();
                     cit != grid->m_mapCandidates.end(); ++cit)
                {
                    CMap* cm = static_cast<CMap*>(G_CDataManager()->find_map(*cit));
                    if (cm == NULL)
                    {
                        ok = false;
                        break;
                    }
                    std::list<_mapMonster> condList = cm->get_condition_monster_list();
                    if (condList.empty())
                    {
                        ok = false;
                        break;
                    }
                }
                if (ok)
                    candidates.push_back(std::make_pair(col, row));
            }
        }

        // —— 条件三元组（monsterIdx, spawnCnt, maxKillCnt）→ boss 房状态 ——
        const std::vector<int>& triples = m_pDungeon->m_bossRoom90c.m_vec4;  // +0x910
        std::vector<int>::const_iterator it = triples.begin();
        while (it != triples.end())
        {
            int monsterIdx = *it;
            ++it;
            int spawnCnt = *it;
            ++it;
            int maxKillCnt = *it;
            ++it;

            stConditionEnterBossRoomStatus_t status;
            status.m_monsterIdx = monsterIdx;
            status.m_curKillCount = 0;
            status.m_maxKillCount = maxKillCnt;
            for (int k = 0; k < spawnCnt; ++k)
            {
                int r = get_rand_int((int)candidates.size());
                status.m_pos.push_back(candidates.at(r));
            }
            m_bossRoomConditions.push_back(status);   // +0x170
        }
    }

    // —— 事件怪生成（需 tryGenEventMonsterCnt!=0 且重复事件 0x24 判活）——
    if (m_pDungeon->getTryGenEventMonsterCnt() != 0)
    {
        bool eventOn = false;
        CEventBase* repeatEvent = GlobalData::s_event_manager->GetRepeatEvent(0x24);
        if (repeatEvent->IsEventing((CUser*)0))       // vtable+0x34 槽 13，ORIG 不判空
            eventOn = true;

        if (eventOn)
        {
            std::vector<std::pair<int, int> > positions;
            for (int row = 0; row < maze->m_height; ++row)
            {
                for (int col = 0; col < maze->m_width; ++col)
                {
                    const GridScript* grid = &maze->getGridR(col, row);
                    if (grid->m_gridValue != 0)
                        positions.push_back(std::make_pair(col, row));
                }
            }

            std::list<MONSTER_POS> usedPositions;
            for (int attempt = 0; attempt < m_pDungeon->getTryGenEventMonsterCnt();
                 ++attempt)
            {
                if (!(get_rand_int(10000) <=
                      (unsigned int)m_pDungeon->getTryGenEventMonsterProb()))
                    continue;

                const std::vector<stEventMonster_t>* monsters =
                    m_pDungeon->getEventMonsters();
                int roll = get_rand_int(m_pDungeon->getEventMonsterTotalProb());
                int acc = 0;
                for (std::vector<stEventMonster_t>::const_iterator mit =
                         monsters->begin();
                     mit != monsters->end(); ++mit)
                {
                    acc += mit->m_prob;
                    if ((unsigned int)roll >= (unsigned int)acc)
                        continue;

                    // 命中：随机位置 + 状态（0x18 字段族）
                    stEventMonsterStatus_t status;
                    status.m_eventIdx = mit->m_eventIdx;
                    int posIdx = get_rand_int((int)positions.size());
                    status.m_pos = positions.at(posIdx);
                    status.m_monsterIdx = mit->m_monsterIdx;
                    status.m_field10 = mit->m_field0c;
                    status.m_field14 = mit->m_field10;

                    MONSTER_POS mp;
                    mp.m_x = (char)status.m_pos.first;
                    mp.m_y = (char)status.m_pos.second;

                    bool sameOnList =
                        std::find(usedPositions.begin(), usedPositions.end(), mp) !=
                        usedPositions.end();
                    if (!sameOnList)
                    {
                        cMyTrace trace(__PRETTY_FUNCTION__,
                                       0x3e3, 0);
                        trace("Event monster generated (%d,%d ->%d(%d)",
                              status.m_pos.first, status.m_pos.second,
                              status.m_eventIdx, status.m_monsterIdx);
                        usedPositions.push_back(mp);
                        m_eventMonsterStatus.push_back(status);   // +0x17c
                    }
                    else
                    {
                        cMyTrace trace(__PRETTY_FUNCTION__,
                                       0x3e8, 0);
                        trace("Event monster SameMap (not generated) (%d,%d ->%d(%d)",
                              status.m_pos.first, status.m_pos.second,
                              status.m_eventIdx, status.m_monsterIdx);
                    }
                    break;
                }
            }
            usedPositions.clear();
        }
    }

    // —— 第二事件怪生成（无去重、无日志）——
    if (m_pDungeon->getTryGenSecondEventMonsterCnt() != 0)
    {
        std::vector<std::pair<int, int> > positions;
        for (int row = 0; row < maze->m_height; ++row)
        {
            for (int col = 0; col < maze->m_width; ++col)
            {
                const GridScript* grid = &maze->getGridR(col, row);
                if (grid->m_gridValue != 0)
                    positions.push_back(std::make_pair(col, row));
            }
        }

        for (int attempt = 0; attempt < m_pDungeon->getTryGenSecondEventMonsterCnt();
             ++attempt)
        {
            if (!(get_rand_int(10000) <=
                  (unsigned int)m_pDungeon->getTryGenSecondEventMonsterProb()))
                continue;

            const std::vector<stSecondEventMonster_t>* monsters =
                m_pDungeon->getSecondEventMonsters();
            int roll = get_rand_int(m_pDungeon->getSecondEventMonsterTotalProb());
            int acc = 0;
            for (std::vector<stSecondEventMonster_t>::const_iterator mit =
                     monsters->begin();
                 mit != monsters->end(); ++mit)
            {
                acc += mit->m_prob;
                if ((unsigned int)roll >= (unsigned int)acc)
                    continue;

                stEventMonsterStatus_t status;
                status.m_eventIdx = mit->m_eventIdx;
                int posIdx = get_rand_int((int)positions.size());
                status.m_pos = positions.at(posIdx);
                status.m_monsterIdx = mit->m_monsterIdx;
                status.m_field10 = mit->m_field0c;
                status.m_field14 = mit->m_field10;
                m_eventMonsterStatus.push_back(status);
                break;
            }
        }
    }
}

// ORIG 0x0830e32e checkLayeredMap(x, y)：迷宫格带层叠表则推进层号并置当前图层标记。
void CBattle_Field::checkLayeredMap(int x, int y)
{
    MazeScript* maze = getMaze();
    if (maze != NULL)
    {
        const GridScript* grid = &maze->getGridR(x, y);
        if (grid != NULL && grid->m_layeredMapIndexes.size() != 0)
        {
            if ((int)grid->m_layeredMapIndexes.size() > getLayeredMapIndex() + 1)
                setLayeredMapIndex();

            getCurrentLayeredMapInfo()->m_layeredMap = 1;    // MapInfo+0xe8
        }
    }
}

// ORIG 0x0830dbde onUseItem(user, item)：技能材料计时 + 用物计数。
void CBattle_Field::onUseItem(CUser* user, const Inven_Item& item)
{
    MapInfo* mapInfo = GetCurrentMapInfo();

    if (item.m_addInfo == 0xbdd)                 // +0x02 dword（技能材料物品）
    {
        unsigned int now = GlobalData::s_systemTime_.getCurTickCount();
        mapInfo->m_killChecker1.setUseSkillMaterial(now);   // +0x4c
        now = GlobalData::s_systemTime_.getCurTickCount();
        mapInfo->m_killChecker2.setUseSkillMaterial(now);   // +0x64
    }

    if ((unsigned char)item.m_field1 == 2)
        ++mapInfo->m_playInfo.m_useItemType2Count;          // +0x84
    else
        ++mapInfo->m_playInfo.m_useItemOtherCount;          // +0x88
}

// ORIG 0x0830e170 setQuestMazeInfo()：扫描两类任务迷宫，符合条件者置任务迷宫标记。
void CBattle_Field::setQuestMazeInfo()
{
    for (int questType = 0; questType < 2; ++questType)
    {
        std::vector<MazeScript>* mazes =
            m_pDungeon->GetQuestMazeScriptVector((ENUM_MAZE_QUEST_TYPE)questType);
        if (mazes == NULL || mazes->empty())
            continue;

        for (std::vector<MazeScript>::iterator it = mazes->begin();
             it != mazes->end(); ++it)
        {
            MazeScript* maze = &*it;
            if ((int)m_bloodState == 1)              // 血战态跳过
                continue;

            bool connected = false;
            if (m_party != NULL &&
                m_party->CheckQuestConnection(maze->m_50,
                                              (ENUM_MAZE_QUEST_TYPE)questType))
                connected = true;
            if (!connected)
                continue;

            Quest* quest = G_CDataManager()->find_quest(maze->m_50);
            if (quest == NULL)
                continue;

            if (quest->m_field120 == 1)
            {   // 单人限定任务迷宫：仅单人队伍置位
                if (m_party->get_member_count() == 1)
                    setQuestMaze(true);
            }
            else
            {
                setQuestMaze(true);
            }
        }
    }
}
