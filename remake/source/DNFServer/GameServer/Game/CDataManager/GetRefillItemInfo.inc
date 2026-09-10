std::vector<stRefillItemInfo::RefillItem>* CDataManager::GetRefillItemInfo() const
{
    return const_cast<std::vector<stRefillItemInfo::RefillItem>*>(
        &m_limitItemUsage.m_refill18.m_list);
