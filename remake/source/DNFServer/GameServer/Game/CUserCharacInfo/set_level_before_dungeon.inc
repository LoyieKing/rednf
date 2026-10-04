void CUserCharacInfo::set_level_before_dungeon(short level)
{
    if (m_selected)
    {
    m_selected->m_levelBeforeDungeon = level;
    }
}