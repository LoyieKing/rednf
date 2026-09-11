void CRelayBattleMgr::ArrangeChangeSequence(std::vector<int>& seq, int team,
                                            int& outIdx)
{
    for (int i = 0; i < 8; ++i)
    {
        if (m_battles[i].m_team == team && m_battles[i].m_state == 1 &&
            m_battles[i].m_requestFight)
        {
            m_battles[i].m_seq = (short)seq[outIdx++];
        }
    }
}