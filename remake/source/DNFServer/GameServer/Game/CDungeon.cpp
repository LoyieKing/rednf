// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CDungeon/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "GameRand.h"
// df_game_r 还原 —— CDungeon / CDungeon_TowerOfDespair / STDungeonScript
//（G2 副本链）。
// 逐函数对照 docs/class_func_reports/CDungeon.md、CDungeon_TowerOfDespair.md
// 与 ORIG df_game_r 反汇编实现；目标 identical 口径。
// 支撑类型（MazeScript/stDungeonAssignItem_t/STWarRoomData/...）的 ORIG W 符号
// 也在本 TU 内实现。
// [b28d-solo-CDungeon2 收口] 清单 5 件：_do_after_dungeon_start NEAR→AE；
// GetRandMazebyPartyCount DIFF→NEAR（帧 0x40 / divisor 槽 -0x2c 与 ORIG 全等）；
// set_dungeon 真实残差 537→20 行（帧 0x2bc / nameBuf 槽 -0x29c 与 ORIG 全等，
// 余 3×movzbl 异型中转 N18a + 1 岛 N112 + 中间区 4 槽 N82）；
// ClearRewardData::operator= / IncreaseEnterCount 复验仍 N18a 封顶。
// TU 口径：identical+ae 1767→1768、near 1（换件）、diff 6→5；p167 五件全 CLEAR。
#include "CDungeon.h"

#include <string.h>
#include <stdlib.h>

#include <algorithm>

#include "CMap.h"
#include "GameTypes.h"
#include "LogManager.h"
#include "DNFFunctionLib.h"
#include "CInventory.h"             // check_item_exist/GetInvenSlot/delete_item
// [Lane37CUser 2026-09-10 解环] CInventory.h 的 `#include "CUser.h"` 已改前向声明
//   （解 CUser.h ⇄ CInventory.h 环）⇒ 依赖传递获得 CUser 完整定义的 TU 需自行直补。
#include "CUser.h"
#include "PacketGuard.h"            // 门票回执包
#include "GlobalData.h"             // GlobalData::s_systemTime_/s_statistic_proxy_mgr
#include "CServerProxyMgr.h"        // GetServerProxy 完整模板
#include "CStatisticServerProxy.h"  // 统计上报
#include "DataManagerInitDepends.h" // TOD_UserState/TOD_Layer（绝望之塔扩展数据）
#include "CBoosterGage.h"           // charac_expand::CData::alter（脏标记）

// ============================================================================
// 支撑类型：GridScript / RidableObject / RidableScript / DungeonClearCondition
// ============================================================================

// ============================================================================
// CDungeon 弱访问器 / 自包含逻辑（ORIG 地址见各函数注释；
// ctor/dtor/set_dungeon 及 GameStubs.cpp 已有桩的方法不在此重复定义）
// ============================================================================

#include "CDungeon/getDungeonKind.inc"
{
    return m_dungeonKind;
}

#include "CDungeon/isNoFatigueDungeon.inc"
{
    return m_noFatigueFlag;
}

#include "CDungeon/get_blood_dungeon_type.inc"
{
    return m_bloodDungeonType;
}

#include "CDungeon/getQuestNpcDungeon.inc"
{
    return m_questNpcDungeon;
}

#include "CDungeon/get_limit_party_count.inc"
{
    return m_limitPartyCount;
}

#include "CDungeon/get_limit_Inout_count.inc"
{
    return m_limitInoutCount;
}

#include "CDungeon/limitOfStackableItemInTower.inc"
{
    return m_limitStackableItemInTower;
}

#include "CDungeon/getHeroesRequiredQuest.inc"
{
    return m_heroesRequiredQuest;
}

#include "CDungeon/getAICharacterAppearRate.inc"
{
    return m_aiCharacterAppearRate;
}

#include "CDungeon/getRandomTowerMaxStage.inc"
{
    return m_randomTowerMaxStage;
}

#include "CDungeon/getDeathTowerMaxStage.inc"
{
    return m_deathTowerMaxStage;
}

#include "CDungeon/getDeathTowerMapIndex.inc"
{
    return m_deathTowerMapIndexes[idx];
}

#include "CDungeon/getTowerMaxClearRewardItemNum.inc"
{
    return m_towerMaxClearRewardItemNum;
}

#include "CDungeon/getEventMonsterTotalProb.inc"
{
    return m_eventMonsterTotalProb;
}

#include "CDungeon/getTryGenEventMonsterCnt.inc"
{
    return m_tryGenEventMonsterCnt;
}

#include "CDungeon/getTryGenEventMonsterProb.inc"
{
    return m_tryGenEventMonsterProb;
}

#include "CDungeon/getSecondEventMonsterTotalProb.inc"
{
    return m_secondEventMonsterTotalProb;
}

#include "CDungeon/getTryGenSecondEventMonsterCnt.inc"
{
    return m_tryGenSecondEventMonsterCnt;
}

#include "CDungeon/getTryGenSecondEventMonsterProb.inc"
{
    return m_tryGenSecondEventMonsterProb;
}

// [b28d-solo-CBossStage R2] 随 CDungeon.h 声明同步（uchar→bool，调用点 P292 判据）。
#include "CDungeon/hasTowerItemDrop.inc"
{
    return m_hasTowerItemDrop;
}

#include "CDungeon/GetDummyAppearCount.inc"
{
    return m_dummyAppearCount;
}

#include "CDungeon/IsHaveDummyMap.inc"
{
    return m_hasDummyMap;
}

#include "CDungeon/isRiskDungeon.inc"
{
    return m_riskFlag;
}

#include "CDungeon/IsOpenEvenEnemy.inc"
{
    return m_openEvenEnemy;
}

#include "CDungeon/GetPartyMemberCoinLimit.inc"
{
    return m_partyMemberCoinLimit;
}

#include "CDungeon/get_exp_weight.inc"
{
    return m_expWeight;
}
#include "CDungeon/getDropItems.inc"
{
    return const_cast<std::vector<std::pair<int, int> >*>(&m_vec40);
}
#include "CDungeon/getObjectDropItems.inc"
{
    return const_cast<std::vector<std::pair<int, int> >*>(&m_vec4c);
}

#include "CDungeon/getSpecialPassiveObjectItems.inc"
    const
{
    return const_cast<std::vector<stDungeonAssignItem_t>*>(&m_vec58);
}

#include "CDungeon/getEventMonsters.inc"
{
    return const_cast<std::vector<stEventMonster_t>*>(&m_eventMonsters);
}

