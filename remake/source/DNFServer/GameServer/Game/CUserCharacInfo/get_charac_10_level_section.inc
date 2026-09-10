int CUserCharacInfo::get_charac_10_level_section() const
{
    if (m_selected)
    {
        if (CUR->m_level % 10 == 0)
            return CUR->m_level;
        else
            return CUR->m_level - (CUR->m_level % 10) + 10;
    }
    return 10;
