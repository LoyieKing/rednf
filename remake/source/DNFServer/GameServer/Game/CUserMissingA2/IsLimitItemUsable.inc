bool CUser::IsLimitItemUsable(int itemIdx)
{
    for (std::vector<LimitItemUsage>::iterator it = m_limitItemUsage.begin(),
         e = m_limitItemUsage.end(); it != e; it++)
    {
        if (it->m_field15 == itemIdx)
        {
            if (it->isUsable())
                return 1;
            return 0;
        }
    }
    return 1;