#include "CDungeon/getSecondEventMonsters.inc"
{
    return const_cast<std::vector<stSecondEventMonster_t>*>(&m_secondEventMonsters);
}

#include "CDungeon/getRandomTowerMapIndexes.inc"
    const
{
    return const_cast<std::map<int, std::vector<RandomList> >*>(&m_map658);
}

#include "CDungeon/get_dimension_min_partymem.inc"
{
    return m_dimensionMinPartymem;
}

#include "CDungeon/GetQuestMazeScriptVector.inc"
    ENUM_MAZE_QUEST_TYPE mazeQuestType) const
{
    return const_cast<std::vector<MazeScript>*>(&m_mazeList0[mazeQuestType]);
}

// ---- 迁移自 GameStubs.cpp 的 CDungeon 桩方法（ORIG 弱符号，签名已核对）----
#include "CDungeon/CDungeon.inc"
    : m_typeInheritance(0),
      m_riskFlag(false),
      m_partyMemberCoinLimit(0),
      m_enterEachMap(false),
      m_openEvenEnemy(false)
{
    // ORIG 0x08373f3c：初始化列表按成员声明位置赋值（vtable 后立即 +0x04，
    // 0x6bc vector 构造后 0x6c9/0x6cc/0x6d0/0x6d1），其它 POD 不初始化。
}
#include "CDungeon/dtor_CDungeon.inc"
{
    // ORIG 0x08376fd8：析构 std 容器成员（自动）。
}

#include "CDungeon/get_index.inc"
{
    // ORIG 0x080fdcf0：return m_index（+0x08）
    return m_index;
}

#include "CDungeon/get_min_level.inc"
{
    // ORIG 0x0814559a：return m_minLevel（+0x0c）
    return m_minLevel;
}

#include "CDungeon/get_standard_level.inc"
{
    // ORIG 0x080f9810：return m_standardLevel（+0x10）
    return m_standardLevel;
}

#include "CDungeon/GetDungeonName.inc"
{
    // ORIG 0x081455a6：return m_name.c_str()（m_name +0x18）
    return m_name.c_str();
}
#include "CDungeon/get_dimension_possible.inc"
{
    // ORIG 0x0826b938：return m_dimensionPossible（单 movzbl）
    return m_dimensionPossible;
}

#include "CDungeon/isTowerOfDespairDungeon.inc"
{
    // ORIG 0x0822b4fa：return GetType_DungeonInHeritance() == 1。
    // [b28c-21] ORIG `sub $0x18` 保守帧；4.4 对「TU 内已见定义的 callee」精确
    // 继承 $0x4（P210 三态②）⇒ 把 GetType_DungeonInHeritance 定义移到本函数
    // 之后（本 TU 仅此处调用它），4.4 只见声明 ⇒ 恢复 $0x18（P210 三态③）。
    return GetType_DungeonInHeritance() == 1;
}

#include "CDungeon/GetType_DungeonInHeritance.inc"
{
    return m_typeInheritance;
}

#include "CDungeon/isTournamentDungeon.inc"
{
    // ORIG 0x0836520a：m_tournamentFlag 非 0 且 (signed char)<=1
    //（1=普通锦标赛，2=特殊，负值视为普通）
    if (m_tournamentFlag != 0 && (char)m_tournamentFlag <= 1)
        return true;
    return false;
}

#include "CDungeon/getTournamentRoundFatigue.inc"
{
    // ORIG T 0x08365238：返回 m_tournamentRoundFatigue（+0x8a4）；0 兜底返回 1。
    if (m_tournamentRoundFatigue == 0)
        return 1;
    return m_tournamentRoundFatigue;
}

#include "CDungeon/IsEnterEachMap.inc"
{
    // ORIG 0x08365278：return m_enterEachMap（movzbl 0x6d0）
    return m_enterEachMap;
}

// ORIG _ZN12CDataManager18set_dimensionInoutEic / _ZN12CDataManager21set_limit_inout_countEc15ENUM_BLOOD_TYPE
// 真实 C++ 成员实现（权威声明在 CDataManager.h），语义按 ORIG 反汇编：
//   set_dimensionInout（080eed82）：idx>5 或 idx<0 早退，再写数组
//   set_limit_inout_count（08374c82）：写 m_limitInoutCount[type]
#include "CDungeon/set_dimensionInout.inc"
{
    if (idx > 5)
        return;
    if (idx < 0)
        return;
    m_dimensionInout[idx] = value;
}

#include "CDungeon/set_limit_inout_count.inc"
{
    m_limitInoutCount[type] = value;
}

