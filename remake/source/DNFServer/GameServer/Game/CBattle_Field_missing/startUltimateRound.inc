void CBattle_Field::startUltimateRound()
{
    const short roundOrder = (short)m_bloodRound.getBloodRoundOrder();
    const short smallRoundOrder = (short)m_bloodRound.getBloodSmallRoundOrder();
    // ORIG 第三次 call 结果死存槽 -0xe（-O0 无条件发射具名局部写入，P204 口径）。
    const short lastSmallRoundOrder = (short)m_bloodRound.getBloodLastSmallRoundOrder();
    (void)lastSmallRoundOrder;
    int nextDiff = getNextRoundDifficulty(roundOrder);
    if (nextDiff != 1)
    {
        if (nextDiff == 0)
        {
            LogManager::logFormat(1, "battle_field.cpp",
                                  __PRETTY_FUNCTION__, 0xf04,
                                  "ULTIMATE_LOG : ULITMATE ROUND (%d) startUltimateRound() ERROR!!\n",
                                  (int)roundOrder);
            return;
        }
        if (!forceValuesToChangeRound(roundOrder, smallRoundOrder, nextDiff))
        {
            LogManager::logFormat(1, "battle_field.cpp",
                                  __PRETTY_FUNCTION__, 0xf0a,
                                  "ULTIMATE_LOG : startUltimateRound() ERROR!!\n",
                                  (int)roundOrder);
            return;
        }
    }
    BloodSpawnTimerManager(true, false);
