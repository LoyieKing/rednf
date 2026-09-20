void CUserCharacInfo::setGuildMemberGrade(unsigned char grade)
{
    if (m_selected)
    {
    m_selected->m_fieldDF3 = grade;
    }
}