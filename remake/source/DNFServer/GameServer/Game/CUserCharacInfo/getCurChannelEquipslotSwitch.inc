char CUserCharacInfo::getCurChannelEquipslotSwitch()
{
    return m_selected ? (int)(unsigned char)m_selected->m_channelEquipslotSwitch : -1;
}