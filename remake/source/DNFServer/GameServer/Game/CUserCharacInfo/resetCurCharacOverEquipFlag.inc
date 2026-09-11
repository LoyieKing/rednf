void CUserCharacInfo::resetCurCharacOverEquipFlag()
{
    if (!m_selected)
        return;
    CUR->m_overEquipFlag = 0;
}