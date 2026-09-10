bool CBattle_Field::add_blood_monster(map_monster& mob, int& uid, int& seq,
                                      std::vector<STAssignBloodSystem> systems,
                                      std::vector<STAssignBloodPhaseTime> phaseTimes)
{
    if (phaseTimes.empty() || systems.empty())
        return false;

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
        // P225/探针 /tmp/probe_crazy.cpp fB：ORIG `cmp $1; sete %al; test %al,%al;
        // je` + 两分支各自 `mov %ax,-0x24`（0x083048c5..0x083048ea）⇒ 源写 if/else；
        // 三目在 4.4 产 `jne` + 汇合点单次存储（少 2 条 + 存储位点错）。
        if (m_pMap->get_map_type() == 1)
            crazyRound = get_rand_int(4);
        else
            crazyRound = get_rand_int(5);
    }
    if (m_pDungeon->m_bloodDungeonType == 2)                      // CDungeon+0x89f（无判空，忠实 ORIG）
        crazyRound = -1;

    // P28 增量侧：ORIG 增量段 `ppEv`（0x083118e0）⇒ 源写 `++it`；smallRoundOrder++
    // 在 ORIG 中紧随 `++it` 之后（0x08304d5d→0x08304d6b）⇒ 并入 for 增量逗号式。
    for (std::vector<STAssignBloodPhaseTime>::iterator it = phaseTimes.begin();
         it != phaseTimes.end(); ++it, smallRoundOrder++)
    {
        // P167 新判据：ORIG 循环体三次 `call ptEv`（operator->()，0x083118bc）⇒ 源写 `it->`。
        phaseKey = it->m_smallRoundOrder;            // 阶段键 +0x00
        systemIdx = it->m_phaseIdx;                  // 系统序号 +0x04
        if (it->m_phaseIdx == -1)
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
            // S290 union 双视角：ORIG 对 +0x11/+0x12 发两条独立 movb
            // （0x08304b99/0x08304ba0），+0x34 亦清零（0x08304bb5）⇒ 逐字节写。
            mob.m_field11b.m_field11b0 = 0;                  // +0x11
            mob.m_field11b.m_field11b1 = 0;                  // +0x12
            mob.m_field13 = 0;                               // +0x13
            mob.m_field14 = 0;                               // +0x14
            mob.m_flag34 = 0;                                // +0x34
            mob.m_bloodSmallRoundOrder = smallRoundOrder;    // +0x33a ← 阶段序
            // P243：ORIG 先 flds 0xc(%eax)（m_time）后 flds 常量 ⇒ 书写序 m_time 在前。
            mob.m_bloodPhaseTime = it->m_time * 1000.0f;     // +0x33c（常量 0x8c23d70=1000.0f）
            mob.m_bloodRoundOrder = systems[systemIdx].m_phase;   // +0x33e ← 系统+0x08
            mob.m_bloodSpawnIdx = systems[systemIdx].m_field0c;   // +0x340 ← 系统+0x0c
            mob.m_bloodPhaseIdx = systems[systemIdx].m_field10;   // +0x342 ← 系统+0x10
            mob.m_bloodKillFlag = 0;                         // +0x344
            mob.m_bloodRoundOrder2 = it->m_smallRoundOrder;       // +0x346 ← 阶段+0x00
            mob.m_bloodSpawned = it->m_difficulty;                // +0x348 ← 阶段+0x18

            GetCurrentMapInfo()->Add_Mob(mob);               // 实参槽直接 copy-ctor
            crazyType = 0x13;                                // 复位默认类型

        }

        // P0：ORIG 结果落具名槽 -0x18（0x08304d4d `mov %eax,-0x18(%ebp)`）⇒ 具名局部。
        CMonster* found =
            static_cast<CMonster*>(G_CDataManager()->find_monster(mob.m_mobIndex));
        if (found == NULL)
            return false;
    }

    m_bloodRound.setBloodLastSmallRoundOrder(smallRoundOrder - 1);  // +0x2ac
    return true;
