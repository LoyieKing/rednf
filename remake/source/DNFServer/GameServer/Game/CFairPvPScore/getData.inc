void CFairPvPScore::getData(char* pData) const
{
    SIG_FAIR_PVP_SCORE* sig = reinterpret_cast<SIG_FAIR_PVP_SCORE*>(pData);
    int idx = 0;
    std::map<JobKey, FAIR_PVP_SCORE_MODE_COUNT>::const_iterator it =
        m_jobScore_.begin();
    for (idx = 0; it != m_jobScore_.end() && idx <= 99; ++it, ++idx)
    {
        // ORIG 基址形态 (idx+2)*0x10 + 0x10（084a55e1 `lea 0x2;shl 4; movb 0x10(%ecx,%eax)`）。
        reinterpret_cast<FairPvpScoreEntryPair*>(
            reinterpret_cast<char*>(sig) + (idx + 2) * 0x10)->m_e1.m_job =
            (unsigned char)(unsigned int)it->first.m_job;
        reinterpret_cast<FairPvpScoreEntryPair*>(
            reinterpret_cast<char*>(sig) + (idx + 2) * 0x10)->m_e1.m_sub =
            (unsigned char)(unsigned int)it->first.m_sub;
        *(FAIR_PVP_SCORE_MODE_COUNT*)&reinterpret_cast<FairPvpScoreEntryPair*>(
            reinterpret_cast<char*>(sig) + (idx + 2) * 0x10)->m_e1.m_cnt0 = it->second;
        ++sig->m_jobScoreCount;                          // 字节自增（ORIG movzbl/lea/movb）
    }

    std::list<unsigned char>::const_iterator it2 = m_recentResults_.begin();
    // ORIG 084a56f0/084a56fa：先 `addl $1,-0xc` 再 operator++（increment 序与 map 循环相反）。
    for (idx = 0; it2 != m_recentResults_.end() && idx <= 9; ++idx, ++it2)
        sig->m_recentResults[idx] = *it2;

    for (idx = 0; idx <= 1; ++idx)
    {
        sig->m_modeStats[idx][0] = m_modeStats[idx][0];
        sig->m_modeStats[idx][1] = m_modeStats[idx][1];
        sig->m_modeStats[idx][2] = m_modeStats[idx][2];
    }
    sig->m_field1c = m_field40;
    sig->m_field20 = m_field44;
    sig->m_winStreak = m_winStreak;
    sig->m_bestWinStreak = m_bestWinStreak;
    sig->m_dailyPlayCount = m_dailyPlayCount;
    sig->m_lastPlayTime = m_field54;   // unsigned→int 异型：ORIG 084a57d3 两段中转
    sig->m_giveItem = m_giveItem;
