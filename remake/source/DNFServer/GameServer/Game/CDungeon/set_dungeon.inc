void CDungeon::set_dungeon(STDungeonScript& script)
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
    // [L6S2 收尾复核 · §6.2 三件套（2026-09-11）]
    //  ①gap：ORIG 803~804 vs our 802；唯一来源 = 0x0834b15c `jmp 0x0834b15f` +
    //    0x0834b15e `nop`（4.1 if-else 汇聚岛：失败路 `je 0x0834b15e` 落 nop、成功路
    //    体尾 jmp 跳岛回循环条件 0x0834b15f）——§6.4 属「ORIG 有我方无」命中，判 N112。
    //  ②p167：本 TU .o 逐 call 目标**真差集 = 0**；报告所列 MISSING/EXTRA 全部为
    //    C1/C2、D1/D2 同址别名伪差（P93；nm 实证同址：cMyTrace 0x0854f718、pairIiiE
    //    0x081b5c8e、pairIKiiE 0x082faac0、pairIKiSt6vector 0x083895fc、
    //    _Rb_tree_const_iterator 0x08389550、D1/D2 0x08376d12/0x08376cfc）。
    //  ③帧/槽：sub $0x2bc 两侧全等；深区（≥ -0x8c：nameBuf -0x29c、totalRate、迭代器
    //    组…）槽位全等；残差仅中间区 4B（trace -0x84 vs -0x88、rit -0x74 vs -0x78、
    //    p -0x70/-0x6c vs -0x74/-0x70）——该区对象数我方比 ORIG 少 1 个 4B 槽
    //    （我方 -0x88 为空洞；4.4 把 cMyTrace(16B)+end(4B)+pair(8B) 自 -0x84 起连续
    //    排布，ORIG 自 -0x88 起），pad0/pad1 死局部只推动深区 ⇒ N82/N93 族。
    //  ⇒ 判「语义完成、byte 层 N112 + N82/N93 封顶」。
    //  [L6S2 同轮修复] logFormat 第三参改 __PRETTY_FUNCTION__（P139，0x0834b126 直证，
    //    与 0x0834ad91 首个 trace 块同一 _ZZ…E19__PRETTY_FUNCTION__ 符号 0x8c34980）。
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
                // [L6S2 · P139 直证] ORIG 0x0834b126 `movl $&_ZZN8CDungeon11set_dungeon
                // ER15STDungeonScriptE19__PRETTY_FUNCTION__,0x8(%esp)`——与本函数首个
                // trace 块 0x0834ad91 同一符号（0x8c34980）⇒ 第 3 参是
                // __PRETTY_FUNCTION__（局部静态符号），旧译手写串既不生成该符号、
                // 也走匿名 .rodata（符号差集 + §6.5 内容盲区）。
                LogManager::logFormat(
                    1, "data_manager.cpp", __PRETTY_FUNCTION__, 0x5e1,
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