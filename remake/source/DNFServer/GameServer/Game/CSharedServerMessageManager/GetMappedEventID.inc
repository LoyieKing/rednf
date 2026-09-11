// ----------------------------------------------------------------------------
// Adapter：索引/映射表查询
// ----------------------------------------------------------------------------
int CSharedServerMessageManager::CSharedServerMessageAdapter::GetMappedEventID(int nIndex)
{
    std::vector<std::pair<int, int> >::iterator it = m_pInfo->m_vecMapped.begin();
    while (it != m_pInfo->m_vecMapped.end())
    {
        std::pair<int, int>* pPair = &(*it);
        if (pPair == 0)
        {
            it++;
            continue;
        }
        if (pPair->first == nIndex)
            return pPair->second;
        it++;
    }
    return -1;
}