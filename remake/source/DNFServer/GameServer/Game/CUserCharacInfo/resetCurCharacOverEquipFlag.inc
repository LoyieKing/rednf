void CUserCharacInfo::resetCurCharacOverEquipFlag()
{
    if (!m_selected)
        return;
    m_selected->m_needCheckOverEquipItem = 0;
}