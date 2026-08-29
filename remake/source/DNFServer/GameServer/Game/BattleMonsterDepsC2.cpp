// ============================================================================
// BattleMonsterDepsC2.cpp — CBattle_Field 怪物装配/换图剩余方法（独占 TU，
// 宿主登记 CMake；不改 GameStubs，不重复既有定义）。
//
// 恢复符号（ORIG nm 全部实证，df_game_r）：
//   _ZN13CBattle_Field6getMapEi                          T 0x08300ed4
//   _ZN13CBattle_Field20setDungeonClearTraceEiiii        T 0x0830107e
//   _ZN13CBattle_Field20getAddChampionMobCntEv           T 0x083010ee
//   _ZN13CBattle_Field16get_monster_listERSt4listI11_mapMonsterSaIS1_EE
//                                                        T 0x083012a4
//   _ZN13CBattle_Field35add_monster_list_bossroom_conditionERSt4listI11_mapMonsterSaIS1_EEii
//                                                        T 0x083013e2
//   _ZN13CBattle_Field24add_monsters_current_mapERSt4listI11_mapMonsterSaIS1_EERSt6vectorIiSaIiEER11map_monsterRiSB_SB_
//                                                        T 0x0830162e
//   _ZN13CBattle_Field22add_monster_hell_partyER11map_monsterRbRiS3_
//                                                        T 0x08301b8c
//   _ZN13CBattle_Field17add_monster_bloodER11map_monsterRiS2_
//                                                        T 0x08301bf6
//   _ZN13CBattle_Field18add_monster_APC_AIERi            T 0x08301d76
//   _ZN13CBattle_Field26add_monster_APC_AI_by_rateER11map_monsterRiii
//                                                        T 0x083020b6
//   _ZN13CBattle_Field31add_monster_additional_championERSt6vectorIiSaIiEEi
//                                                        T 0x083022a8
//   _ZN13CBattle_Field17add_monster_eventERiii           T 0x083023ec
//   _ZN13CBattle_Field19set_monster_abilityEv            T 0x083026da
//   _ZN13CBattle_Field29insert_tournamentRoundMonsterEv  T 0x083029b8
//   _ZN13CBattle_Field17add_item_curr_mapERiS0_          T 0x08302bc6
//   _ZN13CBattle_Field13moveMap_1_LogEii                 T 0x0830766e
//   _ZN13CBattle_Field13moveMap_2_LogEii                 T 0x08307740
//   _ZN13CBattle_Field13moveMap_3_LogEii                 T 0x08307812
//   _ZN13CBattle_Field13moveMap_4_LogEiiPK10GridScript   T 0x083078e4
//   _ZN13CBattle_Field14check_boss_mapEii                T 0x083079ba
//   _ZN13CBattle_Field19check_movemap_rangeEii           T 0x08307b2a
//   _ZN13CBattle_Field18get_candidator_mapEPK8CDungeonPK10GridScriptRiRSt6vectorIiSaIiEEiii
//                                                        T 0x08308214
//   _ZN13CBattle_Field15set_visited_mapEiib              T 0x083086d2
//   _ZN13CBattle_Field10set_map_idEPK10GridScriptRiRSt6vectorIiSaIiEE
//                                                        T 0x08308882
//   _ZN13CBattle_Field20set_visited_map_gotoEii          T 0x08308a24
//   _ZN13CBattle_Field36choose_random_bossmap_from_dummymapsEPK8CDungeonPK10GridScriptRiRbiii
//                                                        T 0x08307eca
//   _ZN13CBattle_Field17check_visited_mapEiiRb           T 0x08307d7c
//   _ZN13CBattle_Field20get_invade_directionEii          T 0x08307e44
//   _ZN13CBattle_Field10onStartMapEv                     T 0x0830dc74
// 桥接（本 TU 独有定义点）：
//   _ZN16CAICharacterList10getByLevelEj                  T 0x0834aa60
//     （工程头以 static getByLevel(void*,unsigned) 桥接声明；实现按
//       CDeathTower.cpp 中 get 桥接同型转写，objdump 0x834aa60 逐行核对）
//
// 报告依据（编辑前已完整读 ##汇编 + ##反编译 C 全文）：
//   docs/class_func_reports/CBattle_Field/{getMap,setDungeonClearTrace,
//     getAddChampionMobCnt,get_monster_list,add_monster_list_bossroom_condition,
//     add_monsters_current_map,add_monster_hell_party,add_monster_blood,
//     add_monster_APC_AI,add_monster_APC_AI_by_rate,
//     add_monster_additional_champion,add_monster_event,set_monster_ability,
//     insert_tournamentRoundMonster,add_item_curr_map,moveMap_1_Log,
//     moveMap_2_Log,moveMap_3_Log,moveMap_4_Log,check_boss_map,
//     check_movemap_range,get_candidator_map,set_visited_map,set_map_id,
//     set_visited_map_goto,choose_random_bossmap_from_dummymaps,
//     check_visited_map,get_invade_direction,onStartMap}.md
//   CAICharacterList::getByLevel 无独立报告 → 先读同类（CDeathTower.cpp get 桥接、
//     CAICharacterList 布局注释）后 objdump 0x0834aa60 转写。
// ============================================================================

#include <cstdlib>
#include <cstring>
#include <list>
#include <map>
#include <utility>
#include <vector>

#include "CBattle_Field.h"      // CBattle_Field + WarField/CMap/CDungeon/CParty/
                                // CMonster/STMonsterScript/GridScript/MazeScript 权威链
#include "CDataManager.h"       // G_CDataManager/find_*/get_hellparty_groupOrder_map/
                                // CAICharacterList（get/getByLevel 桥接声明）
#include "CItemGenerator.h"     // WongWork::stGenerateRefData_t/stGenerateResult_t/CItemGenerator
#include "CItemGeneratorMgr.h"  // WongWork::CItemGeneratorMgr::eGenerateType_t/getGenerator
#include "GlobalData.h"         // GlobalData::s_systemTime_
#include "MonsterIndependentDrop.h"  // CAICharacter 完整镜像（getLevel/getIdx 内联）

using WongWork::stGenerateRefData_t;
using WongWork::stGenerateResult_t;
using WongWork::CItemGenerator;
// ============================ CAICharacterList =============================

