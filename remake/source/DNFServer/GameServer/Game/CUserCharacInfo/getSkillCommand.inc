const char* CUserCharacInfo::getSkillCommand() const
{
    return m_selected ? (const char*)CUR->m_skillCommand : 0;
}