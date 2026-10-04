void CUserCharacInfo::resetCharGuildInfo()
{
    if (m_selected)
    {
    m_selected->m_guildId = 0;
    m_selected->m_guildTempExp = 0;
    m_selected->m_guildExp = 0;
    m_selected->m_guildMemberGrade = 0;
    m_selected->m_guildCreateRight = 0;
    m_selected->m_guildCreateFlag = 0;
    m_selected->m_guildTodayExp = 0;
    }
}