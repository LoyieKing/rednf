char* CUserCharacInfo::getCurCharacSkillW()
{
    enableSaveSkill();
    return reinterpret_cast<char*>(&m_selected->m_skillSlot);
}
