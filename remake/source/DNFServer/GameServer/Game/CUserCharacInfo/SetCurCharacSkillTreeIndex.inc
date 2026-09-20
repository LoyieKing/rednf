void CUserCharacInfo::SetCurCharacSkillTreeIndex(char index)
{
    if (m_selected)
    {
    enableSaveCharacInfo();
    m_selected->m_field1239 = index;
    }
}