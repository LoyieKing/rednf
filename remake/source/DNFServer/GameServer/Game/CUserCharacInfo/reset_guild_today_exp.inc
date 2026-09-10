void CUserCharacInfo::reset_guild_today_exp()
{
    if (m_selected && CUR->m_guildkey != 0)
        CUR->m_guildTodayExp = 0;