#include "CDungeon/set_dungeon.inc"
{
    // [b28d-solo-CDungeon2 判词 · N18a/N112/N82 封顶（真实残差 537→20 行）]
    // ORIG 0x0834acc4 / our 805（ORIG 804，gap +1）；p167 82=82 CLEAR。
    // 历史修法：cit 非 const + while(cit!=end)
    // 正极性 + cit++ 后置 + pair 临时构造 + insert 直构 value_type（P135/aSEOS0_）。
    // 本轮修法：①maze 分流 if-else 链改 switch（P251/P46 无槽多路比较；叶块序
    //   -1/0/1 对齐 ORIG 0x834afa7/afc1/afdb），删 questType 具名局部（P0：ORIG
    //   直读 maze+0x4c 无槽）；②m_map340 循环 for-init 逗号声明 rit/ritEnd 缓存
    //   end()（P13；ORIG begin+end 各求值一次于循环前，4.4 原每次迭代重调 end()）；
    // ③字段类型旋钮（P248 档②/③，均不入 mangled/偏移）：STDungeonScript::m_48c/
    //   m_4e0/m_504/m_505 uchar→bool（消 4 处 test/setne）、m_140 uchar→bool
    //   （配 S291 的 CDungeon::m_89d bool 化，源/目标同型消 test/setne）、
    //   CDungeon::m_advanceAltarSurvivalFlag char→uchar（消 movzbl→eax→edx 中转）、
    //   m_towerMaxClearRewardItemNum int→unsigned（复现 ORIG 异型 RHS-first 中转）、
    //   m_entryRestrictFlag char→uchar（消异型 store 中转）。
    // 本轮修法：加 `char pad0[4]; (void)&pad0;`（P232 dead local 占帧）⇒ 帧
    //   0x2ac→0x2bc、nameBuf 槽 -0x298→-0x29c，二者与 ORIG 全等，且深区
    //   （≥-0x8c：totalRate/nameBuf/...）槽位全部对齐（真实残差 43 组→12 组）。
    // 残余（N 封顶）：②ORIG log 分支后 `jmp;nop` 循环顶对齐岛（N112，
    //   -falign-loops 无效；P223 形 `if(p.first==0) goto L; body; L:;` 探针可造，
    //   但会改写整个循环块布局、收益仅 2 条，不做）。
    //   ③中间区 4 槽差 4：trace -0x84 vs -0x88、rit -0x74 vs -0x78、p -0x70/-0x6c vs
    //   -0x74/-0x70（块内/临时区基址差，N82/N93 族；pad1 实验只推动深区不推动本区）。
    // [b28d-SweepCDungeon 已修 · ①] m_minAttackPartyMember/m_maxAttackPartyMember/
    //   m_maxRevengeCount 由 char 改 unsigned char（CDungeon.h；T1 直证：ORIG
    //   0x085ba84b/85d `movzbl; movsbl %al` = uchar 字段 + (signed char) cast、
    //   0x085ba8bb `movzbl; cmpb/jae` = uchar + 无符号比较；探针 /tmp/probe_char.cpp
    //   f1(char 目标)=movzbl→%eax; mov %eax,%edx 两段 vs f2(uchar 目标)=movzbl→%edx
    //   单段）⇒ 本函数 3 处 `movzbl→eax; mov %eax,%edx` 中转消除，ndiff 18→12、
    //   our 805→802、gap −2→+1；CParty.cpp:1550-1551 同步补 (signed char) cast
    //   保持 ORIG 有符号比较（CParty/DisPatcherCore/PartyClosureC2 三消费 TU
    //   改前/改后四数逐值相同，零回退）。
    // .h 改动已 R2 广播（S273 同步）；CDungeon/STDungeonScript/CParty/DisPatcherCore/
    // CGameManager/BattleMonsterDepsC2/CPartyDepsB2/CPartyOnlyOrig/PartyClosureC2 全复验零回归。
    // ORIG 0x0834acc4：从脚本复制全部字段（顺序按 ORIG 汇编）
    m_index = script.m_18;
    m_standardLevel = script.m_ac;
    m_minLevel = script.m_78;
    m_expWeight = script.m_b0;

    // [b28d-solo-CDungeon2] P232 dead local：4.4 对未使用数组照占帧；此处把 nameBuf
    // 槽从 -0x298 顶到 ORIG 的 -0x29c（帧同步 0x2ac→0x2bc），深区槽位随之全对齐。
    char pad0[4];
    (void)&pad0;
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

    m_heroesRequiredQuest = script.m_14;
    m_dimensionPossible = script.m_304;
    m_dimensionInoutValue = script.m_305;
    m_dimensionMinPartymem = script.m_306;
    if (m_dimensionPossible != 0)
    {
        G_CDataManager()->set_dimensionInout(
            (int)(char)m_dimensionPossible - 1, (char)m_dimensionInoutValue);
        G_CDataManager()->set_original_dimensionInout(
            (int)(char)m_dimensionPossible - 1, (char)m_dimensionInoutValue);
    }
    m_limitPartyCount = script.m_324;
    m_limitInoutCount = script.m_325;
    if (m_index == 0x2aff)
    {
        G_CDataManager()->set_limit_inout_count(
            (char)m_limitInoutCount, (ENUM_BLOOD_TYPE)2);
    }
    if (m_index == 0x2afe)
    {
        G_CDataManager()->set_limit_inout_count(
            (char)m_limitInoutCount, (ENUM_BLOOD_TYPE)1);
    }

    std::vector<MazeScript>::iterator mit = script.m_mazeList170.begin();
    std::vector<MazeScript>::iterator mitEnd = script.m_mazeList170.end();
    while (mit != mitEnd)
    {
        MazeScript* maze = &(*mit);
        switch (maze->m_questType)
        {
        case -1:
            m_mazeList.push_back(*maze);
            break;
        case 0:
            m_mazeList0[0].push_back(*maze);
            break;
        case 1:
            m_mazeList0[1].push_back(*maze);
            break;
        }
        ++mit;
    }

    int totalRate = 0;
    m_clearItemMap.clear();
    std::vector<std::pair<int, int> >::iterator cit =
        script.m_vecc8.begin();
    std::pair<int, int> p;
    while (cit != script.m_vecc8.end())
    {
        p = std::pair<int, int>(*cit);
        if (p.first != 0)
        {
            totalRate = p.second + totalRate;
            m_clearItemMap.insert(std::pair<const int, int>(totalRate, p.first));
            if (totalRate > 100)
            {
                LogManager::logFormat(
                    1, "data_manager.cpp",
                    "void CDungeon::set_dungeon(STDungeonScript&)", 0x5e1,
                    "total_rate(%d) > 100", totalRate);
                goto copy_rest;
            }
            cit++;
        }
    }

copy_rest:
    m_vec40 = script.m_vecd4;          // 通关掉落
    m_vec4c = script.m_vece0;          // 对象掉落
    m_vec58 = script.m_vecec;          // 指定物品对象
    script.m_warRoom18c.copy(&m_warRoomData);
    m_entryRequiredItem = script.m_requiredItem7c;
    m_entryRestrictFlag = script.m_88;
    m_entryRequiredItem2 = script.m_requiredItem8c;
    m_vec86c = script.m_vec98;
    m_minAttackPartyMember = script.m_314;
    m_maxAttackPartyMember = script.m_315;
    m_maxRevengeCount = script.m_317;
    if (script.m_316)
        m_villageAttackEventFlag = 1;
    else
        m_villageAttackEventFlag = 0;
    m_87c = script.m_a4;
    m_bossRoom90c.m_0 = script.m_bossRoom328.m_0;
    m_bossRoom90c.m_vec4 = script.m_bossRoom328.m_vec4;
    std::copy(script.m_128, script.m_128 + 5, m_880);
    m_894 = script.m_13c;
    m_898 = script.m_168;
    m_89c = script.m_28c;
    m_89d = script.m_140;
    m_89e = script.m_28d;
    m_bloodDungeonType = script.m_28e;
    m_advanceAltarSurvivalFlag = script.m_2ab;
    m_advanceAltarType = script.m_2ac;
    memcpy(m_808, script.m_2b0, sizeof(m_808));
    memcpy(m_820, script.m_2bc, sizeof(m_820));
    m_vec814 = script.m_vec2c8;
    m_clearReward = script.m_clearReward2d4;
    m_survivalReward = script.m_survival2f8;
    m_hasDummyMap = 0;
    m_dummyAppearCount = script.m_16c;
    m_aiCharacterAppearRate = script.m_338;
    m_dungeonKind = script.m_358;
    m_deathTowerMaxStage = script.m_368;
    if (m_deathTowerMaxStage != 0)
    {
        m_deathTowerMapIndexes.reserve(m_deathTowerMaxStage + 1);
        for (unsigned int i = 0; i < script.m_vec35c.size(); ++i)
            m_deathTowerMapIndexes[script.m_vec35c.at(i).first] =
                script.m_vec35c.at(i).second;
    }
    m_randomTowerMaxStage = script.m_33c;
    if (m_randomTowerMaxStage != 0)
    {
        m_map658.clear();
        for (std::map<int, std::vector<RandomList> >::const_iterator
                 rit = script.m_map340.begin(),
                 ritEnd = script.m_map340.end();
             rit != ritEnd; ++rit)
        {
            m_map658.insert(std::make_pair(rit->first, rit->second));
        }
    }
    m_8a0 = script.m_3c4;
    m_vec8f0 = script.m_vec3c8;
    m_tryGenEventMonsterCnt = script.m_3d4;
    m_tryGenEventMonsterProb = script.m_3d8;
    m_eventMonsterTotalProb = script.m_3dc;
    m_eventMonsters = script.m_vec3e0;
    m_tryGenSecondEventMonsterCnt = script.m_3ec;
    m_tryGenSecondEventMonsterProb = script.m_3f0;
    m_secondEventMonsterTotalProb = script.m_3f4;
    m_secondEventMonsters = script.m_vec3f8;
    m_limitStackableItemInTower = (script.m_380 == 1);
    m_hasTowerItemDrop = (script.m_3b4 == 1);
    m_towerMaxClearRewardItemNum = script.m_3b8;
    m_vec900 = script.m_vec404;
    m_noFatigueFlag = script.m_48c;
    if (script.m_2a9 != 0)
    {
        m_typeInheritance = 1;
    }
    m_questNpcDungeon = script.m_2aa;
    m_tournamentFlag = script.m_494;
    m_tournamentRoundFatigue = script.m_498;
    m_itemRate8c4 = script.m_itemRate4b4;
    m_map8ac = script.m_map49c;
    m_8d4 = script.m_4c4;
    m_map8d8 = script.m_map4c8;
    m_map91c = script.m_map4e4;
    m_dimensionPartyCount.swap(script.m_dimension308);
    m_riskFlag = script.m_4e0;
    m_partyMemberCoinLimit = script.m_4fc;
    m_enterEachMap = script.m_504;
    m_openEvenEnemy = script.m_505;
}

