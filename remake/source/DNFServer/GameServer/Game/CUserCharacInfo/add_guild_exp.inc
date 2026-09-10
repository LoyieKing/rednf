void CUserCharacInfo::add_guild_exp(int exp)
{
    if (m_selected && CUR->m_guildkey != 0)
        // P274 异型视角（同 add_guild_today_exp）。
        CUR->m_guildExpU = CUR->m_guildExpU + exp;
