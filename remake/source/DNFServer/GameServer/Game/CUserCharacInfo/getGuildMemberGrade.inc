unsigned char CUserCharacInfo::getGuildMemberGrade()
{
    return m_selected ? CUR->m_guildMemberGrade : 0;
}