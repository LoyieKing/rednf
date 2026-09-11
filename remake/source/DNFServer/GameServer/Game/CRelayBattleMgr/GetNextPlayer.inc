int CRelayBattleMgr::GetNextPlayer(int team)
{
    int best = -1;
    int bestSeq = 0;
    int i = 0;
    while (i <= 7)
    {
        if (m_battles[i].m_team == team)
        {
            switch (m_battles[i].m_state)
            {
            case 2:
                return -1;
            case 1:
                if (bestSeq < m_battles[i].m_seq)
                {
                    bestSeq = m_battles[i].m_seq;
                    best = i;
                }
                break;
            }
        }
        ++i;
    }
    m_battles[best].m_state = 2;
    return best;
}