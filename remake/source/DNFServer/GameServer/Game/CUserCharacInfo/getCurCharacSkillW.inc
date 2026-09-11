char* CUserCharacInfo::getCurCharacSkillW()
{
    enableSaveSkill();
    return (char*)CUR->m_skill;
}