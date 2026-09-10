int CDataManager::getSeriaRoomAniDecoItem(int idx)
{
    if (m_seriaAniDeco.m_itemMap.empty())
    {
        return 0;
    }
    ++idx;
    std::map<int, int>::iterator it = m_seriaAniDeco.m_itemMap.find(idx);
    if (it != m_seriaAniDeco.m_itemMap.end())
    {
        return it->second;
    }
    return 0;
