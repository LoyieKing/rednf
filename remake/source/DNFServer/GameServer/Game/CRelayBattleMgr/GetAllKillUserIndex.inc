int CRelayBattleMgr::GetAllKillUserIndex(int* out)
{
    for (int i = 0; i < 8; ++i)
    {
        if (m_battles[i].m_killCount > 1 &&
            (signed char)m_battles[i].m_killCount >= m_readyCount / 2 &&
            m_battles[i].m_state == 2)
        {
            if (out != 0)
            {
                *out = m_battles[i].m_team;
            }
            return i;
        }
    }
    return -1;
