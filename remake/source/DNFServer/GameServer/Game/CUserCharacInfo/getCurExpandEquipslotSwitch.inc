char CUserCharacInfo::getCurExpandEquipslotSwitch()
{
    return m_selected ? (int)(unsigned char)m_selected->m_field1277 : -1;
}