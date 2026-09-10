void WarRoom::BattleReset()
{
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] != 0)
        {
            (m_slotsUser[i])->getHackAnalyzer()->reportHackInfo();
            (m_slotsUser[i])->getHackAnalyzer()->resetHackInfo();
            (m_slotsUser[i])->ResetCoinCount();
        }
        m_liveFlag[i] = 0;
        m_slotData[i] = 0;
        m_slotFp[i] = 0;
        m_hostVotes[i] = -1;
    }
    m_field.Reset();
    memset(m_roleKillCnt, 0, sizeof(m_roleKillCnt));
    m_userCount = 0;
    m_flag10 = 0;
    m_flag11 = 0;
    m_flag12 = 0;
    CurSpawnStepReset();
