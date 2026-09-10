bool CDataManager::invalidAniDecoItem(int idx, int value)
{
    ++idx;
    std::map<int, int>::iterator it = m_seriaAniDeco.m_itemMap.find(idx);
    if (it != m_seriaAniDeco.m_itemMap.end())
    {
        if (it->second == value)
        {
            return true;
        }
    }
    return false;
