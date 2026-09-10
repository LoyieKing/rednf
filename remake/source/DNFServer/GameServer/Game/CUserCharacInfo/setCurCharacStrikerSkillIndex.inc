void CUserCharacInfo::setCurCharacStrikerSkillIndex(unsigned char index)
{
    if (m_selected)
    {
    enableSaveCharacManageInfo();
    CUR->m_strikerSkillIndex = index;
    }
