void CUserCharacInfo::reset_guild_exp()
{
    if (m_selected && CUR->m_guildkey != 0)
        CUR->m_guildExp = 0;
