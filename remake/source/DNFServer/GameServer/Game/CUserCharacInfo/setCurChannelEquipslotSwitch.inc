void CUserCharacInfo::setCurChannelEquipslotSwitch(char flag)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    m_selected->m_channelEquipslotSwitch = (unsigned char)flag;
    }
}