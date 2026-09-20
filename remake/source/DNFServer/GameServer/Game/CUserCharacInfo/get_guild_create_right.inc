unsigned char CUserCharacInfo::get_guild_create_right()
{
    return m_selected ? m_selected->m_fielddc2 : 0;
}