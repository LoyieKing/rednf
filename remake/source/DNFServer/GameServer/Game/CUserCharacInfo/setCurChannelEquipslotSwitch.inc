void CUserCharacInfo::setCurChannelEquipslotSwitch(char flag)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    m_selected->m_field1276 = (unsigned char)flag;
    }
}