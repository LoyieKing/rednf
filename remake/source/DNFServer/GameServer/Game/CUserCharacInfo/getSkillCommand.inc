const char* CUserCharacInfo::getSkillCommand() const
{
    return m_selected ? reinterpret_cast<const char*>(&m_selected->m_skillCommandsMap) : 0;
}