// ORIG 0x0834aa60 _ZN16CAICharacterList10getByLevelEj（桥接签名 static (void*, uint)）：
// idx=get_rand_int(10)+level-5；idx==0 或 (uint)idx>0x62 → 0；
// m_byLevelList[idx] 空 → 0；否则 at(get_rand_int(size()))。
// ORIG 0x0834aa60 _ZN16CAICharacterList10getByLevelEj（成员 this）：
// idx=get_rand_int(10)+level-5；idx==0 或 (uint)idx>0x62 → 0；
// m_byLevelList[idx] 空 → 0；否则 at(get_rand_int(size()))。
CAICharacter* CAICharacterList::getByLevel(unsigned int level)
{
    const int idx = get_rand_int(10) + static_cast<int>(level) - 5;
    if (idx == 0 || static_cast<unsigned int>(idx) > 0x62)
        return 0;
    std::vector<CAICharacter*>& bucket = m_byLevelList[idx];
    if (bucket.empty())
        return 0;
    return bucket.at(get_rand_int(static_cast<int>(bucket.size())));
}

// ============================ CBattle_Field ================================

// ORIG 0x08300ed4：find_map(mapIndex)；失败按当前图种类记日志返回 0；
// 困难 4 且地图 m_b4 非零时改写当前图号后二次查找（再失败同样记日志 0x4c4）。
CMap* CBattle_Field::getMap(int mapIndex)
{
    CMap* map = static_cast<CMap*>(G_CDataManager()->find_map(mapIndex));
    if (map == NULL)
    {
        MapInfo* info = GetCurrentMapInfo();
        LogManager::logFormat(
            1, "battle_field.cpp", __PRETTY_FUNCTION__, 0x4a7,
            "Find Map[%d] Error(In Map Maker)(%d/%d) [Map Kind : %s]",
            GetCurrentMapInfo()->m_mapIndex, m_curX, m_curY,
            info->m_layeredMap ? "layeredMap" : "generalMap");
        return NULL;
    }
    if (get_dungeon_diff() == 4 && map->m_b4 != 0)
    {
        GetCurrentMapInfo()->m_mapIndex = map->m_b4;
        map = static_cast<CMap*>(G_CDataManager()->find_map(map->m_b4));
        if (map == NULL)
        {
            MapInfo* info = GetCurrentMapInfo();
            LogManager::logFormat(
                1, "battle_field.cpp", __PRETTY_FUNCTION__, 0x4c4,
                "Find Map[%d] Error(In Map Maker)(%d/%d) [Map Kind : %s]",
                GetCurrentMapInfo()->m_mapIndex, m_curX, m_curY,
                info->m_layeredMap ? "layeredMap" : "generalMap");
            return NULL;
        }
    }
    return map;
}

// ORIG 0x0830107e：四段直写 m_dungeonClearTracer。
void CBattle_Field::setDungeonClearTrace(int mapId, int maxBoss, int killedBoss,
                                         int killedMonster)
{
    m_dungeonClearTracer.SetMapId(mapId);
    m_dungeonClearTracer.SetMaxBossCount(maxBoss);
    m_dungeonClearTracer.SetKilledBossCount(killedBoss);
    m_dungeonClearTracer.SetKilledMonsterCount(killedMonster);
}

// ORIG 0x0830dc74：当前图 m_playInfo.m_startTick(+0x7c) ← 系统 tick。
void CBattle_Field::onStartMap()
{
    GetCurrentMapInfo()->m_playInfo.m_startTick =
        GlobalData::s_systemTime_.getCurTickCount();
}

// ORIG 0x083012a4：前置指针检查；锦标赛副本清空轮次表后走 ConsistTournament 并
// 初始化 boss 计数/轮次；普通副本直接拷贝 CMap 怪物表。
bool CBattle_Field::get_monster_list(std::list<_mapMonster>& mobList)
{
    if (m_pDungeon == NULL || m_pMap == NULL || m_party == NULL)
        return false;
    if (m_pDungeon->isTournamentDungeon())
    {
        m_tournamentRoundMap.clear();                                    // +0x13c
        if (!ConsistTournament(m_pMap, mobList, m_tournamentRoundMap))
            return false;
        m_dungeonClearTracer.SetMaxBossCount(m_party->get_member_count());
        SetTournamentCurRound(1);
    }
    else
    {
        mobList = m_pMap->get_monster_list();   // ORIG list operator= 自临时副本
    }
    return true;
}

// ORIG 0x083013e2：boss 房条件副本（m_pDungeon->m_bossRoom90c.m_0==1）时，把
// 与条件坐标匹配的怪物从条件怪表并入 mobList。
void CBattle_Field::add_monster_list_bossroom_condition(std::list<_mapMonster>& mobList,
                                                        int x, int y)
{
    if (m_pDungeon == NULL || m_pMap == NULL)
        return;
    if (m_pDungeon->m_bossRoom90c.m_0 != 1)
        return;
    for (std::vector<stConditionEnterBossRoomStatus_t>::iterator cond =
             m_bossRoomConditions.begin();
         cond != m_bossRoomConditions.end(); ++cond)                       // +0x170
    {
        bool matched = false;
        for (std::vector<std::pair<int, int> >::iterator pos = cond->m_pos.begin();
             pos != cond->m_pos.end(); ++pos)
        {
            if (pos->first == x && pos->second == y)
            {
                matched = true;
                break;
            }
        }
        if (!matched)
            continue;
        std::list<_mapMonster> conditions = m_pMap->get_condition_monster_list();
        for (std::list<_mapMonster>::iterator mobIt = conditions.begin();
             mobIt != conditions.end(); ++mobIt)
        {
            if (mobIt->m_mobId == cond->m_monsterIdx)
                mobList.push_back(*mobIt);
        }
    }
}

