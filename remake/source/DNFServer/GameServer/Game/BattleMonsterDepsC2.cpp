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

#include "CBattle_Field.h"
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h      // CBattle_Field + WarField/CMap/CDungeon/CParty/
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
    // P0/P209：ORIG -0x10 槽为 unsigned（`cmpl $0x62,-0x10` 直读内存 + jbe），
    // 且 `add 0xc(%ebp),%eax` 无装载中转 ⇒ 表达式不带 int cast。
    // P183/P48：empty 判定的地址全量物化一次（不入槽），判定后再物化并存
    // -0xc 槽供 size/at 两次复用（ORIG 0x0834aae5/0x0834aae8 实证回读）。
    unsigned int idx = static_cast<unsigned int>(get_rand_int(10)) + level - 5;
    if (idx == 0 || idx > 0x62)
        return 0;
    if (m_byLevelList[idx].empty())
        return 0;
    // ORIG 第二次物化走 `lea 0x18(%eax),%ecx; lea (%ecx,%eax,1)`（数组基址先算）
    // ⇒ 用数组名退化的指针算术（`arr + idx`）而非下标形。
    std::vector<CAICharacter*>* bucket = m_byLevelList + idx;
    return bucket->at(get_rand_int(static_cast<int>(bucket->size())));
}

// ============================ CBattle_Field ================================

// ORIG 0x08300ed4：find_map(mapIndex)；失败按当前图种类记日志返回 0；
// 困难 4 且地图 m_b4 非零时改写当前图号后二次查找（再失败同样记日志 0x4c4）。
CMap* CBattle_Field::getMap(int mapIndex)
{
    CMap* map = static_cast<CMap*>(G_CDataManager()->find_map(mapIndex));
    if (map == NULL)
    {
        LogManager::logFormat(
            1, "battle_field.cpp", __PRETTY_FUNCTION__, 0x4a7,
            "Find Map[%d] Error(In Map Maker)(%d/%d) [Map Kind : %s]",
            GetCurrentMapInfo()->m_mapIndex, m_curX, m_curY,
            GetCurrentMapInfo()->m_layeredMap ? "layeredMap" : "generalMap");
        return NULL;
    }
    if (get_dungeon_diff() == 4 && map->m_b4 != 0)
    {
        GetCurrentMapInfo()->m_mapIndex = map->m_b4;
        map = static_cast<CMap*>(G_CDataManager()->find_map(map->m_b4));
        if (map == NULL)
        {
            LogManager::logFormat(
                1, "battle_field.cpp", __PRETTY_FUNCTION__, 0x4c4,
                "Find Map[%d] Error(In Map Maker)(%d/%d) [Map Kind : %s]",
                GetCurrentMapInfo()->m_mapIndex, m_curX, m_curY,
                GetCurrentMapInfo()->m_layeredMap ? "layeredMap" : "generalMap");
            return NULL;
        }
    }
    return map;
}

