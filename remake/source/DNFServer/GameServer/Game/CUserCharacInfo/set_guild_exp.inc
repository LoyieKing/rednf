void CUserCharacInfo::set_guild_exp(int exp)
{
    if (m_selected && m_selected->m_guildId != 0)
        m_selected->m_fieldDE9 = exp;
}