// ORIG 0x0830162e：把 mobList 按 spawnType 展开生成到当前图。
// 命名怪条件：m_pDungeon->m_vec8f0 非空且 _mapMonster.m_champion 时查已清场表跳过。
void CBattle_Field::add_monsters_current_map(std::list<_mapMonster>& mobList,
                                             std::vector<int>& championList,
                                             map_monster& mob, int& count, int& uid,
                                             int& mapCount)
{
    if (m_pDungeon == NULL)
        return;
    int spawnCnt = 0;                          // 跨条目保留（ORIG local_18 初值 0）
    for (std::list<_mapMonster>::iterator it = mobList.begin();
         it != mobList.end();)
    {
        _mapMonster& ref = *it;
        if (!m_pDungeon->m_vec8f0.empty() && ref.m_champion &&
            checkClearedNamedMonsterMap(static_cast<unsigned char>(ref.m_posX),
                                        ref.m_mobId))
        {
            ++it;                    // 已清场命名怪：不生成不计数（仅推进迭代器）
            continue;
        }

        switch (ref.m_spawnType)
        {
        case 1:
            spawnCnt = (m_field198 <= 2) ? 1 : 2;                      // +0x198
            break;
        case 2:
            spawnCnt = m_field198;
            break;
        case 0:
            spawnCnt = 1;
            break;
        default:                   // 其余类型沿用上一条目数量（ORIG 不落任何赋值）
            break;
        }

        for (int s = 0; s < spawnCnt; ++s)
        {
            mob.m_mobId = ref.m_orderIdx;                              // ref+0x2c → mob+0x00
            if (m_villageMonsterId == 0 || m_party == NULL)            // +0x128
            {
                if (ref.m_flag04 == 0)
                {
                    mob.m_level =
                        (m_pDungeon->get_dimension_possible() < 1)
                            ? static_cast<char>(ref.m_levelAdd)
                            : static_cast<char>(m_hellPartyLevel);     // +0x238 低字节
                }
                else
                {
                    mob.m_level =
                        (m_pDungeon->get_dimension_possible() < 1)
                            ? static_cast<char>(m_pDungeon->get_standard_level() +
                                                static_cast<char>(ref.m_field08))
                            : static_cast<char>(m_hellPartyLevel);
                    if (mob.m_level == 0)
                        mob.m_level = 1;
                    if (static_cast<unsigned char>(mob.m_level) > 200)
                        mob.m_level = 200;
                }
            }
            else
            {
                mob.m_level = static_cast<char>(m_party->getMemberMaxLevel());
                if (mob.m_level == 0)
                    mob.m_level = 1;
                if (static_cast<unsigned char>(mob.m_level) > 200)
                    mob.m_level = 200;
            }

            if (ref.m_mobId == -1)
            {
                mob.m_mobIndex =
                    G_CDataManager()->m_monsterManager->get_levelrandom_mob(
                        static_cast<int>(static_cast<unsigned char>(mob.m_level)));
                const char* kind =
                    GetCurrentMapInfo()->m_layeredMap ? "layeredMap" : "generalMap";
                LogManager::logFormat(
                    1, "battle_field.cpp",
                    "void CBattle_Field::add_monsters_current_map(MOB_REF_LIST&, "
                    "std::vector<int, std::allocator<int> >&, map_monster&, int&, "
                    "int&, int&)",
                    0x5c0,
                    "HEROES_LOG : RANDOM MONSTER INDEX (%d) MONSTER LEVEL (%d) MAP "
                    "INDEX (%d) [Map Kind : %s]\n",
                    mob.m_mobIndex,
                    static_cast<unsigned int>(
                        static_cast<unsigned char>(mob.m_level)),
                    GetCurrentMapInfo()->m_mapIndex, kind);
            }
            else
            {
                mob.m_mobIndex = ref.m_mobId;
            }

            // mob+0x11(低字节)←ref.m_levelAdd、+0x12(高字节)←ref.m_field14（ORIG 逐字节写）
            mob.m_field11 = static_cast<short>(
                static_cast<unsigned char>(ref.m_levelAdd)
                | (static_cast<unsigned char>(ref.m_field14) << 8));
            mob.m_roleType = static_cast<char>(ref.m_roleType);        // mob+0x08 ← ref+0x18
            mob.m_instId = uid;
            uid = mob.m_instId + 1;
            mob.m_field20 = 0;
            mob.m_field24 = 0;

            if (mob.m_roleType == 0 && !mob.isNamedMonster() &&
                !m_pDungeon->isTournamentDungeon())
            {
                championList.push_back(mob.m_instId);
            }

            mob.m_field13 = ref.m_champion;                            // mob+0x13 ← ref+0x1c
            mob.m_field14 = static_cast<char>(ref.m_posX);             // mob+0x14 ← ref+0x20
            mob.m_mobList = ref.m_mobList;                             // mob+0x328 ← ref+0x34
            mob.m_dropCnt = ref.m_hpPct;                               // mob+0x2c ← ref+0x28
            mob.m_flag34 = static_cast<char>(ref.m_flag30);            // mob+0x34 ← ref+0x30

            GetCurrentMapInfo()->Add_Mob(mob);
            if (mob.m_field13 == 0)
                ++count;
            if (mob.m_roleType == 3 && GetCurrentMapInfo()->m_dummyBossMap != 1)
            {
                m_apcList.push_back(mob.m_instId);                     // +0x118
            }
        }
        ++it;
        ++mapCount;
    }
}

// ORIG 0x08301b8c：血战开启且组序表存在时转调 add_hell_party_monster。
void CBattle_Field::add_monster_hell_party(map_monster& mob, bool& killedAll,
                                           int& uid, int& seq)
{
    (void)killedAll;               // ORIG 未使用该引用
    if (m_hellPartyActive == 0)                                          // +0x1b8
        return;
    std::map<unsigned short, STHellPartyGroupOrder>* groups =
        static_cast<std::map<unsigned short, STHellPartyGroupOrder>*>(
            G_CDataManager()->get_hellparty_groupOrder_map(
                GetCurrentMapInfo()->m_mapIndex));
    if (groups == NULL)
        return;
    add_hell_party_monster(mob, uid, seq, groups);
}

// ORIG 0x08301bf6：血战系统/阶段时间自 CMap 复制到本对象后转调 add_blood_monster。
bool CBattle_Field::add_monster_blood(map_monster& mob, int& uid, int& seq)
{
    if (m_pMap == NULL)
        return false;
    if (m_pMap->m_bloodPhaseTime.empty())                                 // CMap+0xc4
        return true;
    m_bloodSystems = m_pMap->m_bloodSystem;                               // +0x240 ← +0xb8
    m_bloodPhaseTimes = m_pMap->m_bloodPhaseTime;                         // +0x24c ← +0xc4
    const bool ok = add_blood_monster(mob, uid, seq, m_pMap->m_bloodSystem,
                                      m_pMap->m_bloodPhaseTime);
    if (!ok)
    {
        LogManager::logFormat(
            1, "battle_field.cpp",
            __PRETTY_FUNCTION__,
            0x62b, "BLOOD_LOG : MONSTERS SETTING ERROR!!\n");
        return false;
    }
    return true;
}

