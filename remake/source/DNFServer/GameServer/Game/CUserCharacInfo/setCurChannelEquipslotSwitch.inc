void CUserCharacInfo::setCurChannelEquipslotSwitch(char flag)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_curChannelEquipslotSwitch = (unsigned char)flag;
    }
