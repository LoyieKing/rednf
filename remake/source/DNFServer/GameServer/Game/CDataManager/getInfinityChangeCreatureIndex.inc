int CDataManager::getInfinityChangeCreatureIndex(int idx)
{
    std::map<int, int>::iterator it = m_createCreatureGift.m_clientChangeMap.find(idx);
    if (it != m_createCreatureGift.m_clientChangeMap.end())
    {
        return it->second;
    }
    return 0xffffffff;
}