// ---- 绝望之塔（TOD）----
// ORIG：GetCharacExpandData(0xd) 返回 TOD_UserState（charac_expand 扩展数据，
// 权威布局见 DataManagerInitDepends.h；IncreaseEnterCount/getEnterCount/
// getTodayEnterLayer/TOD_Layer::GetLayer 均为 ORIG 真实符号）。

// ORIG T 0x0864388a（报告 TOD_UserState/IncreaseEnterCount.md）：enterCount(+0x10)
// 与 lastEnterLayer(+0x16) 各自 ++、lastEnterTime(+0x18)=getCurSec；当日层槽
// (+0x14) 按 ORIG 是内嵌 TOD_Layer——判 0 走真实 call TOD_Layer::GetLayer
// (W 0x0822ec40, this+0x14)，非字段直读。（Sec 批：TOD_UserState +0x8 段已内嵌
// SIG_REQUEST_TOD_USER_STATE，m_todayLayer 本体即 TOD_Layer，直调免 cast。）
// [b22判词·跨版本] 残余 1 insn：ORIG 4.1 对 ushort 回写生成 `add $0x1,%eax;
// mov %eax,%edx`，4.4 恒融合为 `lea 0x1(%eax),%edx`（三变体实测均 lea，见
// 范式库跨版本豁免表 movzwl 段）；`movl $0x0,(%esp)` 系 .o R_386_32 数据地址
// reloc 占位（§6.3），relink 后即 &GlobalData::s_systemTime_。语义等价。
// [b28c-lane17] fresh 复验确认：字段已 unsigned short（movzwl 宽度/偏移全对），
// 唯一残差即该 lea-vs-add+mov 折叠 ⇒ N18a（单段 regmove 不可造）封顶。
#include "CDungeon/IncreaseEnterCount.inc"
{
    // [b28d-solo-CDungeon 判词 · N4/P30 封顶] ORIG 0x0864388a / our 34（ORIG 35，
    // gap 1）；p167 4=4 CLEAR。残余唯一：ORIG `movzwl 0x16; add $0x1,%eax;
    // mov %eax,%edx` vs 4.4 `lea 0x1(%eax),%edx`（add-vs-lea 融合）。本轮再试
    // `++` / `+= 1` / `(unsigned short)(x+1)` 三形全产 lea，与 q1-q6 六形探针
    //（f+1 / +=1 / 1+f / (ushort)cast / register int / register ushort）一致 ⇒ 封顶。
    // [b28d-solo-CDungeon2 复验] fresh 编译仍唯一残差；T1 直证 movzwl/mov %dx ⇒ 宽度
    // 确定（unsigned short），残差仅 add-vs-lea ⇒ 按 P321-T5 禁改类型，N18a 封顶。
    m_data.m_enterCount = m_data.m_enterCount + 1;
    m_data.m_lastEnterLayer = m_data.m_lastEnterLayer + 1;
    m_data.m_lastEnterTime = GlobalData::s_systemTime_.getCurSec();
    if (m_data.m_todayLayer.GetLayer() == 0)
        m_data.m_firstEnterTime = GlobalData::s_systemTime_.getCurSec();
    ((charac_expand::CData*)this)->alter();
}

// 注：保持 POD（无 user ctor）——GCC 4.4 __attribute__((packed)) 对含非 POD
// 成员的外层 struct 不生效，会致 Key 偏移 4 对齐（0xc）而非 ORIG 0xa。
// [b28d-solo-CDungeon2] 成员名保持与 TowerOfDespairStatistic_Key.h 逐字一致（ODR），
// 但语义更正：m_0@+0 = packet+0xa（_do_after_dungeon_start/ClearDungeon/Statics
// 全无写读点，占位）、m_4@+4 = packet+0xe（当日层/层数写入口；ORIG 0x0834d8ec
// `mov %eax,-0x31(%ebp)`，gts 基址 -0x3f ⇒ +0xe）。权威头
// shared/packet/include/Packet_TowerOfDespair_Statistic_GTS.h 同布局（m_padA/m_layer）。
struct TowerOfDespairStatistic_Key
{
    unsigned int m_0;   // +0x00（packet+0xa，无写/读点，占位）
    unsigned int m_4;   // +0x04（packet+0xe，当日层/层数写入口）

