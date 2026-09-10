void CUserCharacInfo::resetTempGuildNameAddr()
{
    if (m_selected)
    {
    memset(CUR->m_guildName, 0, sizeof(CUR->m_guildName));
    memset(CUR->m_guildAddr, 0, sizeof(CUR->m_guildAddr));
    }
