int CDataManager::getClientChangeCreatureIndex(int idx)
{
    std::map<int, int>::iterator it = m_createCreatureGift.m_infinityChangeMap.find(idx);
    if (it != m_createCreatureGift.m_infinityChangeMap.end())
    {
        return it->second;
    }
    return 0xffffffff;
}