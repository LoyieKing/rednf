int CUserCharacInfo::get_charac_level_before_dungeon() const
{
    if (m_selected)
    {
        if (m_selected->m_levelBeforeDungeon != 0)
            return m_selected->m_levelBeforeDungeon;
        return m_selected->m_level;
    }
    return 0;
}