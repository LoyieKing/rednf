int CUserCharacInfo::get_charac_level_before_dungeon() const
{
    if (m_selected)
    {
        if (CUR->m_levelBeforeDungeon != 0)
            return CUR->m_levelBeforeDungeon;
        return CUR->m_level;
    }
    return 0;
}