void CUserCharacInfo::reset_guild_temp_exp()
{
    if (m_selected)
    {
    CUR->m_guildTempExp = 0;
    }
}