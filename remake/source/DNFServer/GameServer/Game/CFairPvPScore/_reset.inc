// ---- ORIG 0x84a5942 _reset() ------------------------------------------------
void CFairPvPScore::_reset()
{
    m_jobScore_.clear();
    m_recentResults_.clear();
    for (int i = 0; i < 2; ++i)
    {
        m_modeStats[i][0] = 0;
        m_modeStats[i][1] = 0;
        m_modeStats[i][2] = 0;
    }
    m_field40 = 0;
    m_field44 = 0;
    m_winStreak = 0;
    m_bestWinStreak = 0;
    m_dailyPlayCount = 0;
    m_field54 = 0;
    m_lastTrainingSealCount = 0;
    m_lastTrainingSealBonusCount = 0;
    m_remainBonusCount = 0;
    m_giveItem = 0;
}