// ORIG 0x08301d76：按地图 AI 编排表逐条生成 APC 怪物。
void CBattle_Field::add_monster_APC_AI(int& seq)
{
    if (m_pMap == NULL || m_pDungeon == NULL)
        return;
    const std::vector<STAICharacterMapArrangeData>* arranges =
        m_pMap->getAICharacters();
    if (arranges->empty())
        return;

    map_monster mob;               // 默认构造一次（循环体内 memset 复位）
    int apcIdx = 0;
    for (std::vector<STAICharacterMapArrangeData>::const_iterator it =
             arranges->begin();
         it != arranges->end(); ++it)
    {
        const STAICharacterMapArrangeData& arrange = *it;
        CAICharacter* ai = G_CDataManager()->m_aiCharacterList->get(static_cast<unsigned int>(arrange.m_type));
        if (ai == NULL)
        {
            MapInfo* info = GetCurrentMapInfo();
            LogManager::logFormat(
                1, "battle_field.cpp", __PRETTY_FUNCTION__,
                0x649,
                "Unknown APC Type(%d). In Dungeon(%d), Map(%d) [Map Kind : %s]",
                arrange.m_type, m_pDungeon->get_index(), m_pMap->get_index(),
                info->m_layeredMap ? "layeredMap" : "generalMap");
            continue;
        }

        // ORIG 在每次成功取表后 memset 整个 map_monster（0x34c 字节）复位；
        // 默认构造后的空 vector 内部为全零，memset 不改变其可析构状态。
        std::memset(&mob, 0, sizeof(map_monster));
        mob.m_mobId = apcIdx;
        mob.m_mobIndex = arrange.m_type;
        mob.m_instId = seq;
        seq = mob.m_instId + 1;
        mob.m_roleType = static_cast<char>(arrange.m_roleType);            // arrange+0x14
        if (mob.m_roleType == 8 && GetCurrentMapInfo()->m_dummyBossMap != 1)
        {
            m_apcList.push_back(mob.m_instId);
        }
        mob.m_level =
            (m_pDungeon->get_dimension_possible() < 1)
                ? static_cast<char>(ai->getLevel())
                : static_cast<char>(m_hellPartyLevel);
        mob.m_dropCnt = arrange.m_camp;                                    // arrange+0x10 → +0x2c
        // mob+0x11(低)←posY、+0x12(高)←posX（ORIG 逐字节写）
        mob.m_field11 = static_cast<short>(
            static_cast<unsigned char>(arrange.m_posY)
            | (static_cast<unsigned char>(arrange.m_posX) << 8));

        map_monster tmp(mob);
        GetCurrentMapInfo()->Add_Mob(tmp);
        ++apcIdx;
    }
}

// ORIG 0x083020b6：概率生成单只 APC（mobId 固定 0x40，roleType 5）。
void CBattle_Field::add_monster_APC_AI_by_rate(map_monster& mob, int& uid, int x, int y)
{
    if (m_pDungeon == NULL)
        return;
    if ((m_pos[0].first == x && m_pos[0].second == y) ||
        (m_startX == x && m_startY == y))                                  // +0xc/+0x10、+0x4/+0x8
    {
        return;
    }
    const int rate = m_pDungeon->getAICharacterAppearRate();
    if (rate == 0 || get_rand_int(100) >= rate)
        return;

    CAICharacter* ai =
        G_CDataManager()->m_aiCharacterList->getByLevel(
            static_cast<unsigned int>(m_pDungeon->get_standard_level()));
    if (ai == NULL)
        return;

    std::memset(&mob, 0, sizeof(map_monster));      // 同上：ORIG 显式 memset 0x34c
    mob.m_mobId = 0x40;
    mob.m_mobIndex = ai->getIdx();
    mob.m_instId = uid;
    uid = mob.m_instId + 1;
    mob.m_roleType = 5;
    mob.m_level =
        (m_pDungeon->get_dimension_possible() < 1)
            ? static_cast<char>(ai->getLevel())
            : static_cast<char>(m_hellPartyLevel);
    mob.m_dropCnt = 100;
    mob.m_field11 = 0;   // +0x11/+0x12 双字节清零

    map_monster tmp(mob);
    GetCurrentMapInfo()->Add_Mob(tmp);
}

// ORIG 0x083022a8：从候选表中随机抽 count 只追加为宝箱怪（每轮一次 rand 取槽）。
void CBattle_Field::add_monster_additional_champion(std::vector<int>& championList,
                                                    int count)
{
    for (int i = 0; i < count && !championList.empty(); ++i)
    {
        const size_t slot =
            rand() % championList.size();                                  // ORIG 单次 rand
        const int picked = championList[slot];
        championList.erase(championList.begin() +
                           static_cast<std::ptrdiff_t>(slot));
        map_monster& mob = GetCurrentMapInfo()->m_monsterMap[picked];
        if (mob.m_field13 == 0)                                            // +0x13
        {
            mob.m_roleType = 1;
        }
        else if (mob.m_roleType == 1)
        {
            LogManager::logFormat(
                1, "battle_field.cpp",
                "void CBattle_Field::add_monster_additional_champion("
                "std::vector<int, std::allocator<int> >&, int)",
                0x6bf, "Box Monster is Champion! id(%d)", picked);
        }
    }
}

// ORIG 0x083023ec：事件怪按 rival 表实例号区间（0x2710..）落到匹配坐标的事件位。
void CBattle_Field::add_monster_event(int& seq, int x, int y)
{
    if (m_pDungeon == NULL || m_pMap == NULL)
        return;
    if (m_eventMonsterStatus.empty())                                     // +0x17c
        return;

    int spawned = 0;
    const int monsterCnt =
        static_cast<int>(GetCurrentMapInfo()->m_monsterMap.size());
    unsigned int instBase = 10000;
    for (std::vector<stEventMonsterStatus_t>::const_iterator ev =
             m_eventMonsterStatus.begin();
         ev != m_eventMonsterStatus.end(); ++ev)
    {
        if (ev->m_pos.first != x || ev->m_pos.second != y)
            continue;
        if (m_pMap->m_rivalData2.empty())                                 // CMap+0x9c
            return;

        for (int k = 0; k < ev->m_monsterIdx; ++k)
        {
            map_monster mob;               // 每轮默认构造（ORIG ctor 081511c8）
            mob.m_mobIndex = ev->m_eventIdx;
            mob.m_level = static_cast<char>(m_pDungeon->get_standard_level());
            mob.m_mobId = static_cast<int>(instBase);
            // mob+0x11(低)←ev.m_field14、+0x12(高)←ev.m_field10（ORIG 逐字节写）
            mob.m_field11 = static_cast<short>(
                static_cast<unsigned char>(ev->m_field14)
                | (static_cast<unsigned char>(ev->m_field10) << 8));
            mob.m_roleType = 0;
            mob.m_field13 = 0;
            mob.m_field14 = 0;
            mob.m_dropCnt = 100;
            mob.m_flag34 = 0;
            mob.m_bloodRound = 1;                                          // +0x34a

            if (spawned < monsterCnt)
            {
                mob.m_instId = seq;
                seq = mob.m_instId + 1;
                map_monster tmp(mob);
                GetCurrentMapInfo()->Add_Mob(tmp);
                ++instBase;
                if (static_cast<unsigned int>(m_pMap->m_rivalData2.size()) +
                        10000u <=
                    instBase)
                {
                    instBase = 10000;
                }
            }
            ++spawned;
        }
    }
}

