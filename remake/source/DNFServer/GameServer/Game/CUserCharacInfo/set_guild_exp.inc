void CUserCharacInfo::set_guild_exp(int exp)
{
    if (m_selected && CUR->m_guildkey != 0)
        CUR->m_guildExp = exp;
