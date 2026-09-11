void CUserCharacInfo::set_level_before_dungeon(short level)
{
    if (m_selected)
    {
    CUR->m_levelBeforeDungeon = level;
    }
}