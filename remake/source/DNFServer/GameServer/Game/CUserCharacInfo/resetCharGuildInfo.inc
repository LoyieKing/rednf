void CUserCharacInfo::resetCharGuildInfo()
{
    if (m_selected)
    {
    CUR->m_guildkey = 0;
    CUR->m_guildTempExp = 0;
    CUR->m_guildExp = 0;
    CUR->m_guildMemberGrade = 0;
    CUR->m_guildCreateRight = 0;
    CUR->m_guildCreateFlag = 0;
    CUR->m_guildTodayExp = 0;
    }
}