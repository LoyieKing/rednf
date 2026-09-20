int CUserCharacInfo::get_charac_level_before_dungeon() const
{
    if (m_selected)
    {
        if (m_selected->m_fieldDFC != 0)
            return m_selected->m_fieldDFC;
        return m_selected->m_level;
    }
    return 0;
}