// ORIG 0x083026da：对当前图副本逐只计算基础能力并按难度/头目/冠军特性升级。
// ORIG 先复制整张 monsterMap 再遍历（能力写入落在副本上，随函数结束丢弃）——忠实保留。
void CBattle_Field::set_monster_ability()
{
    if (m_pDungeon == NULL)
        return;
    MapInfo* mapInfo = GetCurrentMapInfo();
    mapInfo->m_mapBasisLevel = rand();                                    // +0x48
    srand(static_cast<unsigned int>(mapInfo->m_mapBasisLevel));

    std::map<int, map_monster> monsters = GetCurrentMapInfo()->m_monsterMap;
    for (std::map<int, map_monster>::iterator it = monsters.begin();
         it != monsters.end(); ++it)
    {
        map_monster& mm = it->second;
        if (static_cast<signed char>(mm.m_mobIndex) >= 5)
            continue;
        CMonster* monster =
            static_cast<CMonster*>(G_CDataManager()->find_monster(mm.m_mobIndex));
        if (monster == NULL)
            continue;
        STMonsterScript* script =
            static_cast<STMonsterScript*>(monster->getMonsterScript());
        ActiveStaticInfo* active = &mm.m_active;

        setBasicAbilityByLevel(script, active,
                               static_cast<int>(static_cast<unsigned char>(mm.m_level)),
                               m_dungeonIndex,
                               static_cast<int>(static_cast<signed char>(mm.m_roleType)),
                               false, false);
        upgradeAbilityByMonsterParameterCategory(
            &script->m_parameterCategories, active, mm.m_roleType == 3);
        upgradeAbilityByDifficultyOfDungeon(get_dungeon_diff(), active,
                                            static_cast<int>(
                                                static_cast<unsigned char>(mm.m_level)),
                                            m_pDungeon->m_vec900);
        if (mm.m_roleType == 3)
        {
            upgradeAbilityByBossFeature(active);
        }
        else if (mm.m_roleType == 1 || mm.m_roleType == 2)
        {
            const int elem =
                getRandomChampionElement(script, mapInfo->m_mapBasisLevel);
            std::vector<int> features;
            makeChampionFeatureList(mm.m_mobList,
                                    static_cast<unsigned char>(mm.m_level),
                                    mapInfo->m_mapBasisLevel, features,
                                    get_dungeon_diff());
            upgradeAbilityByChampionFeature(elem,
                                            static_cast<signed char>(mm.m_roleType),
                                            &features, active);
        }
    }
}

// ORIG 0x083029b8：锦标赛副本把当前图怪物按轮次 1..4 分桶进 multimap。
void CBattle_Field::insert_tournamentRoundMonster()
{
    if (m_pDungeon == NULL || m_party == NULL)
        return;
    if (!m_pDungeon->isTournamentDungeon())
        return;
    std::multimap<int, map_monster>* tournamentMob =
        &GetCurrentMapInfo()->m_tournamentMob;                            // MapInfo+0xd0
    std::map<int, map_monster> monsters = GetCurrentMapInfo()->m_monsterMap;
    const int memberCount = m_party->get_member_count();

    for (int round = 1; round <= 4; ++round)
    {
        int n = 0;
        for (std::map<int, map_monster>::iterator it = monsters.begin();
             n < memberCount && it != monsters.end(); ++it, ++n)
        {
            tournamentMob->insert(std::make_pair(round, it->second));
        }
    }
}

