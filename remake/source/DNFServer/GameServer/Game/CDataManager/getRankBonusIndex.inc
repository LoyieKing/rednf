int CDataManager::getRankBonusIndex(int idx) const
{
    if (m_rankSystem.m_dungeonRank.m_rankLevel[0] <= idx) return 4;
    if (m_rankSystem.m_dungeonRank.m_rankLevel[1] <= idx) return 3;
    if (m_rankSystem.m_dungeonRank.m_rankLevel[2] <= idx) return 2;
    if (m_rankSystem.m_dungeonRank.m_rankLevel[3] <= idx) return 1;
    if (m_rankSystem.m_dungeonRank.m_rankLevel[4] <= idx) return 0;
    return -1;
}