    TowerOfDespairStatistic_Key();
};

#include "CDungeon/TowerOfDespairStatistic_Key.inc"
{
    m_0 = 0;
    m_4 = 0;
}

// Packet_TowerOfDespair_Statistic_GTS（ORIG ctor 0x08374cba W，总尺寸 0x17）：
// PacketHeader(0x271c,0x17)[10] + Key[8] + byte flag + uint accId。
// 注：用 #pragma pack（而非 __attribute__((packed))）——GCC 4.4 的 attribute
// packed 对含非 POD 成员（Key 有 user ctor）的外层 struct 不生效，会致 Key
// 偏移 4 对齐（0xc）而非 ORIG 0xa。
#pragma pack(push, 1)
struct Packet_TowerOfDespair_Statistic_GTS
{
    PacketHeader m_header;   // +0x00（packed 0xa 字节）
    TowerOfDespairStatistic_Key m_key;  // +0x0a
    unsigned char m_flag;    // +0x12
    unsigned int m_accId;    // +0x13

    Packet_TowerOfDespair_Statistic_GTS();
};
#pragma pack(pop)

#include "CDungeon/Packet_TowerOfDespair_Statistic_GTS.inc"
    : m_header(0x271c, 0x17)   // 成员序：header → key(默认构造) → flag/accId
{
    m_flag = 0;
    m_accId = 0;
}

#include "CDungeon/CDungeon_TowerOfDespair.inc"
{
    // ORIG C1/C2 0x08377606：先构造基类 CDungeon，再覆写 vtable 为 0x8c349f8
    //（唯一虚槽 _do_after_dungeon_start 指向派生实现）；成员构造由编译器生成。
}
#include "CDungeon/_do_after_dungeon_start.inc"
{
    // [b28d-solo-CDungeon2 判词 · IDENTICAL_AE] ORIG 0x0834d8ec / our 254 = ORIG 254；
    // p167 43=43 CLEAR。历史修法：①cMyTrace 全表达式临时链
    //   `cMyTrace(f,l,v)("...", user->getCharacNo(), slot, m_itemCount)`（P227 翻 N80）；
    //   ②proxy 链式内联实参（P0/P52）。本轮修法：
    //   ③`TOD_Layer todayLayer = ...; todayLayer.GetLayer();` 改全表达式链
    //     `todState->getTodayEnterLayer().GetLayer()`（P278/P0：ORIG 无 todayLayer 具名槽，
    //     sret 临时直接落 -0x16）⇒ gts 槽 -0x3b→-0x3f、sret -0x3e→-0x16、trace -0x24→-0x28
    //     三处一次对齐；
    //   ④TowerOfDespairStatistic_Key 写入口由 m_0(+0) 改 m_4(+4)（语义更正：
    //     m_4@+4 = packet+0xe；证据：ORIG ctor 0x08374cba `add $0xa` 后 call Key C1、
    //     权威头 shared/packet/include/Packet_TowerOfDespair_Statistic_GTS.h 的
    //     m_padA@+0xa / m_layer@+0xe）⇒ 写入口落 gts+0xe，与 ORIG 一致。
    //     成员名保持与 TowerOfDespairStatistic_Key.h 逐字一致（ODR）。
    // 结果：TU 口径 near 1→0、ae +1（本件 AE）。
    // ORIG 0x0834d8ec：进入绝望之塔——递增计数、二次进入扣门票、发包回执、统计。
    // 全调用点成立（GetCharacExpandData/IncreaseEnterCount/getEnterCount/check_item_exist/
    // GetInvenSlot/delete_item/cMyTrace×2/PacketGuard 族/getTodayEnterLayer/
    // GetServerProxy/SendPacket）。语义与 Ghidra C 逐条对应。
    if (user == 0)
        return false;

    TOD_UserState* todState =
        (TOD_UserState*)user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xd);
    todState->IncreaseEnterCount();
    if (todState->getEnterCount() > 1)
    {
        if (m_entryRequiredItem.m_enableFlag != 0)
        {
        // 二次及以上进入需持有门票物品（ORIG this+0x7f4/+0x7f8/+0x7fc）。
        int slot = user->getCurCharacInvenR()->check_item_exist(
            m_entryRequiredItem.m_itemIdx);
        if (slot == -1)
            return false;
        Inven_Item item =
            user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)1, slot);
        if (item.m_addInfo2 < m_entryRequiredItem.m_itemCount)
            return false;

        if (!user->getCurCharacInvenW()->delete_item(
                (INVEN_TYPE)1, slot, m_entryRequiredItem.m_itemCount,
                (eItemDelReason)0xe, true))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x96f, 5)(
                "CDungeon_TowerOfDespair::_do_after_dungeon_start, delete_item failed , User ch =%d , %d %d",
                user->getCurCharacNo(), slot, m_entryRequiredItem.m_itemCount);
            return false;
        }

        PacketGuard pg;
        pg.put_header(0, 0xe);
        pg.put_byte(0);
        pg.put_short(1);
        pg.put_short(slot);
        int rest = item.m_addInfo2 - m_entryRequiredItem.m_itemCount;
        if (rest == 0)
            pg.put_int(-1);
        else
            pg.put_int(item.m_addInfo);   // ORIG local_8b（item idx 字段）
        pg.put_int(rest);
        pg.put_byte(item.GetItemAttr());
        pg.put_short(item.m_fieldb);
        pg.put_byte(item.m_field0);
        pg.put_int(item.m_fieldd);
        pg.put_byte(item.m_amp.getAbilityType());
        pg.put_short(item.m_amp.getAbilityValue());
        pg.put_byte(0);
        pg.put_packet(item);
        pg.finalize(true);
        user->Send(pg);
        user->SaveInventory();
        }
    }

    Packet_TowerOfDespair_Statistic_GTS gts;
    gts.m_key.m_4 = todState->getTodayEnterLayer().GetLayer();
    gts.m_flag = 1;
    gts.m_accId = user->get_acc_id();

    GlobalData::s_statistic_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0)->SendPacket(
        (char*)&gts, sizeof(Packet_TowerOfDespair_Statistic_GTS));
    return true;
}
#include "CDungeon/_do_after_dungeon_start_1.inc"
{
    return true;
}

#include "CDungeon/GetMaze.inc"
{
    if ((unsigned int)idx >= m_mazeList.size())
        return 0;
    return (int)&m_mazeList[idx];
}

