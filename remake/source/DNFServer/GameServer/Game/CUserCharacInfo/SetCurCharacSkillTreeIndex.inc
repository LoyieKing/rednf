void CUserCharacInfo::SetCurCharacSkillTreeIndex(char index)
{
    if (m_selected)
    {
    enableSaveCharacInfo();
    CUR->m_skillTreeIndex = index;
    }
