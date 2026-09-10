void CBattle_Field::BloodSpawnTimerManager(bool onStart, bool isMinimal)
{
    // [b28d-solo-CBattle_Field_missing] ORIG 0x08304dd2：
    // ①receiver = m_party->m_battleField.m_bloodReward（CParty+0xb24 内嵌
    //   CBattle_Field + m_bloodReward+0x2b8 = m_party+0xddc，ORIG 实证）；
    // ②局部集 round(-0x14)/smallRound(-0x12)/last(-0x10)/isFirst(-0xd)/
    //   interval(-0xc)，无具名 r（ORIG 直比 %eax，sete 物化）。
    // ③残余 1 条：ORIG BODY 内 `cmpb $0,-0x1c(%ebp)` 为死比较（无分支消费者，
    //   4.1 逐条发射），4.4 的 `if (onStart)` 必产 je ⇒ P184/N184 死块变体封顶。
    const short roundOrder = (short)m_bloodRound.getBloodRoundOrder();
    const short smallRoundOrder = (short)m_bloodRound.getBloodSmallRoundOrder();
    const short lastSmallRoundOrder = (short)m_bloodRound.getBloodLastSmallRoundOrder();
    (void)lastSmallRoundOrder;
    if (smallRoundOrder > lastSmallRoundOrder)
        return;
    if (!checkBloodSpawnOrder(smallRoundOrder))
    {
        LogManager::logFormat(
            1, "battle_field.cpp",
            __PRETTY_FUNCTION__, 0xd0c,
            "BLOOD_LOG : round_order_ (%d), small_round_order_ (%d) : SET ORDER ERROR!!\n",
            (int)roundOrder, (int)smallRoundOrder);
        return;
    }
    bool isFirst = false;
    if (smallRoundOrder == 0)
        isFirst = true;
    int interval = get_blood_spawn_small_round_interval(smallRoundOrder);
    if (interval < 1)
    {
        LogManager::logFormat(
            1, "battle_field.cpp",
            __PRETTY_FUNCTION__, 0xd19,
            "BLOOD_LOG : small_round_order_ (%d) : ABNOMAL ORDER!!\n",
            (int)smallRoundOrder);
        return;
    }
    if ((onStart && !isMinimal) || isFirst)
    {
        // [判据·多余门禁] ORIG 0x08304f04 的 `cmpb $0x0,-0x1c(%ebp)`（onStart spill
        // 槽）无任何分支消费者（其后 mov/mov/add 覆盖 flags），且 0x08304f08 起
        // 无条件构造 receiver 并 call onStartBloodRound ⇒ 原码此处**无 if(onStart)
        // 门禁**（旧版 `if (onStart)` 为多余门禁真 bug：isFirst 为真而 onStart 为假
        // 时 ORIG 仍调用 onStartBloodRound，我方旧码跳过）。
        // 该死比较用 P225 dead-test 复现形 `while (expr) break;` 保留（4.4 对空 if
        // 体整块 DCE，探针 /tmp/probe2.cpp h2 实证 while-break 留 cmpb 无跳转）。
        while (onStart)
            break;
        m_party->m_battleField.m_bloodReward.onStartBloodRound();
        BloodRoundIntervalPacket(roundOrder, interval);
    }
    if (BloodPhaseSpawnTimerResgister(smallRoundOrder) == -1)
    {
        LogManager::logFormat(
            1, "battle_field.cpp",
            __PRETTY_FUNCTION__, 0xd2b,
            "BLOOD_LOG : small_round_order_ (%d) : ABNOMAL ORDER!!\n",
            (int)smallRoundOrder);
    }
    return;
