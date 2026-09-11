void CUserCharacInfo::SetCurCharacMaxEquipLevel(int level)
{
    if (m_selected)
    {
    enableSaveCharacManageInfo();
    CUR->m_maxEquipLevel = level;
    }
}