// ORIG 0x08302bc6：按地图物件表与副本指定掉落生成地面物品。
// refData 三份上下文 blob 的字节布局按 ORIG 偏移直写（+0x34 区为生成器读取的
// 调用上下文；其中两处保存指针宽度数据，见各注释）。
// 注意：ORIG 在「assign==-1 且标准等级越界」分支直接跳回循环头而不推进迭代器
// （08302de5 jne 083030ad→083030ae，++it 位于 08303080 正常路径上）；脚本校验
// 保证标准等级恒在 1..200，该路径实际不可达。此处忠实保持同一控制流。
void CBattle_Field::add_item_curr_map(int& itemUid, int& itemIdx)
{
    if (m_pDungeon == NULL || m_pMap == NULL)
        return;

    GetCurrentMapInfo()->m_assignItemIdx = itemIdx;                       // MapInfo+0x3c

    std::list<_mapItem> itemList = m_pMap->get_item_list();
    const int curMapIndex = GetCurrentMapInfo()->m_mapIndex;              // 日志用
    std::vector<stDungeonAssignItem_t>* specialItems =
        m_pDungeon->getSpecialPassiveObjectItems();

    stGenerateRefData_t refRandom;                                        // A @-0xc4
    stGenerateRefData_t refSpecific;                                      // B @-0x10c
    stGenerateRefData_t refDrop;                                          // C @-0x154
    refRandom.m_field35 = static_cast<char>(get_dungeon_diff());
    refRandom.m_rateSlot = 1;
    // 【blob 指针槽】refDrop+0x34 区（m_field34..m_pad37 4 字节）按 ORIG 存
    // ObjectDropItems 指针；跨 4 个 char 字段，无具名表达，保留 reinterpret_cast。
    *reinterpret_cast<char**>(reinterpret_cast<char*>(&refDrop) + 0x34) =
        reinterpret_cast<char*>(m_pDungeon->getObjectDropItems());

    std::list<_mapItem>::iterator it = itemList.begin();
    while (it != itemList.end() && m_bloodState != 1)                     // +0x1b4
    {
        const int passiveIdx = it->m_objectIdx;
        const int field04 = it->m_field04;
        const int field08 = it->m_field08;
        const int field0c = it->m_field0c;

        if (passiveIdx < 0 ||
            static_cast<unsigned int>(passiveIdx) >= specialItems->size())
        {
            LogManager::logFormat(
                1, "battle_field.cpp",
                __PRETTY_FUNCTION__, 0x7a5,
                "Dungeon passive object index error! Map id:%d, DungeonAssignItem "
                "size:%d, Passive object id:%d",
                curMapIndex, static_cast<int>(specialItems->size()), passiveIdx);
            return;
        }

        stDungeonAssignItem_t& assign = specialItems->at(passiveIdx);
        if (assign.m_assignItemIdx != -1)
        {
            refRandom.m_field34 = static_cast<char>(assign.m_assignItemIdx);
            // ORIG byte 写 +0x38 = m_rankBonusIndex 低字节（int 槽内字节写）
            *reinterpret_cast<unsigned char*>(&refSpecific.m_rankBonusIndex) =
                static_cast<unsigned char>(assign.m_assignItemIdx);
        }
        else
        {
            const int stdLvl = m_pDungeon->get_standard_level();
            if (stdLvl >= 1 && stdLvl <= 200)
            {
                char lvl;
                if (m_pDungeon->get_dimension_possible() < 1)
                    lvl = static_cast<char>(m_pDungeon->get_standard_level());
                else
                    lvl = static_cast<char>(m_hellPartyLevel);
                refRandom.m_field34 = lvl;
                *reinterpret_cast<unsigned char*>(&refSpecific.m_rankBonusIndex) =
                    static_cast<unsigned char>(lvl);
            }
            else
            {
                continue;          // ORIG 直跳循环头（不推进迭代器，见函数头注释）
            }
        }

        // ORIG byte 写 +0x39 = m_rankBonusIndex 第 2 字节（保留 0/2/3 字节）
        refSpecific.m_rankBonusIndex =
            (refSpecific.m_rankBonusIndex & 0xffff00ffu)
            | (static_cast<unsigned char>(field04) << 8);
        refRandom.m_pad37 = static_cast<char>(field08);
        // 【blob 指针槽】refSpecific+0x34 区（m_field34..m_pad37 4 字节）按 ORIG
        // 存 assign.m_list 指针；跨 4 个 char 字段，无具名表达，保留 reinterpret_cast。
        *reinterpret_cast<char**>(reinterpret_cast<char*>(&refSpecific) + 0x34) =
            reinterpret_cast<char*>(&assign.m_list);                      // assign+0x04

        stGenerateResult_t result;
        CItemGenerator* gen = G_CDataManager()->getItemGenerator()->getGenerator(
            WongWork::CItemGeneratorMgr::eGenerateType_1);
        gen->generateRandomItem(refRandom, result);
        gen = G_CDataManager()->getItemGenerator()->getGenerator(
            WongWork::CItemGeneratorMgr::eGenerateType_1);
        gen->generateSpecificItem(refSpecific, result);
        gen = G_CDataManager()->getItemGenerator()->getGenerator(
            WongWork::CItemGeneratorMgr::eGenerateType_4);
        gen->generateSpecificItem(refDrop, result);

        for (std::vector<Inven_Item>::iterator dropIt = result.m_dropItems.begin();
             dropIt != result.m_dropItems.end(); ++dropIt)
        {
            map_item mi;
            mi.m_count = 1;
            mi.m_itemIndex = itemUid;
            itemUid = mi.m_itemIndex + 1;
            mi.m_standardLevel = static_cast<char>(field0c);
            mi.m_item = *dropIt;
            GetCurrentMapInfo()->Add_Item(mi);
        }
        ++it;
    }
}

// ---- moveMap_*_Log 族（0x0830766e / 0x08307740 / 0x08307812 / 0x083078e4）----

void CBattle_Field::moveMap_1_Log(int x, int y)
{
    MapInfo* info = GetMapInfoFromPos(x, y, false);
    LogManager::logFormat(
        1, "battle_field.cpp", __PRETTY_FUNCTION__, 0x1126,
        "TRY MoveMap [1][Dungeon Type : %d][Map Index : %d](%d,%d) -> (%d,%d) "
        "[Map Kind : %s]",
        m_dungeonIndex, GetMapInfoFromPos(x, y, false)->m_mapIndex, m_curX, m_curY,
        x, y, info->m_layeredMap ? "layeredMap" : "generalMap");
}

void CBattle_Field::moveMap_2_Log(int x, int y)
{
    MapInfo* info = GetMapInfoFromPos(x, y, false);
    LogManager::logFormat(
        1, "battle_field.cpp", __PRETTY_FUNCTION__, 0x1138,
        "TRY MoveMap [2][Dungeon Type : %d][Map Index : %d](%d,%d) -> (%d,%d) "
        "[Map Kind : %s]",
        m_dungeonIndex, GetMapInfoFromPos(x, y, false)->m_mapIndex, m_curX, m_curY,
        x, y, info->m_layeredMap ? "layeredMap" : "generalMap");
}

void CBattle_Field::moveMap_3_Log(int x, int y)
{
    MapInfo* info = GetMapInfoFromPos(x, y, false);
    LogManager::logFormat(
        1, "battle_field.cpp", __PRETTY_FUNCTION__, 0x114a,
        "TRY MoveMap [3][Dungeon Type : %d][Map Index : %d](%d,%d) -> (%d,%d) "
        "[Map Kind : %s]",
        m_dungeonIndex, GetMapInfoFromPos(x, y, false)->m_mapIndex, m_curX, m_curY,
        x, y, info->m_layeredMap ? "layeredMap" : "generalMap");
}

void CBattle_Field::moveMap_4_Log(int x, int y, const GridScript* gs)
{
    if (gs == NULL)
        return;
    const unsigned int layeredCount =
        static_cast<unsigned int>(gs->m_layeredMapIndexes.size());        // gs+0x1c
    MapInfo* info = GetMapInfoFromPos(m_curX, m_curY, false);
    LogManager::logFormat(
        1, "battle_field.cpp",
        __PRETTY_FUNCTION__, 0x115b,
        "TRY MoveMap [4] Error(G_CDataManager()->find_dungeon( m_Dungeon )) "
        "Dungeon[%d] Pos(%d,%d), gs->mapIndex(%d) [Map Kind : %s], "
        "gs->layeredMapIndex(%d)\n",
        m_dungeonIndex, m_curX, m_curY,
        static_cast<unsigned int>(gs->m_mapCandidates.size()),
        info->m_layeredMap ? "layeredMap" : "generalMap", layeredCount);
}

