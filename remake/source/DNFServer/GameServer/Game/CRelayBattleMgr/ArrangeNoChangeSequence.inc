void CRelayBattleMgr::ArrangeNoChangeSequence(std::vector<int>& seq, int team,
                                              int& outIdx)
{
    for (int i = 0; i < 8; ++i)
    {
        if (m_battles[i].m_team == team && m_battles[i].m_state == 1)
        {
            // [b29 P248档②] 同 OnRequestFight：bool 字段 + 裸形。
            if (!m_battles[i].m_requestFight)
            {
                m_battles[i].m_seq = (short)seq[outIdx++];
            }
        }
    }
