bool UserMercenaryInfoMgr::CheckValidAppend(const CHARAC_LOAD_MERCENARY& info)
{
    size_t i = 0;
    while (m_characs.size() > i)
    {
        if (info.m_characNo == m_characs[i].m_characNo)
        {
            if (m_characs[i].m_level != info.m_level)
            {
                m_characs[i].m_level = info.m_level;
                m_characs[i].alter();
                return 0;
            }
            return 0;
        }
        ++i;
    }
    return 1;