#include "CDungeon/GetRandMaze.inc"
{
	out = rand() % m_mazeList.size();
	(void)m_mazeList[out];
}

#include "CDungeon/GetRandMazebyPartyCount.inc"
{
    // [b28d-solo-CDungeon2 判词 · NEAR（DIFF→NEAR）] ORIG 0x0834d06c / our 131
    //（ORIG 133，gap 2）；p167 18=18 CLEAR。历史修法：两处 `(int)candidates[x]`
    // 改 `*(int*)&candidates[x]` 视图读（P266）；删 local_1c 具名局部；r 声明 int。
    // 本轮修法：删具名 `unsigned int sz` 改匿名 `rand() % m_mazeList.size()` /
    // `rand() % candidates.size()`（P0：ORIG 的 -0x2c 是 `divl` 的内存操作数临时槽，
    // 非具名局部）⇒ 4.4 临时槽落 -0x2c、帧 0x2ac→0x40 与 ORIG 全等。
    // 残余（N 封顶）：①ORIG 余数 `mov %edx,%ecx; mov %ecx,%eax` 双中转 vs 4.4
    // `mov %edx,%eax`（N18a 单段 regmove 不可造，×2 处）；②块内变量槽方向差
    //（elem -0x24 vs -0xc、i -0xc vs -0x10、r -0x10 vs -0x14、vector -0x20 vs -0x24；
    // 4.1 块内变量占浅槽 / 4.4 占深槽，N82/N93 族；声明序三变体 + elem 循环内声明
    // 实测均不可翻）。调用点差集/槽位集逐条核对成立。
	// ORIG 0x0834d06c：唯一迷宫直返；否则按 [m_partyCountMin, m_partyCountMax]
	// 收集候选；候选唯一/随机取，空则全表随机。
	if (m_mazeList.size() == 1)
		return (int)&m_mazeList[0];

	std::vector<MazeScript*> candidates;
	int r;
	unsigned int i;
	MazeScript* elem;

	i = 0;
	while (m_mazeList.size() > i)
	{
		elem = const_cast<MazeScript*>(&m_mazeList[i]);
		if (elem->m_partyCountMin <= partyCount &&
			partyCount <= elem->m_partyCountMax)
		{
			candidates.push_back(static_cast<MazeScript*&&>(elem));
		}
		i = i + 1;
	}

	r = 0;
	if (candidates.empty())
	{
		r = rand() % m_mazeList.size();
		return (int)&m_mazeList[r];
	}
	if (candidates.size() == 1)
		return *(int*)&candidates[0];
	r = rand() % candidates.size();
	return *(int*)&candidates[r];
}

#include "CDungeon/GetStartMap.inc"
{
    // ORIG 0x0834c75a：方向守卫共享一个 -1 返回块；空表 log 后 -1；
    // 随机前进 it++（后置）逐槽检查；返回 it->first。
    if (direction > 0xf || direction < 0)
        return -1;

    if (m_maplist[direction].size() == 0)
    {
        LogManager::logFormat(
            1, "data_manager.cpp", __PRETTY_FUNCTION__,
            0x79a,
            "CDungeon::GetStartMap() : direction(%d), dungeon index(%d), "
            "start_maplist_[direction].size() == 0, May be Script Error!",
            direction, m_index);
        return -1;
    }

    int r = get_rand_int(m_maplist[direction].size());
    __gnu_cxx::hash_map<int, CMap*>::const_iterator it =
        m_maplist[direction].begin();
    for (int i = 0; i < r; i++)
    {
        it++;
        if (it == m_maplist[direction].end())
            return -1;
    }
    return it->first;
}

#include "CDungeon/get_clear_item.inc"
{
	// ORIG 0x0834c8c0：ret 先置栈、迭代器默认构造、lower_bound 到临时再赋值
	int ret = 0;
	int key;
	std::map<int, int>::const_iterator it;
	key = get_rand_int(100) + 1;
	it = m_clearItemMap.lower_bound(key);
	if (it == m_clearItemMap.end())
		ret = -2;
	else
		ret = it->second;
	return ret;
}

#include "CDungeon/get_clear_item_1.inc"
{
	// ORIG 0x0834c95c：同 get_clear_item()，key = level + 1
	int ret = 0;
	int key;
	std::map<int, int>::const_iterator it;
	key = level + 1;
	it = m_clearItemMap.lower_bound(key);
	if (it == m_clearItemMap.end())
		ret = -2;
	else
		ret = it->second;
	return ret;
}

#include "CDungeon/getLimitCoinDiff.inc"
{
    // ORIG 0x08365316：size==0 → 0；命中取 value.m_0 写 out 返回 1；未命中返回 0
    if (m_map91c.size() == 0)
        return 0;
    std::map<int, UseCoinDungeonDiff>::const_iterator it = m_map91c.find(diff);
    if (it != m_map91c.end())
    {
        out = it->second.m_0;
        return 1;
    }
    return 0;
}

#include "CDungeon/checkFreeRevivalCondition.inc"
{
    // ORIG 0x08365298：it != end 且 !(value.m_4)（xor $1 取反测试）→ true
    std::map<int, UseCoinDungeonDiff>::const_iterator it = m_map91c.find(diff);
    return it != m_map91c.end() && !it->second.m_4;
}

#include "CDungeon/get_dimension_member_count.inc"
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

#include "CDungeon/clear.inc"
{
    m_gridValue = 0;
    m_mapCandidates.clear();
    m_secondCandidates.clear();
    m_layeredMapIndexes.clear();
}

#include "CDungeon/GridScript.inc"
{
    clear();
}

#include "CDungeon/RidableObject.inc"
{
    clear();
}


#include "CDungeon/clear_1.inc"
{
    m_0 = 0;
    m_4 = 0;
    m_objects.clear();
}

#include "CDungeon/RidableScript.inc"
{
    clear();
}

#include "CDungeon/dtor_RidableScript.inc"
{
}

#include "CDungeon/DungeonClearCondition.inc"
{
    clear();
}

#include "CDungeon/clear_2.inc"
{
    // 定义置于 ctor 之后：4.4 对"被调函数定义未前置"的 ctor 帧发 sub $0x18，
    // 与 ORIG 0x088f29f4 帧形一致（探针 /tmp/near7/p7-p8）。
    m_0 = 0;
    m_4 = 0;
    m_8 = 0;
}

