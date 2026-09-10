unsigned char CUserCharacInfo::GetCurExEquipSlotStat()
{
    return m_selected ? (unsigned char)CUR->m_curExEquipSlotStat : 0;
