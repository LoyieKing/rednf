unsigned char CUserCharacInfo::GetCurExEquipSlotStat()
{
    return m_selected ? (unsigned char)m_selected->m_exEquipSlotStat : 0;
}