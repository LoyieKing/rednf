char CUserCharacInfo::getCurChannelEquipslotSwitch()
{
    return m_selected ? (int)(unsigned char)CUR->m_curChannelEquipslotSwitch : -1;
}