// ============================================================================
// MazeScript（0x74 字节）
// ============================================================================
#include "CDungeon/clear_3.inc"
{
    // ORIG 0x88f2a76：宽高置 1；三组 vector clear；0x2c..0x54 置 -1；
    // 0x64..0x6c 置 0；0x70 字节清零；ridable clear。
    // ORIG 0x088f2a85 段：宽高置 1 是 `for (i=0;i<=1;++i) arr[i]=1` 索引循环
    // （`movl $0x1,(%eax,%edx,4)` + setle 回边），两条独立直写复现不了 ⇒
    // 经匿名 union 的数组视图 m_dims 写。
    for (int i = 0; i <= 1; ++i)
        m_dims[i] = 1;
    m_grids.clear();
    m_vec14.clear();
    m_vec20.clear();
    m_2c = -1;
    m_30 = -1;
    m_34 = -1;
    m_questType = -1;
    m_50 = -1;
    m_54 = -1;
    m_clearCondition.clear();
    m_64 = 0;
    m_partyCountMin = 0;
    m_partyCountMax = 0;
    m_70 = 0;
    m_ridable.clear();
}

#include "CDungeon/MazeScript.inc"
{
    clear();
}

#include "CDungeon/dtor_MazeScript.inc"
{
    // ORIG D1/D2 0x08376f00（合成形态）：逆序析构 m_clearCondition(+0x58)、
    // m_ridable(+0x38)、m_vec20/m_vec14/m_grids；由编译器按成员声明生成。
}

#include "CDungeon/getGrid.inc"
{
    // ORIG 0x88f2c2a：返回 &m_grids[m_width*y+x]（vector::operator[] 结果）
    return &m_grids[m_width * y + x];
}

// ============================================================================
// stDungeonAssignItem_t / RandomList / 事件怪结构
// ============================================================================

#include "CDungeon/clear_4.inc"
{
    m_assignItemIdx = -1;
    m_list.clear();
}

#include "CDungeon/stDungeonAssignItem_t.inc"
{
    clear();
}

#include "CDungeon/dtor_stDungeonAssignItem_t.inc"
{
}

// ============================================================================
// STMonsterTypeSpawnProb / STDeadTowerData / STWarRoomData / RequiredItem
// ============================================================================

// ORIG W 0x088f2c64（报告缺失兜底：nm 定址+objdump 切片；清单给
// docs/class_func_reports/CCharacter/clear.md 系撞名报告，签名为
// _ZN10CCharacter5clearEv，已按 §6.3 弃用）：clear 整体 memset(this,0,0x10)。
// [b22判词·跨版本] 残余 1 序差：4.4 填 memset 出参槽先于取 this，4.1 先生成
// `mov 0x8(%ebp),%eax`（指令集相同仅两 mov 对调，三变体实测 4.4 无法复现前序）；
// call 目标 reloc 即 memset（§6.3 占位假阳性）。语义等价。
#include "CDungeon/clear_5.inc"
{
    // P68 正向：ORIG 0x88f2c6a 实参序「this 先物化→0x10→0x0」＝ 带 cast 形。
    memset((char*)this, 0, sizeof(STMonsterTypeSpawnProb));
}

#include "CDungeon/STMonsterTypeSpawnProb.inc"
{
    clear();
}

#include "CDungeon/clear_6.inc"
{
    m_vec.clear();
}

#include "CDungeon/STDeadTowerData.inc"
{
    clear();
}

#include "CDungeon/dtor_STDeadTowerData.inc"
{
    // ORIG D1/D2 0x0836b41c（合成形态）：仅析构 m_vec（+0x00）。
}

#include "CDungeon/STWarRoomData.inc"
{
    // ORIG C1/C2 0x0836a1f6：构造 m_vec14[4]（+0x14）/m_vec44（+0x44）/
    // m_vec6c[4]（+0x6c）/m_spawnProb（+0xb0）后调用 clear()。
    clear();
}

#include "CDungeon/clear_7.inc"
{
    // ORIG 0x836a398：两个 vector<int>[4] 数组循环 + 标量区。
    memset(&m_0, 0, 0xc);
    m_c = 0;
    m_10 = -1;
    for (int i = 0; i < 4; ++i)
        m_vec14[i].clear();
    m_vec44.clear();
    m_50 = 0;
    m_54 = 0;
    m_58 = 0;
    memset(m_5c, 0, sizeof(m_5c));
    for (int i = 0; i < 4; ++i)
        m_vec6c[i].clear();
    memset(m_9c, 0, sizeof(m_9c));
    m_ac = 0;
    m_spawnProb.clear();
    m_bc = 0;
    m_c0 = 0;
    memset(m_c4, 0, sizeof(m_c4));
    m_d4 = 0;
    memset(m_dc, 0, sizeof(m_dc));
    m_d8 = 0;
    m_ec = 0;
    m_f0 = 0;
    m_f4 = 0;
    m_f8 = 0;
    m_fc = 0;
}

#include "CDungeon/copy.inc"
{
    // [b28d-solo-CDungeon 判词 · IDENTICAL_AE（gap 0）] ORIG 0x0836a578 / our 298 = ORIG 298；
    // p167 24=24 CLEAR。本轮修法：①m_vec14/m_vec6c/m_dc 合并进同一 i 循环 ——
    //   ORIG 0x836a5bc..0x836a759 单循环含三段（旧判词「ORIG 不写 m_dc」系地址算错：
    //   ORIG `add $0x34,%edx; mov 0xc(%eax,%edx,4),%edx` = m_dc[i]，+0xdc+4i，
    //   P171 照抄恢复 `other->m_dc[i] = m_dc[i];`）；②首个 memcpy 实参改
    //   `(char*)other, (const char*)this` 复现 ORIG 先备 this→%edx/other→%eax 再压
    //   size 的装载序（P68 形；(void*) 形反劣）。残余仅首处 memcpy 装载时刻与槽号
    //（N82/P75 跨版本）。结构整体赋值 rep movsl 分块启发式差（N84/N90）不在此件。
    // ORIG 0x836a578：数组按元素 reserve/push_back，标量逐个/memcpy。
    memcpy((char*)other, (const char*)this, 0xc);
    other->m_c = m_c;
    other->m_10 = m_10;
    for (int i = 0; i < 4; ++i)
    {
        other->m_vec14[i].reserve(m_vec14[i].size());
        for (size_t j = 0; j < m_vec14[i].size(); ++j)
            other->m_vec14[i].push_back(m_vec14[i][j]);
        other->m_vec6c[i].reserve(m_vec6c[i].size());
        for (size_t j = 0; j < m_vec6c[i].size(); ++j)
            other->m_vec6c[i].push_back(m_vec6c[i][j]);
        other->m_dc[i] = m_dc[i];
    }
    other->m_vec44.reserve(m_vec44.size());
    for (size_t i = 0; i < m_vec44.size(); ++i)
        other->m_vec44.push_back(m_vec44[i]);
    other->m_50 = m_50;
    other->m_54 = m_54;
    other->m_58 = m_58;
    memcpy(other->m_5c, m_5c, sizeof(m_5c));
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
    other->m_ec = m_ec;
    other->m_f0 = m_f0;
    other->m_f4 = m_f4;
    other->m_f8 = m_f8;
    other->m_fc = m_fc;
}

