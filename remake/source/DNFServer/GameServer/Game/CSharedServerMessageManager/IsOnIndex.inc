bool CSharedServerMessageManager::CSharedServerMessageAdapter::IsOnIndex(int nIndex)
{
    std::vector<int>::iterator it = m_pInfo->m_vecIndex.begin();
    while (it != m_pInfo->m_vecIndex.end())
    {
        int nVal = *it;
        if (nVal == nIndex)
            return true;
        it++;
    }
    return false;
}