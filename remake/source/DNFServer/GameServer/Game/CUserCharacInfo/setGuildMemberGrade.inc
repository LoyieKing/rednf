void CUserCharacInfo::setGuildMemberGrade(unsigned char grade)
{
    if (m_selected)
    {
    CUR->m_guildMemberGrade = grade;
    }
}