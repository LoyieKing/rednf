int CUserCharacInfo::GetCurCharacMaxEquipLevel() const
{
    return m_selected ? m_selected->m_maxEquipLevel : 0;
}