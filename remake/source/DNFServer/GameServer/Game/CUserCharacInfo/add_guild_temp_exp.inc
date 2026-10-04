void CUserCharacInfo::add_guild_temp_exp(int exp)
{
    if (m_selected && m_selected->m_guildId != 0)
        m_selected->m_guildTempExp = (unsigned short)(m_selected->m_guildTempExp + (short)exp);
}