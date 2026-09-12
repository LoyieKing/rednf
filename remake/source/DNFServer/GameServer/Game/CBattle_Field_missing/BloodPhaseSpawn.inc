// ---------------------------------------------------------------------------
// CBattle_Field::BloodPhaseSpawn(short smallRound, short& nextRound,
//                                int timerKey, int phaseIdx)
//   —— ORIG T 0x0830553a（报告 BloodPhaseSpawn.md 逐指令转写）。
// ---------------------------------------------------------------------------
void CBattle_Field::BloodPhaseSpawn(short smallRound, short& nextRound,
                                    int timerKey, int phaseIdx)
{
    // [round5 收口实测] ndiff=123（285/288）：帧 0x7c vs 0x6c（+0x10）且全槽号整体
    // 平移 0x10（形参槽 -0x5c vs -0x4c、-0x44 vs -0x3c …）⇒ N115（4.4 恒预留 16B
    // 对齐 outgoing 区）/N126（版本首槽基址常数）族；p167 32=32 CLEAR ⇒ 无语义缺口。
    // [b28d-solo-CBattle_Field_missing 判词 · N82/N93] ORIG 0x0830553a：删 hi/tq
    // 具名局部后帧 0x7c vs ORIG 0x6c（仍差 0x10 槽基 + 3 条指令），N82 lifetime
    // 槽复用/槽基族；逐字段语义等价。
    const short spawnCnt = (short)get_blood_spawn_cnt_of_phase(smallRound, nextRound);
    int spawnTime = get_blood_spawn_time_of_phase(smallRound);
    const short roundOrder = (short)m_bloodRound.getBloodRoundOrder();
    const short smallRoundOrder = (short)m_bloodRound.getBloodSmallRoundOrder();
    const short lastSmallRoundOrder = (short)m_bloodRound.getBloodLastSmallRoundOrder();

    if (spawnCnt < 0 || spawnTime < 0)
    {
        LogManager::logFormat(1, "battle_field.cpp",
                              "void CBattle_Field::BloodPhaseSpawn(short, short&, int, int)",
                              0xdb8,
                              "BLOOD_LOG : small_round_order_ (%d) : ABNOMAL ORDER!!\n",
                              (int)smallRound);
        return;
    }
    if (m_pMap != 0 && lastSmallRoundOrder == smallRound)
    {
        if (m_pMap->get_map_type() == 1)
            spawnTime = -1;
        else
            spawnTime = 0;
    }
    // ORIG 0x08305672：spawnTime 调整后立即发包 BloodMonsterSpawnPacket(cnt, time, smallRound, nextRound)
    BloodMonsterSpawnPacket(spawnCnt, spawnTime, smallRound, nextRound);
    const int totalPhase = get_blood_spawn_phase_total_cnt_of_small_round(smallRound);
    if (totalPhase < 0)
    {
        LogManager::logFormat(1, "battle_field.cpp",
                              "void CBattle_Field::BloodPhaseSpawn(short, short&, int, int)",
                              0xdcc,
                              "BLOOD_LOG : small_round_order_ (%d) : ABNOMAL ORDER!!\n",
                              (int)smallRound);
        return;
    }
    if ((int)nextRound < totalPhase - 1)
    {
        const int key = (int)m_party->gen_timer_key((TIMER_MESSAGE)timerKey);
        G_TimerQueue()->InsertTimerInMilisecond((TimerEntry::OBJ_TYPE)1,
                                                m_party->GetPartyIndex(),
                                                (TIMER_MESSAGE)timerKey,
                                                (long long)spawnTime,
                                                key, 0);
    }
    else
    {
        m_bloodSpawnTime[phaseIdx] = 0;
        // [R3a11 语义核验·真差异修复] ORIG 索引是 param_1(smallRound) 而非 *nextRound：
        // 0x08305762 `movswl -0x4c(%ebp),%edx`（-0x4c = 入口 `mov %ax,-0x4c(%ebp)` spill
        // 的 smallRound，全函数仅入口写一次）→ `mov %edx,0x8(%esp)` → 0x0830577d
        // `call <vector<bool>::operator[]>`（0x264=&m_bloodPhaseClear）⇒ 复刻 ORIG。
        m_bloodPhaseClear[smallRound] = true;
        if (checkBloodSpawnOrder(smallRoundOrder))
        {
            if (lastSmallRoundOrder < smallRoundOrder)
            {
                m_bloodRound.incBloodRoundOrder();
                m_bloodRound.setPreMapBloodRoundOrder();
                return;
            }
            int curRound = get_blood_round_of_small_round(smallRoundOrder);
            int prevRound = get_blood_round_of_small_round(smallRoundOrder - 1);
            int curDiff = get_blood_round_difficylty_of_small_round(smallRoundOrder);
            int prevDiff = get_blood_round_difficylty_of_small_round(smallRoundOrder - 1);
            if (curRound < 0 || prevRound < 0)
            {
                LogManager::logFormat(
                    1, "battle_field.cpp",
                    "void CBattle_Field::BloodPhaseSpawn(short, short&, int, int)",
                    0xdee,
                    "BLOOD_LOG : small_round_order_ (%d) : ABNOMAL ROUND!!\n",
                    (int)smallRoundOrder);
                return;
            }
            int nextTime = get_blood_spawn_time_of_next_small_round(smallRound);
            if (nextTime < 1)
            {
                LogManager::logFormat(
                    1, "battle_field.cpp",
                    "void CBattle_Field::BloodPhaseSpawn(short, short&, int, int)",
                    0xdf5,
                    "BLOOD_LOG : small_round_order_ (%d) : ABNOMAL ORDER!!\n",
                    (int)smallRoundOrder);
                return;
            }
            if (prevRound == curRound && prevDiff == curDiff)
            {
                const int key = (int)m_party->gen_timer_key((TIMER_MESSAGE)0x16);
                G_TimerQueue()->InsertTimerInMilisecond((TimerEntry::OBJ_TYPE)1,
                                                        m_party->GetPartyIndex(),
                                                        (TIMER_MESSAGE)0x16,
                                                        (long long)nextTime,
                                                        key, 0);
            }
            else
            {
                m_bloodRound.incBloodRoundOrder();
                m_bloodRound.setPreMapBloodRoundOrder();
            }
        }
    }
    nextRound = (short)(nextRound + 1);
    (void)roundOrder;
}