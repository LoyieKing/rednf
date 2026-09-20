unsigned char CUserCharacInfo::getGuildMemberGrade()
{
    return m_selected ? m_selected->m_fieldDF3 : 0;
}