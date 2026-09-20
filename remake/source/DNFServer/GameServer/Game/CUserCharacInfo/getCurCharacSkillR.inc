const char* CUserCharacInfo::getCurCharacSkillR() const
{
    return reinterpret_cast<const char*>(&m_selected->m_skillSlot);
}
