unsigned char CUserCharacInfo::get_guild_create_right()
{
    return m_selected ? CUR->m_guildCreateRight : 0;