// ORIG 0x083079ba：终点位 boss 房条件未满足时报错码 7。
int CBattle_Field::check_boss_map(int x, int y)
{
    if (m_pos[0].first == x && m_pos[0].second == y &&                     // +0xc/+0x10
        m_pDungeon != NULL && m_pDungeon->m_bossRoom90c.m_0 == 1)
    {
        for (std::vector<stConditionEnterBossRoomStatus_t>::const_iterator it =
                 m_bossRoomConditions.begin();
             it != m_bossRoomConditions.end(); ++it)
        {
            if (it->m_curKillCount < it->m_maxKillCount)
            {
                LogManager::logFormat(
                    1, "battle_field.cpp",
                    __PRETTY_FUNCTION__, 0x1181,
                    "TRY MoveMap error condition [%d][%d]/[%d]", it->m_monsterIdx,
                    it->m_curKillCount, it->m_maxKillCount);
                return 7;
            }
        }
    }
    return 0;
}

// ORIG 0x08307b2a：相邻格（含斜向禁止）越界检查。
int CBattle_Field::check_movemap_range(int x, int y)
{
    int curX = -1;
    int curY = -1;
    getCurPosXY(curX, curY);
    int dx = x - curX;
    dx = (dx ^ (dx >> 31)) - (dx >> 31);
    int dy = y - curY;
    dy = (dy ^ (dy >> 31)) - (dy >> 31);
    if (dx <= 1 && dy <= 1 && !(dx == 1 && dy == 1))
        return 0;

    MapInfo* info = GetMapInfoFromPos(x, y, false);
    LogManager::logFormat(
        1, "battle_field.cpp", __PRETTY_FUNCTION__,
        0x119f,
        "TRY MoveMap [3][Dungeon Type : %d][Map Index : %d](%d,%d) -> (%d,%d) "
        "[Map Kind : %s]",
        m_dungeonIndex, GetMapInfoFromPos(x, y, false)->m_mapIndex, m_curX, m_curY,
        x, y, info->m_layeredMap ? "layeredMap" : "generalMap");
    return 0x15;
}

// ORIG 0x08307e44：以当前位置为基准的单向方向判定。
int CBattle_Field::get_invade_direction(int x, int y)
{
    int curX = -1;
    int curY = -1;
    getCurPosXY(curX, curY);
    int direction = 0;
    if (curX == x)
    {
        if (curY > y)
            direction = 8;
        if (curY < y)
            direction = 2;
    }
    if (curY == y)
    {
        if (curX < x)
            direction = 4;
        if (curX > x)
            direction = 1;
    }
    return direction;
}

// ORIG 0x08307d7c：按副本是否每图进入选用 goto 位或普通访问位判定。
bool CBattle_Field::check_visited_map(int x, int y, bool& firstVisit)
{
    const bool enterEach = (m_pDungeon != NULL) && m_pDungeon->IsEnterEachMap();
    if (enterEach)
    {
        if (is_visited_map_goto(x, y))
        {
            on_not_visited_map(x, y, firstVisit);
            return true;
        }
    }
    else
    {
        if (is_visited_map(x, y))
        {
            on_not_visited_map(x, y, firstVisit);
            return true;
        }
    }
    return false;
}

// ORIG 0x083086d2：写目标格/当前层 visited 标记。
void CBattle_Field::set_visited_map(int x, int y, bool visited)
{
    int curX = -1;
    int curY = -1;
    getCurPosXY(curX, curY);
    if (curX == x && curY == y)
    {
        if (getLayeredMapIndex() == 0)
            GetMapInfoFromPos(curX, curY, true)->m_visited = visited ? 1 : 0;
        else
            getLayeredMapInfoIndex(getLayeredMapIndex() - 1)->m_visited =
                visited ? 1 : 0;
    }
    else
    {
        GetCurrentMapInfo()->m_visited = visited ? 1 : 0;
    }
}

// ORIG 0x08308a24：写目标格/当前层 visitedGoto 标记。
void CBattle_Field::set_visited_map_goto(int x, int y)
{
    int lastX = -1;
    int lastY = -1;
    getLastPosXY(lastX, lastY);
    if (lastX == x && lastY == y)
    {
        if (getLayeredMapIndex() == 0)
            GetMapInfoFromPos(x, y, true)->m_visitedGoto = 1;
        else
            getLayeredMapInfoIndex(getLayeredMapIndex() - 1)->m_visitedGoto = 1;
    }
    else
    {
        GetMapInfoFromPos(x, y, false)->m_visitedGoto = 1;
    }
}

// ORIG 0x08307eca：dummy 图序列推进 / boss 位传送。
void CBattle_Field::choose_random_bossmap_from_dummymaps(const CDungeon* dungeon,
                                                         const GridScript* gs,
                                                         int& mapId, bool& flag,
                                                         int x, int y, int dir)
{
    if (dungeon == NULL || gs == NULL || m_gridMaze == NULL)
        return;

    if (dungeon->IsHaveDummyMap())
    {
        if (m_pos[0].first != x || m_pos[0].second != y)                  // +0xc/+0x10
            return;
        if (static_cast<unsigned int>(m_dummyMapPosIdx) <
            m_dummyMapPos.size())                                         // +0xcc < +0xc0.size
        {
            const std::pair<int, int>& next = m_dummyMapPos[m_dummyMapPosIdx];
            m_pos[0].first = next.first;
            m_pos[0].second = next.second;

            MapInfo* info = GetMapInfoFromPos(m_pos[0].first, m_pos[0].second, false);
            if (info->m_visited)
            {
                GetMapInfoFromPos(m_pos[0].first, m_pos[0].second, false)->m_visited = 0;
                GetMapInfoFromPos(m_pos[0].first, m_pos[0].second, false)->m_mapType = 2;
            }

            int curX = -1;
            int curY = -1;
            getCurPosXY(curX, curY);
            if (m_pos[0].first == curX && m_pos[0].second == curY)
            {
                flag = false;
                GetMapInfoFromPos(m_pos[0].first, m_pos[0].second, false)->m_mapType = 2;
            }

            mapId = dungeon->ChooseRandomMap(2, gs->m_gridValue, dir);
            GetMapInfoFromPos(x, y, false)->m_dummyBossMap = 1;
            m_dummyMapPosIdx = m_dummyMapPosIdx + 1;
        }
        else
        {
            MapInfo* info = GetMapInfoFromPos(x, y, false);
            if (info->m_dummyBossMap)
                GetMapInfoFromPos(x, y, false)->m_dummyBossMap = 0;
        }
    }
    else if (dungeon->m_entryRestrictFlag != 0)
    {
        const size_t count = m_gridMaze->m_vec20.size();                  // maze+0x20
        for (size_t i = 0; i < count; ++i)
        {
            const std::pair<int, int>& pos = m_gridMaze->m_vec20[i];
            if (pos.first == x && pos.second == y)
            {
                m_pos[0].first = x;                                       // +0xc
                m_pos[0].second = y;                                      // +0x10
                // ORIG 083081e6..083081eb：getGridR 返回引用地址直接写入 mapId。
                mapId = static_cast<int>(
                    reinterpret_cast<long>(&m_gridMaze->getGridR(x, y)));
                return;
            }
        }
    }
}

