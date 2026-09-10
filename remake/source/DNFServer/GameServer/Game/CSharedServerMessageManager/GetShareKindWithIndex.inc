int CSharedServerMessageManager::CSharedServerMessageAdapter::GetShareKindWithIndex(int nIndex)
{
    int nMapped = GetMappedEventID(nIndex);
    if (nMapped == -1)
        return -1;

    std::vector<std::pair<int, int> >::iterator it = m_pInfo->m_vecShareKind.begin();
    while (it != m_pInfo->m_vecShareKind.end())
    {
        std::pair<int, int>* pPair = &(*it);
        if (pPair == 0)
        {
            it++;
            continue;
        }
        if (pPair->first == nMapped)
            return pPair->second;
        it++;
    }
    return -1;
