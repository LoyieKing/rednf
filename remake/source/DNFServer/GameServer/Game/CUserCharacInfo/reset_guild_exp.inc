void CUserCharacInfo::reset_guild_exp()
{
    if (m_selected && m_selected->m_guildId != 0)
        m_selected->m_guildExp = 0;
}