// ORIG 0x08308214：候选图收集（终点/起点任务/hell/普通四路分支）。
int CBattle_Field::get_candidator_map(const CDungeon* dungeon, const GridScript* gs,
                                      int& mapId, std::vector<int>& candidator,
                                      int x, int y, int dir)
{
    if (dungeon == NULL || gs == NULL)
        return 0x17;
    if (mapId != -1)
        return 0;

    if (m_pos[0].first == x && m_pos[0].second == y)                      // +0xc/+0x10
    {
        if (gs->m_secondCandidates.empty())                               // gs+0x10
        {
            mapId = dungeon->ChooseRandomMap(1, gs->m_gridValue, dir);
        }
        else
        {
            const size_t n = gs->m_secondCandidates.size();
            for (size_t i = 0; i < n; ++i)
            {
                CMap* map = static_cast<CMap*>(
                    G_CDataManager()->find_map(gs->m_secondCandidates[i]));
                if (map == NULL)
                    return 0x17;
                if (map->CheckEntrance(dir))
                    candidator.push_back(gs->m_secondCandidates[i]);
            }
        }
    }
    else if (m_pos[1].first == x && m_pos[1].second == y &&               // +0x14/+0x18
             m_questStageMap != 0)                                        // +0x19c
    {
        mapId = m_questStageMap;
        if (mapId < 1)
        {
            MapInfo* info = GetMapInfoFromPos(m_curX, m_curY, false);
            LogManager::logFormat(
                1, "battle_field.cpp",
                "int CBattle_Field::get_candidator_map(const CDungeon*, "
                "const GridScript*, int&, INT_VECTOR&, int, int, int)", 0x12bb,
                "[CBattle_Field::MoveMap] Error(quest_stage_map_==-1) Dungeon[%d] "
                "Pos(%d,%d), candidator(%d), gs->mapIndex(%d), [Map Kind : %s], "
                "gs->layeredMapIndex(%d)\n",
                m_dungeonIndex, m_curX, m_curY,
                static_cast<unsigned int>(candidator.size()),
                static_cast<unsigned int>(gs->m_mapCandidates.size()),
                info->m_layeredMap ? "layeredMap" : "generalMap",
                static_cast<unsigned int>(gs->m_layeredMapIndexes.size()));
        }
    }
    else if (m_hellPosX == x && m_hellPosY == y && m_hellPartyActive)     // +0x1c0/+0x1c4/+0x1b8
    {
        mapId = m_hellMapIdx;                                             // +0x1bc
        m_visitedGotoFlag = 1;                                            // +0x1c8
    }
    else
    {
        if (gs->m_mapCandidates.empty())                                  // gs+0x04
        {
            mapId = dungeon->ChooseRandomMap(0, gs->m_gridValue, dir);
        }
        else
        {
            const size_t n = gs->m_mapCandidates.size();
            for (size_t i = 0; i < n; ++i)
            {
                CMap* map = static_cast<CMap*>(
                    G_CDataManager()->find_map(gs->m_mapCandidates[i]));
                if (map == NULL)
                    return 0x17;
                if (map->CheckEntrance(dir))
                    candidator.push_back(gs->m_mapCandidates[i]);
            }
            if (!GetCurrentMapInfo()->m_layeredMap && candidator.empty())
            {
                for (size_t i = 0; i < n; ++i)
                {
                    (void)G_CDataManager()->find_map(gs->m_mapCandidates[i]);
                    LogManager::logFormat(
                        1, "battle_field.cpp",
                        "int CBattle_Field::get_candidator_map(const CDungeon*, "
                        "const GridScript*, int&, INT_VECTOR&, int, int, int)",
                        0x12e4,
                        "[CBattle_Field::MoveMap] Error(< map_candidator.size()==0) "
                        "Dungeon[%d] Pos(%d,%d), (Direction: %d) \n",
                        m_dungeonIndex, m_curX, m_curY, dir);
                    LogManager::logFormat(
                        1, "battle_field.cpp",
                        "int CBattle_Field::get_candidator_map(const CDungeon*, "
                        "const GridScript*, int&, INT_VECTOR&, int, int, int)",
                        0x12e5,
                        "[CBattle_Field::MoveMap] Error(< map_candidator.size()==0) "
                        "gs->mapIndexes_[%d]=%d \n",
                        static_cast<int>(i), gs->m_mapCandidates[i]);
                }
            }
        }
    }
    return 0;
}

// ORIG 0x08308882：确定最终图号并写回当前图信息。
int CBattle_Field::set_map_id(const GridScript* gs, int& mapId,
                              std::vector<int>& candidator)
{
    if (gs == NULL)
        return 0x15;

    if (mapId < 0)
    {
        if (!candidator.empty())
            mapId = candidator[get_rand_int(static_cast<int>(candidator.size()))];
        if (GetCurrentMapInfo()->m_layeredMap)
            mapId = gs->m_layeredMapIndexes[getLayeredMapIndex()];
    }

    GetCurrentMapInfo()->m_mapIndex = mapId;
    if (mapId < 1)
    {
        MapInfo* info = GetMapInfoFromPos(m_curX, m_curY, false);
        LogManager::logFormat(
            1, "battle_field.cpp",
            __PRETTY_FUNCTION__,
            0x1363,
            "[CBattle_Field::MoveMap] Error Dungeon[%d] Pos(%d,%d), candidator(%d), "
            "gs->mapIndex(%d) [Map Kind : %s], gs->layeredMapIndex(%d)\n",
            m_dungeonIndex, m_curX, m_curY,
            static_cast<unsigned int>(candidator.size()),
            static_cast<unsigned int>(gs->m_mapCandidates.size()),
            info->m_layeredMap ? "layeredMap" : "generalMap",
            static_cast<unsigned int>(gs->m_layeredMapIndexes.size()));
        return 0x15;
    }
    return 0;
}
