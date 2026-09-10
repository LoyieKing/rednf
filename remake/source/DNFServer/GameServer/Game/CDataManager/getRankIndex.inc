unsigned short CDataManager::getRankIndex(int idx)
{
    if (m_rankSystem.m_dungeonRank.m_rankLevel[0] <= idx) return m_rankSystem.m_dungeonRank.m_rankLevel[0];
    if (m_rankSystem.m_dungeonRank.m_rankLevel[1] <= idx) return m_rankSystem.m_dungeonRank.m_rankLevel[1];
    if (m_rankSystem.m_dungeonRank.m_rankLevel[2] <= idx) return m_rankSystem.m_dungeonRank.m_rankLevel[2];
    if (m_rankSystem.m_dungeonRank.m_rankLevel[3] <= idx) return m_rankSystem.m_dungeonRank.m_rankLevel[3];
    if (m_rankSystem.m_dungeonRank.m_rankLevel[4] <= idx) return m_rankSystem.m_dungeonRank.m_rankLevel[4];
    if (m_rankSystem.m_dungeonRank.m_rankLevel[5] <= idx) return m_rankSystem.m_dungeonRank.m_rankLevel[5];
    if (m_rankSystem.m_dungeonRank.m_rankLevel[6] <= idx) return m_rankSystem.m_dungeonRank.m_rankLevel[6];
    if (m_rankSystem.m_dungeonRank.m_rankLevel[7] <= idx) return m_rankSystem.m_dungeonRank.m_rankLevel[7];
    return 0;
