std::vector<stResetItemInfo::ResetItem>* CDataManager::GetResetItemInfo() const
{
    return const_cast<std::vector<stResetItemInfo::ResetItem>*>(
        &m_limitItemUsage.m_resetC.m_list);
