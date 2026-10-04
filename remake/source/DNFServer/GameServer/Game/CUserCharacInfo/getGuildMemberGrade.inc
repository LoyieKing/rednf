unsigned char CUserCharacInfo::getGuildMemberGrade()
{
    return m_selected ? m_selected->m_guildMemberGrade : 0;
}