bool CWorldMap::hasDungeon(unsigned int dungeonIdx) const
{
    return std::find(m_dungeonList.begin(), m_dungeonList.end(), dungeonIdx) !=
           m_dungeonList.end();