// ORIG 0x0830107e：四段直写 m_dungeonClearTracer。
// [b28d-D9] N95：ORIG 4.1 dead 局部 -0x4(%ebp) 单条 store；4.4 槽基恒 ebp-8（P232/P190 均不可上移），残 1 条（ORIG 0x0830107e sz 0x0000006f）
void CBattle_Field::setDungeonClearTrace(int mapId, int maxBoss, int killedBoss,
                                         int killedMonster)
{
    m_dungeonClearTracer.SetMapId(mapId);
    m_dungeonClearTracer.SetMaxBossCount(maxBoss);
    m_dungeonClearTracer.SetKilledBossCount(killedBoss);
    m_dungeonClearTracer.SetKilledMonsterCount(killedMonster);
    // ORIG 0x083010e4 尾部 `movl $0x0,-0x4(%ebp)`（4.1 dead 局部顶到 ebp-4）。
    int dummy = 0;
    (void)dummy;
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
        bool ok = ConsistTournament(m_pMap, mobList, m_tournamentRoundMap);
        if (!ok)
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
// [b28d-solo-BattleMonsterDepsC2] P214①/P13（已修 168→175，正文逐条全等）：ORIG 无 matched
// 具名 bool/无 break——匹配即执行内层 list 拷贝+push_back 后继续遍历（`mov $1/$0` 短路物化）；
// 外层 const_iterator 转换构造 + begin/end 各一次（4 槽），内层 const_iterator 缓存 end；
// conditions 为 const 引用绑定临时（ORIG local_10 指针槽，无 4.4 值拷贝）。
// [SweepBattleMonster 2026-09-09] 残差 N121：尾部 4.4 三层循环出口各发 nop+jmp 岛（ORIG 2 岛，
// 第三层直落 lea esp）⇒ clean 171 vs 172（+1 条）。
void CBattle_Field::add_monster_list_bossroom_condition(std::list<_mapMonster>& mobList,
                                                        int x, int y)
{
    // [b28d-Lane27] ORIG 0x083013f5/0x08301406 两条独立 `je`（各带岛）+ 0x0830141e
    // `jne <epilogue>`（直跳）⇒ 拆三条独立守卫。
    // [round5-C05 2026-09-10 实测] D08 合并形（单条短路 `||` 守卫 / goto 尾标签两形
    // 均试）对本件**零收益**（仍 our 176 / ORIG 173）；残余 1 对 `jmp epilogue;nop`
    // 属 4.4 epilogue-threading/rel8 跳板（A27/A28 边界：源码层不可控）⇒ 恢复
    // ORIG 直对应的三条独立守卫并封顶。
    if (m_pDungeon == NULL)
        return;
    if (m_pMap == NULL)
        return;
    if (m_pDungeon->m_bossRoom90c.m_0 != 1)
        return;
    // ORIG：外层 const_iterator（begin/end 各转换构造一次，4 槽）+ 缓存 end；内层
    // const_iterator 无转换（begin() 已返 const）+ 缓存 end（P13）。
    std::vector<stConditionEnterBossRoomStatus_t>::const_iterator cond =
        m_bossRoomConditions.begin();
    std::vector<stConditionEnterBossRoomStatus_t>::const_iterator condEnd =
        m_bossRoomConditions.end();
    for (; cond != condEnd; ++cond)                                       // +0x170
    {
        std::vector<std::pair<int, int> >::const_iterator pos =
            cond->m_pos.begin();
        std::vector<std::pair<int, int> >::const_iterator posEnd =
            cond->m_pos.end();
        for (; pos != posEnd; ++pos)
        {
            if (pos->first == x && pos->second == y)
            {
                const std::list<_mapMonster>& conditions =
                    m_pMap->get_condition_monster_list();
                for (std::list<_mapMonster>::const_iterator mobIt =
                         conditions.begin();
                     mobIt != conditions.end(); ++mobIt)
                {
                    if (mobIt->m_mobId == cond->m_monsterIdx)
                        mobList.push_back(*mobIt);
                }
            }
        }
    }
}

// ORIG 0x0830162e：把 mobList 按 spawnType 展开生成到当前图。
// 命名怪条件：m_pDungeon->m_vec8f0 非空且 _mapMonster.m_champion 时查已清场表跳过。
// [SweepBattleMonster 2026-09-09] 已修（399 vs 398，clean 397=397，DIFF→near）：①P167 end()
// 每轮回边重求值（不缓存 itEnd）；②P214① 正极性 `m_villageMonsterId!=0 && m_party!=NULL`
// 包裹队伍分支、普通分支内 `ref.m_flag04!=0` 在前；③真 bug：flag==0 分支等级来源应为
// ref+0x8（m_field08，ORIG 0x08301893 `mov 0x8(%eax),%eax`），旧写 m_levelAdd(+0x10)；
// ④P226 语句表达式复现 `!empty() && champion` 物化岛；⑤P324 case 升序(0,1,2)；⑥P225
// `(signed char)dim>0`+if/else 复现 setg；⑦__PRETTY_FUNCTION__；⑧P231 register 复用 uid。
// 残差仅 uid 值落 %ebx vs ORIG %eax（N7 寄存器角色，指令数/序列全等）。
void CBattle_Field::add_monsters_current_map(std::list<_mapMonster>& mobList,
                                             std::vector<int>& championList,
                                             map_monster& mob, int& count, int& uid,
                                             int& mapCount)
{
    if (m_pDungeon == NULL)
        return;
    int spawnCnt = 0;                          // 跨条目保留（ORIG local_18 初值 0）
    // ORIG local_18(-0x14) = GetCurrentMapInfo() 一次缓存（Ghidra `local_18 = (MapInfo *)GetCurrentMapInfo(this)`），
    // 其后 m_layeredMap/m_mapIndex/Add_Mob/m_dummyBossMap 全走该指针（p167 EXTRA GetCurrentMapInfo×3）。
    MapInfo* mapInfo = GetCurrentMapInfo();
    // [b28d-D9] P167：ORIG 用 `_List_const_iterator` + 后置 `operator++(int)`；end() 在
    // 循环尾每轮重新求值（`lea -0x370; call end; C1ERK` 位于 +0x500 回边处）⇒ 不缓存 itEnd。
    for (std::list<_mapMonster>::const_iterator it = mobList.begin();
         it != mobList.end();)
    {
        const _mapMonster& ref = *it;
        // P226 条件向：ORIG 前两条件先物化 `mov $1/$0; test %al,%al; je`（无槽）⇒
        // 语句表达式包住 `!empty() && champion`，checkCleared 作为嵌套内层条件。
        if (({ !m_pDungeon->m_vec8f0.empty() && ref.m_champion; }))
        {
            if (checkClearedNamedMonsterMap(static_cast<unsigned char>(ref.m_posX),
                                            ref.m_mobId))
            {
                it++;                // 已清场命名怪：不生成不计数（仅推进迭代器）
                continue;
            }
        }

        switch (ref.m_spawnType)
        {
        case 0:
            spawnCnt = 1;
            break;
        case 1:
            if (m_field198 > 2)
                spawnCnt = 2;
            else
                spawnCnt = 1;
            break;                                                     // +0x198
        case 2:
            spawnCnt = m_field198;
            break;
        default:                   // 其余类型沿用上一条目数量（ORIG 不落任何赋值）
            break;
        }

        for (int s = 0; s < spawnCnt; ++s)
        {
            mob.m_mobId = ref.m_orderIdx;                              // ref+0x2c → mob+0x00
            // P214①/P133：ORIG 两条 je 同落普通分支（+0x1b6）⇒ 正极性 `!=0 && !=NULL`
            // 包裹队伍分支；普通分支内 flag!=0 代码在前（ORIG +0x1c1），flag==0 在后
            // （+0x235），且 flag==0 分支等级来源 = ref+0x8（Ghidra local_14[2]）。
            if (m_villageMonsterId != 0 && m_party != NULL)            // +0x128
            {
                mob.m_level = static_cast<char>(m_party->getMemberMaxLevel());
                if (mob.m_level == 0)
                    mob.m_level = 1;
                if (static_cast<unsigned char>(mob.m_level) > 200)
                    mob.m_level = 200;
            }
            else
            {
                if (ref.m_flag04 != 0)
                {
                    // P225：ORIG `test %al,%al; setg %al; test %al,%al; je`（有符号
                    // `>0` 物化）⇒ 显式 (signed char) 比较 + if/else（三目被折成统一写）。
                    if (static_cast<signed char>(
                            m_pDungeon->get_dimension_possible()) > 0)
                        mob.m_level = static_cast<char>(m_standardDimensionLevel);
                    else
                        mob.m_level = static_cast<char>(
                            m_pDungeon->get_standard_level() +
                            static_cast<char>(ref.m_field08));
                    if (mob.m_level == 0)
                        mob.m_level = 1;
                    if (static_cast<unsigned char>(mob.m_level) > 200)
                        mob.m_level = 200;
                }
                else
                {
                    if (static_cast<signed char>(
                            m_pDungeon->get_dimension_possible()) > 0)
                        mob.m_level = static_cast<char>(m_standardDimensionLevel);
                    else
                        mob.m_level = static_cast<char>(ref.m_field08); // +0x238 低字节
                }
            }

            if (ref.m_mobId == -1)
            {
                mob.m_mobIndex =
                    G_CDataManager()->m_monsterManager->get_levelrandom_mob(
                        static_cast<int>(static_cast<unsigned char>(mob.m_level)));
                LogManager::logFormat(
                    1, "battle_field.cpp",
                    __PRETTY_FUNCTION__,
                    0x5c0,
                    "HEROES_LOG : RANDOM MONSTER INDEX (%d) MONSTER LEVEL (%d) MAP "
                    "INDEX (%d) [Map Kind : %s]\n",
                    mob.m_mobIndex,
                    static_cast<unsigned int>(
                        static_cast<unsigned char>(mob.m_level)),
                    mapInfo->m_mapIndex,
                    mapInfo->m_layeredMap ? "layeredMap" : "generalMap");
            }
            else
            {
                mob.m_mobIndex = ref.m_mobId;
            }

            // mob+0x11(低字节)←ref.m_levelAdd、+0x12(高字节)←ref.m_field14（ORIG 逐字节写）
            mob.m_field11b.m_field11b0 = static_cast<unsigned char>(ref.m_levelAdd);
            mob.m_field11b.m_field11b1 = static_cast<unsigned char>(ref.m_field14);
            mob.m_roleType = static_cast<char>(ref.m_roleType);        // mob+0x08 ← ref+0x18
            // P231：ORIG 读 uid 入 %eax 后跨 store 复用（`lea 0x1(%eax),%edx`）；裸
            // register 提示让 4.4 免槽保活（非 asm 绑定，P0 合规）。
            register int curUid = uid;
            mob.m_instId = curUid;
            uid = curUid + 1;
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

            mapInfo->Add_Mob(mob);
            if (mob.m_field13 == 0)
                ++count;
            if (mob.m_roleType == 3 && !mapInfo->m_dummyBossMap)
            {
                m_apcList.push_back(mob.m_instId);                     // +0x118
            }
        }
        it++;
        ++mapCount;
    }
}

// ORIG 0x08301b8c：血战开启且组序表存在时转调 add_hell_party_monster。
void CBattle_Field::add_monster_hell_party(map_monster& mob, bool& killedAll,
                                           int& uid, int& seq)
{
    // P63/P133：ORIG 双 je 共享统一出口（+0x64）——嵌套 if 共享出口消 jmp 中继。
    if (m_hellPartyActive != 0)                                          // +0x1b8
    {
        std::map<unsigned short, STHellPartyGroupOrder>* groups =
            static_cast<std::map<unsigned short, STHellPartyGroupOrder>*>(
                G_CDataManager()->get_hellparty_groupOrder_map(
                    GetCurrentMapInfo()->m_mapIndex));
        if (groups != NULL)
            add_hell_party_monster(mob, uid, seq, groups);
    }
}

// ORIG 0x08301bf6：血战系统/阶段时间自 CMap 复制到本对象后转调 add_blood_monster。
// [b28d-solo-BattleMonsterDepsC2] P197④/P225/P0：ORIG `call size; test %eax; setne %al;
// test %al; je` ⇒ 源写 `size() != 0`（empty() 生 test %al 直跳）；且 ORIG 无 ok 具名局部
// （`mov %eax,%ebx; xor $1,%ebx` 保活跨析构）⇒ 条件内联表达式，去掉 `const bool ok` 槽。
bool CBattle_Field::add_monster_blood(map_monster& mob, int& uid, int& seq)
{
    if (m_pMap == NULL)
        return false;
    if (m_pMap->m_bloodPhaseTime.size() != 0)                             // CMap+0xc4
    {
        m_bloodSystems = m_pMap->m_bloodSystem;                           // +0x240 ← +0xb8
        m_bloodPhaseTimes = m_pMap->m_bloodPhaseTime;                     // +0x24c ← +0xc4
        if (!add_blood_monster(mob, uid, seq, m_pMap->m_bloodSystem,
                               m_pMap->m_bloodPhaseTime))
        {
            LogManager::logFormat(
                1, "battle_field.cpp",
                __PRETTY_FUNCTION__,
                0x62b, "BLOOD_LOG : MONSTERS SETTING ERROR!!\n");
            return false;
        }
    }
    return true;
}

// ORIG 0x08301d76：按地图 AI 编排表逐条生成 APC 怪物。
// [b28d-solo-BattleMonsterDepsC2] 已修：`mob.m_instId = seq++`（ORIG 单次读 seq + lea 0x1）；
// m_level 三目改 (signed char)dim > 0 形（ORIG `test %al; setg %al; test; je`）。
// [SweepBattleMonster 2026-09-09] 已修（213 vs 212，clean 211 vs 209）：①删 `map_monster tmp`
// 直接 `Add_Mob(mob)`（省一个 0x350 临时，帧 0xa30→0x6f0，ORIG 0x6e0）；②m_field11 字节视图
// 复现 +0x11/+0x12 双 movb（WarField.h 匿名 union）；③`!m_dummyBossMap`（字段 bool 化）。
// [SweepBattleMonsterDeps 2026-09-10] 归零（211=211，ndiff 152→0，AE）：①apcIdx 声明提到
// arranges 之前 + 循环前重置（ORIG 0x08301da3/0x08301de2 双清零，4.1「先声明更深」）；
// ②begin/end 各调一次并缓存 itEnd（P13；ORIG -0x368/-0x36c 两独立槽，循环尾只 operator++/!=）；
// ③`if (!empty())` 正向包裹（P214①，ORIG `xor $1; test; je`）；④删 info 具名槽（P0，内联
// GetCurrentMapInfo()）；⑤m_level 改语句表达式 + if/else 复现 `setg` 物化（P226/P225）。
// 帧 0x6f0→0x6e0=ORIG，p167 27=27 CLEAR（仅 map_monster C1/C2、D1/D2 同址别名差）。
void CBattle_Field::add_monster_APC_AI(int& seq)
{
    if (m_pMap == NULL || m_pDungeon == NULL)
        return;
    // ORIG 0x08301da3：apcIdx(-0x18) 的初始化发射在 getAICharacters 之前（声明点在
    // arranges 之前，4.1「先声明更深」），且 mob 构造后 0x08301de2 再清零一次
    // ⇒ 原码为「函数头声明初始化 + 循环前重置」两段形。
    int apcIdx = 0;
    const std::vector<STAICharacterMapArrangeData>* arranges =
        m_pMap->getAICharacters();
    // P214①：ORIG 0x08301dc9 `call empty; xor $0x1,%eax; test %al,%al; je <函数尾>`
    // （正向包裹形）⇒ 主体包进 `if (!empty())`；早退 `if (empty()) return;` 4.4 产
    // `test; jne`（少 1 条且块布局反）。
    if (!arranges->empty())
    {
        map_monster mob;           // 默认构造一次（循环体内 memset 复位）
        apcIdx = 0;
        // ORIG 0x08301df9/0x08301e11：begin()/end() 各调一次并缓存（-0x368/-0x36c），
        // 循环尾只调 operator++ 与 operator!=（P13 缓存 end）。
        std::vector<STAICharacterMapArrangeData>::const_iterator it =
            arranges->begin();
        std::vector<STAICharacterMapArrangeData>::const_iterator itEnd =
            arranges->end();
        for (; it != itEnd; ++it)
        {
            const STAICharacterMapArrangeData& arrange = *it;
            CAICharacter* ai = G_CDataManager()->m_aiCharacterList->get(static_cast<unsigned int>(arrange.m_type));
            if (ai == NULL)
            {
                // P0：ORIG 0x08301e5a 段无 info 具名槽（call 结果直接 movzbl 0xe8(%eax)）
                LogManager::logFormat(
                    1, "battle_field.cpp", __PRETTY_FUNCTION__,
                    0x649,
                    "Unknown APC Type(%d). In Dungeon(%d), Map(%d) [Map Kind : %s]",
                    arrange.m_type, m_pDungeon->get_index(), m_pMap->get_index(),
                    GetCurrentMapInfo()->m_layeredMap ? "layeredMap" : "generalMap");
                continue;
            }

            // ORIG 在每次成功取表后 memset 整个 map_monster（0x34c 字节）复位；
            // 默认构造后的空 vector 内部为全零，memset 不改变其可析构状态。
            std::memset(&mob, 0, sizeof(map_monster));
            mob.m_mobId = apcIdx;
            mob.m_mobIndex = arrange.m_type;
            mob.m_instId = seq++;
            mob.m_roleType = static_cast<char>(arrange.m_roleType);        // arrange+0x14
            if (mob.m_roleType == 8 && !GetCurrentMapInfo()->m_dummyBossMap)
            {
                m_apcList.push_back(mob.m_instId);
            }
            // P226/P225：ORIG 0x08301f93 `test %al,%al; setg %al; test %al,%al; je`（比较
            // 结果零槽物化）⇒ 语句表达式包住条件、if/else 分臂（三目被 4.4 折成 `jle` 直跳）。
            if (({ static_cast<signed char>(
                       m_pDungeon->get_dimension_possible()) > 0; }))
                mob.m_level = static_cast<char>(m_standardDimensionLevel);
            else
                mob.m_level = static_cast<char>(ai->getLevel());
            mob.m_dropCnt = arrange.m_camp;                                // arrange+0x10 → +0x2c
            // mob+0x11(低)←posY、+0x12(高)←posX（ORIG 逐字节写）
            mob.m_field11b.m_field11b0 = static_cast<unsigned char>(arrange.m_posY);
            mob.m_field11b.m_field11b1 = static_cast<unsigned char>(arrange.m_posX);

            GetCurrentMapInfo()->Add_Mob(mob);
            ++apcIdx;
        }
    }
}

// ORIG 0x083020b6：概率生成单只 APC（mobId 固定 0x40，roleType 5）。
// [b28d-solo-BattleMonsterDepsC2] 已修：uid 递增改 `uid = uid + 1`（ORIG `lea 0x1(%eax)` 复用
// 寄存器；`mob.m_instId + 1` 会重读 mob+0x4）；m_level 条件加 (signed char)。
// [SweepBattleMonster 2026-09-09] 已修（140 vs 146）：①`int mobId=0x40` 具名局部 + `mobId++`
// 后置（ORIG 0x083020d2 store / 0x08302296 `addl $0x1,-0x10`）；②P0 删 `map_monster tmp` 直接
// `Add_Mob(mob)`（帧 0x6b0→0x370=ORIG）；③m_field11 字节视图双 movb；④P231 register 复用 uid。
// 残差 N：rate()!=0 的 `setne` 与 rate()>rand 的 `setg` 物化（P226 语句表达式实测 +5 条反劣）、
// dim>0 的 if/else 尾部三 jmp 岛、uid 落 %ebx（N7）。
void CBattle_Field::add_monster_APC_AI_by_rate(map_monster& mob, int& uid, int x, int y)
{
    if (m_pDungeon == NULL)
        goto apc_end;   // [round5-C05] P223 尾标签串：ORIG 三守卫直跳 epilogue
                        // （仅 1 个对齐 nop 岛），平铺 `return;` 令 4.4 各发
                        // `jmp epilogue;nop` 中继岛 ×4 ⇒ 改单尾标签。
    // P0：ORIG 0x083020e5 `movl $0x40,-0x10(%ebp)`（具名局部，非直填立即数）
    int mobId;
    mobId = 0x40;
    if ((m_endX == x && m_endY == y) ||
        (m_startX == x && m_startY == y))                                  // +0xc/+0x10、+0x4/+0x8
    {
        goto apc_end;
    }
    // [b23S3C] P48 双取：ORIG getAICharacterAppearRate() 每判各调一次
    // （非空检查 1 次、rand 比较 1 次存 %ebx 跨 get_rand_int call）；
    // EXTRA map_monster C1ERKS/D1 = 4.4 Add_Mob 大对象 by-value caller
    // 复制（P67 跨版本 ABI 豁免）。
    // ORIG 首条件 `test; setne %al; test %al,%al; je`、次条件 `cmp %eax,%ebx; setg %al;
    // test; je`。
    // [b28f-L27·P225 推广] 拆**嵌套 if**（合并短路链被 4.4 折成直跳 `je/jle`，恒不产
    //   setcc）⇒ 两处 setne/setg 物化逐条复现，142→141。
    //   余 2 条 = ①`mob.m_mobId = mobId` 的 RHS-first+`mov %eax,%edx` 中转（P211 档③
    //   要求 map_monster::m_mobId 与 RHS 异型，属 MonsterIndependentDrop.h 建模，
    //   跨 TU 勿动）；②尾部我方多 3 个 `jmp` 岛（P223 早返岛数，ORIG 1 个）⇒ 按 N。
    if (m_pDungeon->getAICharacterAppearRate() != 0)
    {
      if (m_pDungeon->getAICharacterAppearRate() > get_rand_int(100))
      {
        CAICharacter* ai =
            G_CDataManager()->m_aiCharacterList->getByLevel(
                static_cast<unsigned int>(m_pDungeon->get_standard_level()));
        if (ai == NULL)
            goto apc_end;

        std::memset(&mob, 0, sizeof(map_monster));  // ORIG 显式 memset 0x34c
        // [b28d-Lane27] ORIG 0x083021ac 取旧值写 mob+0、自增在**函数尾** 0x08302296
        // （Add_Mob 之后 `addl $0x1,-0x10(%ebp)`）⇒ 拆两句并把 `++mobId` 后置。
        mob.m_mobId = mobId;
        mob.m_mobIndex = ai->getIdx();
        // P231：ORIG 读 uid 入 %eax 后跨 store 复用（`lea 0x1(%eax),%edx`）⇒ 裸 register
        // 提示（非 asm 绑定，P0 合规）。
        register int curUid = uid;
        mob.m_instId = curUid;
        uid = curUid + 1;
        mob.m_roleType = 5;
        // [b23S3C] P47：ORIG 真臂 fall-through（dim>0 读 +0x238），getLevel
        // 在 je 臂；m_field11 short=0 产单 movw（ORIG 0x11/0x12 双 movb，
        // 字段拆分属 Monster 域批次）。
        if (static_cast<signed char>(m_pDungeon->get_dimension_possible()) > 0)
            mob.m_level = static_cast<char>(m_standardDimensionLevel);
        else
            mob.m_level = static_cast<char>(ai->getLevel());
        mob.m_dropCnt = 100;
        mob.m_field11b.m_field11b0 = 0;
        mob.m_field11b.m_field11b1 = 0;

        GetCurrentMapInfo()->Add_Mob(mob);
        ++mobId;                                    // ORIG 0x08302296 尾置自增
      }
    }
apc_end:
    ;
}

// ORIG 0x083022a8：从候选表中随机抽 count 只追加为宝箱怪（每轮一次 rand 取槽）。
// [b28d-solo-BattleMonsterDepsC2] P197④/P225：ORIG `call size; test %eax; sete %al; test %al;
// jne` ⇒ 源写 `size() == 0`（empty() 直跳少 2 条）；`begin()+slot` 不套 static_cast（ORIG
// 直接把 slot 槽址绑 operator+ 第二引用参，cast 会多一条 -0x1c 临时）。
void CBattle_Field::add_monster_additional_champion(std::vector<int>& championList,
                                                    int count)
{
    // ORIG 每轮循环首查 size==0 即退出（首轮亦然，避免空表除零）；empty 检查
    // 必须置于循环体首，不能并入 for 条件（NEW 二进制曾因此首轮漏检）。
    for (int i = 0; i < count; ++i)
    {
        if (championList.size() == 0)
            break;
        const int slot = rand() % championList.size();                     // ORIG 单次 rand
        const int picked = championList[slot];
        championList.erase(championList.begin() + slot);
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
// [b28d-solo-BattleMonsterDepsC2] P167/P13（已修 215→213）：ORIG 调 operator*（deEv）并把结果
// 存具名 -0x10（`(*ev)` 形，非 it->）；begin/end 各一次并缓存；`mob.m_instId = seq++` 复现
// 单次读 seq + `lea 0x1`（写成 seq+1 / mob.m_instId+1 均重读）。
// [SweepBattleMonster 2026-09-09] 已修（188 vs 193，clean 185 vs 187）：①P0 删 `map_monster
// tmp` 直接 `Add_Mob(mob)`（省 0x350 临时，帧 0xa20→0x6d0=ORIG）；②m_field11 字节视图双 movb。
// 残差：`if (m_eventMonsterStatus.empty()) return;` ORIG 为 `!empty()` 正向包裹形
// （`call; xor $1; test; je`，P214①），改包裹需重排整个主流程 ⇒ 记 N + 尾部 2 条。
void CBattle_Field::add_monster_event(int& seq, int x, int y)
{
    // [b28d-Lane27] ORIG 0x083023f7/0x08302408：两条独立 je 各带 nop 岛（P63）。
    if (m_pDungeon == NULL)
        return;
    if (m_pMap == NULL)
        return;
    // [b28d-Lane27] P214①：ORIG 0x08302419 `call empty; xor $0x1,%eax; test %al,%al;
    // je <exit>`（bool callee + `!empty()` 正向包裹、body 直落；原负向早退形 4.4 生
    // `test; jne exit` 且尾部多 2 条）。
    if (!m_eventMonsterStatus.empty())                                    // +0x17c
    {
        int spawned = 0;
        const int monsterCnt =
            static_cast<int>(GetCurrentMapInfo()->m_monsterMap.size());
        unsigned int instBase = 10000;
        // [b28d-solo-BattleMonsterDepsC2] P167/P13：ORIG 调 operator*（`_ZN…deEv`）并把结果存
        // 具名局部 -0x10（`(*ev)` 形），begin/end 各一次并缓存（-0x370/-0x36c + 两转换槽）。
        std::vector<stEventMonsterStatus_t>::const_iterator ev =
            m_eventMonsterStatus.begin();
        std::vector<stEventMonsterStatus_t>::const_iterator evEnd =
            m_eventMonsterStatus.end();
        for (; ev != evEnd; ++ev)
        {
            const stEventMonsterStatus_t& evs = *ev;
            // [b28d-Lane27] ORIG 0x083024e6/0x083024f5 两条独立 `jne <循环尾>`；P296
            // 嵌套正向包裹消同层多 `continue` 的 nop+jmp 中继岛（`jne` 落点=循环尾）。
            if (evs.m_pos.first == x && evs.m_pos.second == y)
            {
            if (m_pMap->m_rivalData2.empty())                             // CMap+0x9c
                return;

            for (int k = 0; k < evs.m_monsterIdx; ++k)
            {
                map_monster mob;               // 每轮默认构造（ORIG ctor 081511c8）
                mob.m_mobIndex = evs.m_eventIdx;
                mob.m_level = static_cast<char>(m_pDungeon->get_standard_level());
                mob.m_mobId = static_cast<int>(instBase);
                // mob+0x11(低)←ev.m_field14、+0x12(高)←ev.m_field10（ORIG 逐字节写）
                mob.m_field11b.m_field11b0 = static_cast<unsigned char>(evs.m_field14);
                mob.m_field11b.m_field11b1 = static_cast<unsigned char>(evs.m_field10);
                mob.m_roleType = 0;
                mob.m_field13 = 0;
                mob.m_field14 = 0;
                mob.m_dropCnt = 100;
                mob.m_flag34 = 0;
                mob.m_bloodRound = 1;                                      // +0x34a

                if (spawned < monsterCnt)
                {
                    mob.m_instId = seq++;          // ORIG 单次读 seq + lea 0x1
                    GetCurrentMapInfo()->Add_Mob(mob);
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
    }
}

// ORIG 0x083026da：直接遍历 GetCurrentMapInfo()->m_monsterMap（+0xc）原图，
// 能力写入落在真实地图怪物上；srand/冠军分支均重新 GetCurrentMapInfo。
// [b28d-solo-BattleMonsterDepsC2] P13/P196（已修 206→209）：ORIG begin/end 各调一次并缓存
// （-0x2c/-0x30）；条件 `movzbl 0x8; cmp $0x4,%al; jg` 为**有符号**比较 ⇒
// (signed char)mm.m_roleType > 4（unsigned 版产 ja 反极性）。
// [SweepBattleMonster 2026-09-09] 已修（213=213，clean 210=210，DIFF→near）：P0 删具名
// `ActiveStaticInfo* active`（ORIG 每处 `lea 0x38(%eax)` 重算，无具名槽）⇒ 槽图整体回正。
// 残差仅 `movsbl` 落 %eax vs ORIG %edx（N7 寄存器角色）。
void CBattle_Field::set_monster_ability()
{
    if (m_pDungeon == NULL)
        return;
    MapInfo* mapInfo = GetCurrentMapInfo();
    mapInfo->m_mapBasisLevel = rand();                                    // +0x48
    srand(static_cast<unsigned int>(GetCurrentMapInfo()->m_mapBasisLevel));

    std::map<int, map_monster>::iterator it =
        GetCurrentMapInfo()->m_monsterMap.begin();
    std::map<int, map_monster>::iterator itEnd =
        GetCurrentMapInfo()->m_monsterMap.end();
    for (; it != itEnd; ++it)
    {
        map_monster& mm = it->second;
        if (static_cast<signed char>(mm.m_roleType) > 4)     // ORIG movzbl + jg
            continue;
        CMonster* monster =
            static_cast<CMonster*>(G_CDataManager()->find_monster(mm.m_mobIndex));
        if (monster == NULL)
            continue;

        // [b28d-D9] P167 差集：ORIG 0x083026da 段 getMonsterScript 调用 3 次
        // （@+0x49/@+0x5c/@+0x89，无具名 script 局部）⇒ 各使用点内联重取（P0/P183）。
        // P0：ORIG 无 active 具名槽（每处 `lea 0x38(%eax)` 重算，仅首调用存 -0x4c 实参临时）
        // ⇒ 全部使用点写 `&mm.m_active`。
        setBasicAbilityByLevel(
            static_cast<STMonsterScript*>(monster->getMonsterScript()),
                               &mm.m_active,
                               static_cast<int>(static_cast<unsigned char>(mm.m_level)),
                               m_dungeonIndex,
                               static_cast<int>(static_cast<signed char>(mm.m_roleType)),
                               false, false);
        upgradeAbilityByMonsterParameterCategory(
            &static_cast<STMonsterScript*>(monster->getMonsterScript())
                 ->m_parameterCategories,
            &mm.m_active, mm.m_roleType == 3);
        upgradeAbilityByDifficultyOfDungeon(get_dungeon_diff(), &mm.m_active,
                                            static_cast<int>(
                                                static_cast<unsigned char>(mm.m_level)),
                                            m_pDungeon->m_vec900);
        if (mm.m_roleType == 3)
        {
            upgradeAbilityByBossFeature(&mm.m_active);
        }
        else if (mm.m_roleType == 1 || mm.m_roleType == 2)
        {
            const int elem = getRandomChampionElement(
                static_cast<STMonsterScript*>(monster->getMonsterScript()),
                GetCurrentMapInfo()->m_mapBasisLevel);
            std::vector<int> features;
            makeChampionFeatureList(mm.m_mobList,
                                    static_cast<unsigned char>(mm.m_level),
                                    GetCurrentMapInfo()->m_mapBasisLevel,
                                    features, get_dungeon_diff());
            upgradeAbilityByChampionFeature(elem,
                                            static_cast<signed char>(mm.m_roleType),
                                            &features, &mm.m_active);
        }
    }
}

// ORIG 0x083029b8：锦标赛副本把当前图怪物按轮次 1..4 分桶进 multimap。
// [b28d-solo-BattleMonsterDepsC2] N82/P225/N113：ORIG 增量段为后置 ppEi（带 -0x20 临时槽），
// 但源写 `it++` 实测 150→157 反劣（4.4 为后置 ++ 另发 sret 拷贝链）；条件 `n<memberCount &&
// it!=end` 的 4.1 bool 物化（mov $1/$0 + test）4.4 恒折直跳。封顶。
void CBattle_Field::insert_tournamentRoundMonster()
{
    // [b28d-Lane27] ORIG 0x083029cc/0x083029d9：两条**独立** `test;je`（各带 nop 岛）
    // ⇒ 拆两条早退；第三层 `call;test %al,%al;je <epilogue>`（2 条、无 xor）⇒ 正向包裹
    // （`if(!f()) return;` 在 4.4 恒产 `xor $0x1; test; jne`，探针 /tmp/l27_probe2.cpp
    //   p1/p2 实证）。
    if (m_pDungeon == NULL)
        return;
    if (m_party == NULL)
        return;
    if (m_pDungeon->isTournamentDungeon())
    {
        std::multimap<int, map_monster>* tournamentMob =
            &GetCurrentMapInfo()->m_tournamentMob;                        // MapInfo+0xd0
        // [b28d-Lane27] ORIG 0x08302a0d 段：`mov 0x8(%ebp),%eax; mov %eax,(%esp); call
        // GetCurrentMapInfo; add $0xc,%eax; mov %eax,-0x14` ⇒ 是**成员指针**（MapInfo+0xc
        // 取址），不是 map 按值拷贝（拷贝会额外 map::C1/D1 + 深拷贝 node 链）。
        std::map<int, map_monster>* monsters = &GetCurrentMapInfo()->m_monsterMap;
        std::map<int, map_monster>::iterator it = monsters->begin();
        const int memberCount = m_party->get_member_count();

        for (int round = 1; round <= 4; ++round)
        {
            int n = 0;
            // ORIG 0x08302b43 段：`n<memberCount && it!=end` 的 4.1 物化岛
            // （`cmp;jge F` + `call ne; test; je F` + `mov $1/jmp/F: mov $0`）⇒ 用 P226
            // 语句表达式复现（`!(A&&B)` 形 4.4 会多一条 xor；直写 && 被折成直跳）。
            // ORIG 0x08302a5c `jmp 08302b43`（前置跳底部条件）+ 体在物理前 ⇒ 4.1 的
            // while 布局（非 do-while：do 形无前置 jmp）。
            while (({ n < memberCount && it != monsters->end(); }))
            {
                tournamentMob->insert(std::make_pair(round, it->second));
                ++n;
                it++;
            }
        }
    }
}

// ORIG 0x08302bc6：按地图物件表与副本指定掉落生成地面物品。
// refData 三份上下文 blob 的字节布局按 ORIG 偏移直写（+0x34 区为生成器读取的
// 调用上下文；其中两处保存指针宽度数据，见各注释）。
// 注意：ORIG 在「assign==-1 且标准等级越界」分支直接跳回循环头而不推进迭代器
// （08302de5 jne 083030ad→083030ae，++it 位于 08303080 正常路径上）；脚本校验
// 保证标准等级恒在 1..200，该路径实际不可达。此处忠实保持同一控制流。
// [b28d-D9] N82/N86：帧 0x20c vs 0x21c + 槽序差（ORIG 0x08302bc6 sz 0x0000060b）
void CBattle_Field::add_item_curr_map(int& itemUid, int& itemIdx)
{
    if (m_pDungeon == NULL || m_pMap == NULL)
        return;

    GetCurrentMapInfo()->m_assignItemIdx = itemIdx;                       // MapInfo+0x3c

    std::list<_mapItem> itemList = m_pMap->get_item_list();
    std::list<_mapItem>::iterator it = itemList.begin();
    // ORIG +0x077 `call GetCurrentMapInfo; mov 0x4(%eax),%eax; mov %eax,-0x24` ⇒
    // 循环前缓存 m_mapIndex 到具名局部（p167 MISSING GetCurrentMapInfo×1）。
    const int curMapIndex = GetCurrentMapInfo()->m_mapIndex;
    (void)curMapIndex;
    _mapItem item;                     // ORIG @0x08302c0e 循环前 C1（-0x44）
    // [b28d-D9] P167 判据②（重载选错）：ORIG 0x08302bc6 调 const 版
    // `_ZNKSt6vectorI21stDungeonAssignItem_tSaIS0_EE2atEj`，我方非 const 版
    // `_ZNSt6vector...2atEj`（MISSING/EXTRA 各 1）⇒ 容器按 const 指针访问。
    const std::vector<stDungeonAssignItem_t>* specialItems =
        m_pDungeon->getSpecialPassiveObjectItems();

    stGenerateRefData_t refRandom;                                        // A @-0xc4
    stGenerateRefData_t refSpecific;                                      // B @-0x10c
    stGenerateRefData_t refDrop;                                          // C @-0x154
    refRandom.m_field35 = static_cast<char>(get_dungeon_diff());
    refRandom.m_rateSlot = 1;
    // [b28d-solo-BattleMonsterDepsC2] §4.3：S290 已在 stGenerateRefData_t +0x34 建具名
    // union 视图（P235）⇒ 改用 m_field34v（vector<pair<int,int>> const*），消除裸偏移。
    refDrop.m_field34v = m_pDungeon->getObjectDropItems();

    while (it != itemList.end() && m_bloodState != 1)                     // +0x1b4
    {
        // [b28d-D9] P167：ORIG 0x08302c46 段用 `item = *it`（List_iterator::de
        // + 4 dword 整块拷贝）而非 it-> 四字段（ptEv×4）；日志段 curMapIndex 亦
        // 无具名槽，重调 GetCurrentMapInfo()->m_mapIndex（P48 无 CSE）。
        item = *it;

        // [b28d-Lane27] P0：ORIG 无 passiveIdx/field04/field08/field0c 具名槽——四者
        // 在使用点直接重读 item 槽（-0x44/-0x40/-0x3c/-0x38），删局部消 4 槽。
        // [b28d-Lane27] ORIG 0x08302cf2 段：先 `cmp size,idx; jbe`（size<=idx）再
        // `test idx; jns`（有符号 >=0）⇒ 按 ORIG 原子序书写（交换后 4.4 产 415 形）。
        if (specialItems->size() <= static_cast<unsigned int>(item.m_objectIdx) ||
            item.m_objectIdx < 0)
        {
            LogManager::logFormat(
                1, "battle_field.cpp",
                __PRETTY_FUNCTION__, 0x7a5,
                "Dungeon passive object index error! Map id:%d, DungeonAssignItem "
                "size:%d, Passive object id:%d",
                GetCurrentMapInfo()->m_mapIndex,
                static_cast<int>(specialItems->size()), item.m_objectIdx);
            return;
        }

        const stDungeonAssignItem_t& assign = specialItems->at(item.m_objectIdx);
        if (assign.m_assignItemIdx != -1)
        {
            refRandom.m_field34 = static_cast<char>(assign.m_assignItemIdx);
            // ORIG byte 写 +0x38 = m_rankBonusIndex 低字节（int 槽内字节写）
            // [b28d-Lane27] 权威头 +0x38 union 已有具名字节视图 m_field38/39 ⇒ 直用，
            // 取址形 `*(uchar*)&m_rankBonusIndex` 4.4 恒产 `lea 0x38; mov %al,(%edx)`（+2/处）。
            refSpecific.m_field38 = static_cast<unsigned char>(assign.m_assignItemIdx);
        }
        else
        {
            // [b28d-D9] P48：ORIG 0x08302d80 段两次独立 get_standard_level()
            // （无具名 stdLvl），且两个赋值目标各自重取源表达式。
            // [b28d-Lane27] ORIG 0x08302dbd 段：`test;jle TRUE(坏)` / `cmp $0xc8; jle FALSE`
            // ⇒ 物化「坏」= lvl<=0 || lvl>200 并早 continue（原 && 正向产 jg/je 反向）。
            if (m_pDungeon->get_standard_level() <= 0 ||
                m_pDungeon->get_standard_level() > 200)
            {
                continue;          // ORIG 直跳循环头（不推进迭代器，见函数头注释）
            }
            // [b28d-Lane27] ORIG 0x08302dfc `test %al; setg %al; test; je`（有符号
            // 「>0」物化）⇒ 写正极性 `> 0` 并交换两臂（then=dimension 分支在前）。
            // [b28d-Lane27] P292 探针 /tmp/l27_probe.cpp g2：ORIG 0x08302dfc
            // `test %al; setg %al` 需**有符号**比较 ⇒ 加 (char) cast（无 cast 生 setne）。
            if (static_cast<char>(m_pDungeon->get_dimension_possible()) > 0)
            {
                refRandom.m_field34 = static_cast<char>(m_standardDimensionLevel);
                refSpecific.m_field38 =
                    static_cast<unsigned char>(m_standardDimensionLevel);
            }
            else
            {
                refRandom.m_field34 =
                    static_cast<char>(m_pDungeon->get_standard_level());
                refSpecific.m_field38 = static_cast<unsigned char>(
                    m_pDungeon->get_standard_level());
            }
        }

        // [b28d-Lane27] ORIG 0x08302e53 段序：result C1 → m_pad37(field08) →
        // +0x39(field04) → +0x34(&assign.m_list)——照 ORIG 语句序（原 result
        // 声明在三写之后、字段写序亦反）。
        stGenerateResult_t result;
        refRandom.m_pad37 = static_cast<char>(item.m_field08);
        // ORIG byte 写 +0x39 = m_rankBonusIndex 第 2 字节（保留 0/2/3 字节）
        // [b28d-Lane27] ORIG 0x08302e67 为**单字节直写**（`mov -0x40,%eax; mov %al,-0xd3`）；
        // 读改写 `(x & 0xffff00ff) | (v<<8)` 生 5 条 → 改字节视图 +1。
        refSpecific.m_field39 = static_cast<unsigned char>(item.m_field04);
        // [b28d-solo-BattleMonsterDepsC2] §4.3：+0x34 = assign.m_list 指针（assign+0x04），
        // 用 S290 具名视图 m_field34l（list<pair<int,int>> const*）替代裸偏移。
        refSpecific.m_field34l = &assign.m_list;                          // assign+0x04

        // [b28d-Lane27] P0：ORIG 无 gen 具名槽（0x08302e99 段接收者留 %eax 直到
        // `call *%ecx`）⇒ 三处链式内联，删局部消 1 槽。
        G_CDataManager()->getItemGenerator()->getGenerator(
            WongWork::CItemGeneratorMgr::eGenerateType_1)
            ->generateRandomItem(refRandom, result);
        G_CDataManager()->getItemGenerator()->getGenerator(
            WongWork::CItemGeneratorMgr::eGenerateType_1)
            ->generateSpecificItem(refSpecific, result);
        G_CDataManager()->getItemGenerator()->getGenerator(
            WongWork::CItemGeneratorMgr::eGenerateType_4)
            ->generateSpecificItem(refDrop, result);

        // [b28d-Lane27] P13：ORIG 0x08302f2a/0x08302f3f 把 begin/end 各求值一次缓存成
        // 两个具名迭代器（-0x48/-0x4c），条件段只做 ne ⇒ 缓存 end（原 for 每轮重调）。
        std::vector<Inven_Item>::iterator dropIt = result.m_dropItems.begin();
        std::vector<Inven_Item>::iterator dropEnd = result.m_dropItems.end();
        for (; dropIt != dropEnd; ++dropIt)
        {
            map_item mi;
            mi.m_count = 1;
            // [b28d-Lane27] ORIG 0x08302f73 段：`mov itemUid,%eax; mov %eax,mi+4;
            // lea 0x1(%eax),%edx; mov %edx,itemUid` ⇒ 后缀自增单次读取（原写成
            // 两句 `mi.m_itemIndex = itemUid; itemUid = mi.m_itemIndex + 1;`
            // 4.4 会重读成员槽 +1 条）。
            mi.m_itemIndex = itemUid++;
            mi.m_standardLevel = static_cast<char>(item.m_field0c);
            mi.m_item = *dropIt;
            GetCurrentMapInfo()->Add_Item(mi);
        }
        ++it;
    }
}

// ---- moveMap_*_Log 族（0x0830766e / 0x08307740 / 0x08307812 / 0x083078e4）----

void CBattle_Field::moveMap_1_Log(int x, int y)
{
    // ORIG 无局部变量：三元表达式直接链式调用（cdecl 从右到左求值，
    // 首个调用用于 m_layeredMap、第二个用于 m_mapIndex）。
    LogManager::logFormat(
        1, "battle_field.cpp", __PRETTY_FUNCTION__, 0x1126,
        "TRY MoveMap [1][Dungeon Type : %d][Map Index : %d](%d,%d) -> (%d,%d) "
        "[Map Kind : %s]",
        m_dungeonIndex, GetMapInfoFromPos(x, y, false)->m_mapIndex, m_curX, m_curY,
        x, y,
        GetMapInfoFromPos(x, y, false)->m_layeredMap ? "layeredMap" : "generalMap");
}

void CBattle_Field::moveMap_2_Log(int x, int y)
{
    LogManager::logFormat(
        1, "battle_field.cpp", __PRETTY_FUNCTION__, 0x1138,
        "TRY MoveMap [2][Dungeon Type : %d][Map Index : %d](%d,%d) -> (%d,%d) "
        "[Map Kind : %s]",
        m_dungeonIndex, GetMapInfoFromPos(x, y, false)->m_mapIndex, m_curX, m_curY,
        x, y,
        GetMapInfoFromPos(x, y, false)->m_layeredMap ? "layeredMap" : "generalMap");
}

void CBattle_Field::moveMap_3_Log(int x, int y)
{
    LogManager::logFormat(
        1, "battle_field.cpp", __PRETTY_FUNCTION__, 0x114a,
        "TRY MoveMap [3][Dungeon Type : %d][Map Index : %d](%d,%d) -> (%d,%d) "
        "[Map Kind : %s]",
        m_dungeonIndex, GetMapInfoFromPos(x, y, false)->m_mapIndex, m_curX, m_curY,
        x, y,
        GetMapInfoFromPos(x, y, false)->m_layeredMap ? "layeredMap" : "generalMap");
}

void CBattle_Field::moveMap_4_Log(int x, int y, const GridScript* gs)
{
    if (gs == NULL)
        return;
    // P44/P52：layeredCount/info 无槽——R2L 实参即时求值，值升 callee-saved。
    LogManager::logFormat(
        1, "battle_field.cpp",
        __PRETTY_FUNCTION__, 0x115b,
        "TRY MoveMap [4] Error(G_CDataManager()->find_dungeon( m_Dungeon )) "
        "Dungeon[%d] Pos(%d,%d), gs->mapIndex(%d) [Map Kind : %s], "
        "gs->layeredMapIndex(%d)\n",
        m_dungeonIndex, m_curX, m_curY,
        static_cast<unsigned int>(gs->m_mapCandidates.size()),
        GetMapInfoFromPos(m_curX, m_curY, false)->m_layeredMap ? "layeredMap" : "generalMap",
        static_cast<unsigned int>(gs->m_layeredMapIndexes.size()));       // gs+0x1c
}

// ORIG 0x083079ba：终点位 boss 房条件未满足时报错码 7。
// [b28d-solo-BattleMonsterDepsC2] P13/P283：ORIG begin/end 各转换构造一次后 `jmp` 底部条件块
// （it=-0x14、itEnd=-0x18 两独立槽）⇒ 源写「两个具名 const_iterator + for(;it!=itEnd;++it)」；
// 条件里直接调 end() 使 4.4 复用 -0x14 槽并每轮重调 end()（-5 条）。
int CBattle_Field::check_boss_map(int x, int y)
{
    if (m_endX == x && m_endY == y &&                                      // +0xc/+0x10
        m_pDungeon != NULL && m_pDungeon->m_bossRoom90c.m_0 == 1)
    {
        std::vector<stConditionEnterBossRoomStatus_t>::const_iterator it =
            m_bossRoomConditions.begin();
        std::vector<stConditionEnterBossRoomStatus_t>::const_iterator itEnd =
            m_bossRoomConditions.end();
        for (; it != itEnd; ++it)
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
// [b28d-solo-BattleMonsterDepsC2] P0（已修）：ORIG 无 MapInfo* 具名局部 ⇒ 两处调用内联
// （消 -0x1c 槽，帧 0x5c→0x4c 与 ORIG 全等）。
// 残差判 N87/N18a：ORIG 绝对值段=单赋值寄存器链 8 条（`sub %eax,%edx; mov %edx,%eax;
// sar $0x1f,%eax; xor %eax,%edx; mov %edx,-0x20; sub %eax,-0x20`）；4.4 两语句形先存槽后
// 读回且把 `-s` 归约为 `+(s&1)`（14 条/处）、单表达式展开 3 次（115）、裸 register 免槽但
// 使 curX/curY 槽浅 4（99，与 ORIG 有槽往返的 P0 证据冲突）。条件块第 4 跳 4.4 恒反极 je。
// 三档类型旋钮（P248）试尽，封顶。
int CBattle_Field::check_movemap_range(int x, int y)
{
    int curX = -1;
    int curY = -1;
    getCurPosXY(curX, curY);
    // [b28f-L27] ORIG 0x08307b5a..0x08307b83 为 abs() 内建内联展开
    //   （`mov %edx,%eax; sar $31,%eax; xor %eax,%edx; mov %edx,slot;
    //     sub %eax,slot`）⇒ 原码写 abs() 而非手写位运算（手写两句会先落槽
    //     再重读，+7 条/处）。
    int dx = abs(x - curX);
    int dy = abs(y - curY);
    // [b28f-L27] ORIG 块序 = 主流程(logFormat)+`mov $0x15`+`jmp epi` 在前、
    //   共享 `mov $0,%eax` 尾块在最后 ⇒ 源码是**正向包裹**（P222/P257）：
    //   条件取反包住主流程，最后的裸 return 0 自然落尾并被两条 `jne` 共享。
    if (dx > 1 || dy > 1 || (dx == 1 && dy == 1))
    {
        LogManager::logFormat(
            1, "battle_field.cpp", __PRETTY_FUNCTION__,
            0x119f,
            "TRY MoveMap [3][Dungeon Type : %d][Map Index : %d](%d,%d) -> (%d,%d) "
            "[Map Kind : %s]",
            m_dungeonIndex, GetMapInfoFromPos(x, y, false)->m_mapIndex, m_curX, m_curY,
            x, y,
            GetMapInfoFromPos(x, y, false)->m_layeredMap ? "layeredMap" : "generalMap");
        return 0x15;
    }
    return 0;
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
    if ((m_pDungeon != NULL) && m_pDungeon->IsEnterEachMap())
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
// [b28d-solo-BattleMonsterDepsC2] P272/P136：ORIG 短路链 `jne<else>`+`je<then>`（else 直落、
// then 在后）⇒ 源写德摩根 `if (curX != x || curY != y) {else} else {then}`；字段直赋 bool 值
// 复现 `movzbl -0x1c(%ebp),%edx; mov %dl,0x1(%eax)`（原三目 `?1:0` 每处 +5 条物化）。
void CBattle_Field::set_visited_map(int x, int y, bool visited)
{
    int curX = -1;
    int curY = -1;
    getCurPosXY(curX, curY);
    if (curX != x || curY != y)
    {
        GetCurrentMapInfo()->m_visited = visited;
    }
    else
    {
        if (getLayeredMapIndex() == 0)
            GetMapInfoFromPos(curX, curY, true)->m_visited = visited;
        else
            getLayeredMapInfoIndex(getLayeredMapIndex() - 1)->m_visited = visited;
    }
}

// ORIG 0x08308a24：写目标格/当前层 visitedGoto 标记。
void CBattle_Field::set_visited_map_goto(int x, int y)
{
    int lastX = -1;
    int lastY = -1;
    getLastPosXY(lastX, lastY);
    if (!(lastX == x && lastY == y))
    {
        GetMapInfoFromPos(x, y, false)->m_visitedGoto = 1;
    }
    else
    {
        if (getLayeredMapIndex() == 0)
            GetMapInfoFromPos(x, y, true)->m_visitedGoto = 1;
        else
            getLayeredMapInfoIndex(getLayeredMapIndex() - 1)->m_visitedGoto = 1;
    }
}

// ORIG 0x08307eca：dummy 图序列推进 / boss 位传送。
// [b28d-solo-BattleMonsterDepsC2] P0：ORIG 对 operator[] 结果**值拷贝**成具名 pair 局部
// （-0x18/-0x14 两槽）再写 m_endX/m_endY；MapInfo* info 无槽 ⇒ 判空内联调用表达式。
// 残差判 N（P292/N18a）：IsHaveDummyMap() 返回 uchar，ORIG 调用点 `call; test %al; je` 无
// setne，4.4 恒加（改 bool 需动 CDungeon.h 共享头，收益 2 条，本波不动）；m_dummyMapPosIdx
// ORIG 两段 `mov 0xcc; mov %eax,%ebx` 而 4.4 单段直落（N18a 反向）；m_visited(char) 判空
// 4.4 多 setne（字段 bool 化会破坏 set_visited_map 已 AE 的写入形）。
void CBattle_Field::choose_random_bossmap_from_dummymaps(const CDungeon* dungeon,
                                                         const GridScript* gs,
                                                         int& mapId, bool& flag,
                                                         int x, int y, int dir)
{
    // [b28d-Lane27] ORIG 0x08307ed1/0x08307edb/0x08307ee5：三条独立 `je` 各跳**不同**
    // nop 岛，且第三岛（0x0830820d）**直落** epilogue（无 jmp）⇒ 用 P223 多岛形
    // 「独立否定 goto + 尾标签串」（标签序=守卫序）复现；if/return 形 4.4 会为第三
    // 个 return 也发 jmp（+2 条）。
    if (dungeon == NULL)
        goto L_dungeonNull;
    if (gs == NULL)
        goto L_gsNull;
    if (m_gridMaze == NULL)
        goto L_gridMazeNull;

    if (dungeon->IsHaveDummyMap())
    {
        // [round5-C05 2026-09-10] D08 配方（多守卫同目标合并为单条短路 goto）：
        // ORIG 0x08307f0f/0x08307f1e 两条 `jne <epilogue>`，我方平铺 return 令
        // 4.4 在尾部各发一对 `jmp epilogue;nop` 中继岛（尾部 5 岛 vs ORIG 3 岛）
        // ⇒ 合并为单条 `||` goto 到尾标签串（与上方三守卫同目标）。
        if (m_endX != x || m_endY != y)                                   // +0xc/+0x10
            goto L_dungeonNull;
        if (static_cast<unsigned int>(m_dummyMapPosIdx) <
            m_dummyMapPos.size())                                         // +0xcc < +0xc0.size
        {
            std::pair<int, int> next = m_dummyMapPos[m_dummyMapPosIdx];
            m_endX = next.first;
            m_endY = next.second;

            if (GetMapInfoFromPos(m_endX, m_endY, false)->m_visited)
            {
                GetMapInfoFromPos(m_endX, m_endY, false)->m_visited = 0;
                GetMapInfoFromPos(m_endX, m_endY, false)->m_mapType = 2;
            }

            int curX = -1;
            int curY = -1;
            getCurPosXY(curX, curY);
            if (m_endX == curX && m_endY == curY)
            {
                flag = false;
                GetMapInfoFromPos(m_endX, m_endY, false)->m_mapType = 2;
            }

            mapId = dungeon->ChooseRandomMap(2, gs->m_gridValue, dir);
            GetMapInfoFromPos(x, y, false)->m_dummyBossMap = 1;
            m_dummyMapPosIdx = m_dummyMapPosIdx + 1;
        }
        else
        {
            if (GetMapInfoFromPos(x, y, false)->m_dummyBossMap)
                GetMapInfoFromPos(x, y, false)->m_dummyBossMap = 0;
        }
    }
    else if (dungeon->m_entryRestrictFlag != 0)
    {
        // [b28d-Lane27] ORIG 0x083081f7 `cmp -0x10(%ebp),%eax; setl %al`（**有符号**）
        // ⇒ count 与 i 都是 int（size_t 会生 setb）。
        const int count = static_cast<int>(m_gridMaze->m_vec20.size());   // maze+0x20
        for (int i = 0; i < count; ++i)
        {
            // ORIG 两字段各重取一次 operator[]（P157 逐字段重读，p167 缺 1 处）
            if (m_gridMaze->m_vec20[i].first == x &&
                m_gridMaze->m_vec20[i].second == y)
            {
                m_endX = x;                                               // +0xc
                m_endY = y;                                               // +0x10
                // [b28d-Lane27] P171：ORIG 0x083081eb `mov %eax,0x10(%ebp)` 把
                // getGridR 返回的引用地址**死写入 gs 形参槽**（gs 此后不再使用）。
                // 旧版写成 `mapId = <地址>` 是**真语义 bug**（越权改输出参数）。
                gs = &m_gridMaze->getGridR(x, y);
                return;
            }
        }
    }

L_dungeonNull:;
L_gsNull:;
L_gridMazeNull:;
}

// ORIG 0x08308214：候选图收集（终点/起点任务/hell/普通四路分支）。
// [b28d-D9] P225/P197④：ORIG test+sete 物化岛；4.4 直跳（ORIG 0x08308214 sz 0x000004bd）
int CBattle_Field::get_candidator_map(const CDungeon* dungeon, const GridScript* gs,
                                      int& mapId, std::vector<int>& candidator,
                                      int x, int y, int dir)
{
    if (dungeon == NULL || gs == NULL)
        return 0x17;
    // [b28d-Lane27] ORIG 0x08308238 段 `cmp $0xffffffff,%eax; jne 0x083086c4`（直跳尾部
    // `mov $0,%eax`）⇒ 写「`if (mapId == -1) { 主体 }` + 尾部共享 `return 0`」；负向早退形
    // 4.4 会把 `return 0` 就地内联（`je 主体; mov $0; jmp`，+3 条）。主体缩进从简。
    if (mapId == -1)
    {
    if (m_endX == x && m_endY == y)                                       // +0xc/+0x10
    {
        if (gs->m_secondCandidates.size() == 0)                           // gs+0x10
        {
            mapId = dungeon->ChooseRandomMap(1, gs->m_gridValue, dir);
        }
        else
        {
            // [b28d-Lane27] ORIG 槽位：map=-0x2c / i=-0x28（map **先声明**更深）⇒ 把
            // map 提到 for 之前声明（P212/P331 同尺寸标量声明序）。
            // [L27-r2·判据⑩②/widthscan] ORIG 该槽无零初始化（我方 `movl $0x0,-0x2c` 为多出）
            //   ⇒ 先声明、循环内赋值（P198 孪生）。
            CMap* map;
            for (unsigned int i = 0; i < gs->m_secondCandidates.size(); ++i)
            {
                map = static_cast<CMap*>(
                    G_CDataManager()->find_map(gs->m_secondCandidates[i]));
                if (map == NULL)
                    return 0x17;
                if (map->CheckEntrance(dir))
                    candidator.push_back(gs->m_secondCandidates[i]);
            }
        }
    }
    else if (m_field14 == x && m_field18 == y &&                          // +0x14/+0x18
             m_questStageMap != 0)                                        // +0x19c
    {
        mapId = m_questStageMap;
        if (mapId < 1)
        {
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
                GetMapInfoFromPos(m_curX, m_curY, false)->m_layeredMap
                    ? "layeredMap" : "generalMap",
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
        if (gs->m_mapCandidates.size() == 0)                              // gs+0x04
        {
            mapId = dungeon->ChooseRandomMap(0, gs->m_gridValue, dir);
        }
        else
        {
            // [b28d-Lane27] ORIG 槽位：map=-0x24 / i=-0x20（map 先声明更深）。
            // [L27-r2·判据⑩②/widthscan] 同上：去 `= NULL` 消 `movl $0x0,-0x24`。
            CMap* map;
            for (unsigned int i = 0; i < gs->m_mapCandidates.size(); ++i)
            {
                map = static_cast<CMap*>(
                    G_CDataManager()->find_map(gs->m_mapCandidates[i]));
                if (map == NULL)
                    return 0x17;
                if (map->CheckEntrance(dir))
                    candidator.push_back(gs->m_mapCandidates[i]);
            }
            if (!GetCurrentMapInfo()->m_layeredMap &&
                candidator.size() == 0)
            {
                for (size_t i = 0; i < gs->m_mapCandidates.size(); ++i)
                {
                    // [round5-C05 2026-09-10] P171 死写入照抄：ORIG 0x08308631
                    // `mov %eax,-0x24(%ebp)` 把 find_map 结果写回具名 `map` 槽
                    // （其后无读点）；我方 `(void)` 丢弃 ⇒ 少 1 条（327 vs 325）。
                    map = static_cast<CMap*>(
                        G_CDataManager()->find_map(gs->m_mapCandidates[i]));
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
    }
    return 0;
}

// ORIG 0x08308882：确定最终图号并写回当前图信息。
// [b28d-solo-BattleMonsterDepsC2] P197④/P0：ORIG `call size; test %eax; setne %al; test %al;
// je` ⇒ `candidator.size() != 0`（empty() 生 xor $1 两段）；get_rand_int 结果具名存 -0x1c
// （ORIG 有槽往返）；logFormat 内联 GetMapInfoFromPos（具名 info 使调用提前，破坏右起求值序）。
int CBattle_Field::set_map_id(const GridScript* gs, int& mapId,
                              std::vector<int>& candidator)
{
    if (gs == NULL)
        return 0x15;

    if (mapId < 0)
    {
        if (candidator.size() != 0)
        {
            const int idx =
                get_rand_int(static_cast<int>(candidator.size()));
            mapId = candidator[idx];
        }
        if (GetCurrentMapInfo()->m_layeredMap)
            mapId = gs->m_layeredMapIndexes[getLayeredMapIndex()];
    }

    GetCurrentMapInfo()->m_mapIndex = mapId;
    if (mapId < 1)
    {
        LogManager::logFormat(
            1, "battle_field.cpp",
            __PRETTY_FUNCTION__,
            0x1363,
            "[CBattle_Field::MoveMap] Error Dungeon[%d] Pos(%d,%d), candidator(%d), "
            "gs->mapIndex(%d) [Map Kind : %s], gs->layeredMapIndex(%d)\n",
            m_dungeonIndex, m_curX, m_curY,
            static_cast<unsigned int>(candidator.size()),
            static_cast<unsigned int>(gs->m_mapCandidates.size()),
            GetMapInfoFromPos(m_curX, m_curY, false)->m_layeredMap
                ? "layeredMap" : "generalMap",
            static_cast<unsigned int>(gs->m_layeredMapIndexes.size()));
        return 0x15;
    }
    return 0;
}
