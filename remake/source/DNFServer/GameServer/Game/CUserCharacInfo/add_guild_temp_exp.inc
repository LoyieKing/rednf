void CUserCharacInfo::add_guild_temp_exp(int exp)
{
    if (m_selected && CUR->m_guildkey != 0)
        CUR->m_guildTempExp = (unsigned short)(CUR->m_guildTempExp + (short)exp);
