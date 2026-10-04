void CUserCharacInfo::setGuildMemberGrade(unsigned char grade)
{
    if (m_selected)
    {
    m_selected->m_guildMemberGrade = grade;
    }
}