void CUserCharacInfo::setCurCharacStrikerSkillIndex(unsigned char index)
{
    if (m_selected)
    {
    enableSaveCharacManageInfo();
    m_selected->m_field14b7 = index;
    }
}