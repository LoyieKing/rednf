void CUserCharacInfo::reset_level_before_dungeon()
{
    if (m_selected)
    {
    m_selected->m_levelBeforeDungeon = 0;
    }
}