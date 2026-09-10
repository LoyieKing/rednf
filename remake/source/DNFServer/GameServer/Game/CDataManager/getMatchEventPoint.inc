int CDataManager::getMatchEventPoint(int idx)
{
    std::map<int, int>::iterator it = m_seriaRoomDeco.m_matchEventMap.find(idx);
    if (it != m_seriaRoomDeco.m_matchEventMap.end())
    {
        return it->second;
    }
    return 0;
