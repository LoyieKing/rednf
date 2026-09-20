void CUserCharacInfo::SetCurCharacMaxEquipLevel(int level)
{
    if (m_selected)
    {
    enableSaveCharacManageInfo();
    m_selected->m_field14C2 = level;
    }
}