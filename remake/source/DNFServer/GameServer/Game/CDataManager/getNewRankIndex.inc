unsigned int CDataManager::getNewRankIndex(int idx) const
{
    if (m_rankSystem.m_dungeonRank.m_rankLevel[0] <= idx) return 0;
    if (m_rankSystem.m_dungeonRank.m_rankLevel[1] <= idx) return 1;
    if (m_rankSystem.m_dungeonRank.m_rankLevel[2] <= idx) return 2;
    if (m_rankSystem.m_dungeonRank.m_rankLevel[3] <= idx) return 3;
    if (m_rankSystem.m_dungeonRank.m_rankLevel[4] <= idx) return 4;
    if (m_rankSystem.m_dungeonRank.m_rankLevel[5] <= idx) return 5;
    if (m_rankSystem.m_dungeonRank.m_rankLevel[6] <= idx) return 6;
    if (m_rankSystem.m_dungeonRank.m_rankLevel[7] <= idx) return 7;
    return 8;
}