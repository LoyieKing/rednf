LimitItemUsage* CUser::GetItemLimitInfo(int itemIdx)
{
    for (std::vector<LimitItemUsage>::iterator it = m_limitItemUsage.begin(),
         e = m_limitItemUsage.end(); it != e; it++)
    {
        if (it->m_field15 == itemIdx)
            return &*it;
    }
    return 0;