#include "CDungeon/dtor_STWarRoomData.inc"
{
}

#include "CDungeon/RequiredItem.inc"
{
    clear();
}

// clear 定义移到 ctor 之后（NL7 实证：callee 后置时 4.4 unit-at-a-time 不塌缩
// ctor outgoing，保持 ORIG sub $0x18）。
#include "CDungeon/clear_8.inc"
{
    m_itemIdx = -1;
    m_itemCount = 0;
    m_enableFlag = 0;
}
#include "CDungeon/clear_9.inc"
{
    // ORIG：vector<_RewardItem> m_vec[3] 数组循环 clear（i=0..2）。
    for (int i = 0; i < 3; ++i)
        m_vec[i].clear();
}

#include "CDungeon/ClearRewardData.inc"
{
    // ORIG 0x8810d5e：仅 3 个 vector 默认构造，无 clear() 调用。
}

#include "CDungeon/dtor_ClearRewardData.inc"
{
}

advancealtar::ClearRewardData& advancealtar::ClearRewardData::operator=(
    const ClearRewardData& other)
{
    // [b28d-solo-CDungeon 判词 · N18a 封顶] ORIG 0x08376c9c / our 27（ORIG 28，
    // gap 1）；p167 1=1 CLEAR。语义已按 ORIG 修正（--n 前置测试只拷前 2 个元素，
    // P171 照抄）；指针步进 do-while（esi=dst/ebx=src 各 +0xc、edi=2 递减到 -1、
    // `setne/test/jne`）；声明序 dst/n/src 对齐 ORIG 装载序（esi→edi→ebx）。
    // 残余唯一：ORIG `mov 0x8(%ebp),%eax; mov %eax,%esi` 被 4.4 折成单条
    // `mov 0x8(%ebp),%esi`（单段 regmove 不可造）。本轮再试 `&m_vec[0]` 形仍折
    // 单段；g1-g6/h1-h6 十二形探针（语句表达式/整型-void*-char* 中转/具名引用）+
    // register 存储类保留均不可翻 ⇒ N18a。[b28d-solo-CDungeon2 复验] fresh 编译
    // 仍唯一残差；根因归 N102（结构首成员数组 m_vec@+0 退化取址，4.4 恒折单条
    // disp），不再试形。
    register std::vector<advancealtar::_RewardItem>* dst = m_vec;
    register int n = 2;
    register const std::vector<advancealtar::_RewardItem>* src = other.m_vec;
    // ORIG 0x08810d8a 为 `--n != -1` 前置测试 while：n=2 只拷贝 2 个元素（P171 照抄）。
    while (--n != -1)
    {
        *dst = *src;
        ++dst;
        ++src;
    }
    return *this;
}

#include "CDungeon/SurvivalRewardData.inc"
{
}

#include "CDungeon/dtor_SurvivalRewardData.inc"
{
}

advancealtar::SurvivalRewardData&
advancealtar::SurvivalRewardData::operator=(const SurvivalRewardData& other)
{
    m_vec = other.m_vec;
    return *this;
}

#include "CDungeon/Clear.inc"
{
    m_c = 0;
    m_items.clear();
}

#include "CDungeon/TournamentRewardItemRate.inc"
{
    Clear();
}

#include "CDungeon/dtor_TournamentRewardItemRate.inc"
{
}

TournamentRewardItemRate& TournamentRewardItemRate::operator=(
    const TournamentRewardItemRate& other)
{
    m_items = other.m_items;
    m_c = other.m_c;
    return *this;
}

#include "CDungeon/stConditionEnterBossRoom_t.inc"
{
    // ORIG 0x836b552：仅 vector<int> 构造（m_0 不初始化）。
}

#include "CDungeon/dtor_stConditionEnterBossRoom_t.inc"
{
}

#include "CDungeon/clear_10.inc"
{
    m_0 = 0;
    m_vec4.clear();
}

#include "CDungeon/RecoveryStairInfo.inc"
{
}

#include "CDungeon/dtor_RecoveryStairInfo.inc"
{
    // ORIG D1/D2 0x083dc7d2（合成形态）：仅析构 m_vec（+0x0c）。
}

#include "CDungeon/clear_11.inc"
{
    // ORIG W 0x088f2d38：m_index=0、m_name.clear()、三个 flag 字节清零。
    m_index = 0;
    m_name.clear();
    m_flag8 = 0;
    m_flag9 = 0;
    m_flagA = 0;
}

#include "CDungeon/STTowerDialog.inc"
{
    clear();
}

#include "CDungeon/dtor_STTowerDialog.inc"
{
}

STTowerDialog& STTowerDialog::operator=(const STTowerDialog& other)
{
    m_index = other.m_index;
    m_name = other.m_name;
    m_flag8 = other.m_flag8;
    m_flag9 = other.m_flag9;
    m_flagA = other.m_flagA;
    return *this;
}

#include "CDungeon/clear_12.inc"
{
    m_vec0.clear();
    m_vecc.clear();
    m_vec18.clear();
}

#include "CDungeon/STTowerFullDialog.inc"
{
    clear();
}

#include "CDungeon/dtor_STTowerFullDialog.inc"
{
}

// ---- ORIG W 弱符号发射段（头内联定义，本 TU 无 odr-use 故未发射）----
//   TournamentRewardItem  C1/C2 @0x088f2dcc（三字段清零）、D1/D2 @0x088f9ef6（空体）
//   STNamedMonsterMapPos  C1/C2 @0x088f2d6c（m_y=0; m_x=m_y）、D1/D2 @0x088f8cbc（空体）
// 真实构造/析构强制发射，不引入导出符号。
namespace
{
#include "CDungeon/emitDungeonRewardWeakSymbols.inc"
{
    TournamentRewardItem item;      // C1/C2
    item.~TournamentRewardItem();   // D1/D2
    STNamedMonsterMapPos pos;       // C1/C2
    pos.~STNamedMonsterMapPos();    // D1/D2
}
}

