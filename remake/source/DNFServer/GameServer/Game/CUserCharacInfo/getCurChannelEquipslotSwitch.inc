char CUserCharacInfo::getCurChannelEquipslotSwitch()
{
    return m_selected ? (int)(unsigned char)m_selected->m_field1276 : -1;
}