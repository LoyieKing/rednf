void CUserCharacInfo::SetCurExpEquipSlotStat(char stat)
{
    if (m_selected)
    {
    m_selected->m_field123e = stat;
    }
}