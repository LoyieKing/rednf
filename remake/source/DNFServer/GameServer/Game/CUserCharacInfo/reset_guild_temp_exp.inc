void CUserCharacInfo::reset_guild_temp_exp()
{
    if (m_selected)
    {
    m_selected->m_guildTempExp = 0;
    }
}