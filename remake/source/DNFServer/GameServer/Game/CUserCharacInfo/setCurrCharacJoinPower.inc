void CUserCharacInfo::setCurrCharacJoinPower(char power)
{
    enableSaveCharacStat();
    if (m_selected)
        m_selected->m_powerWarData.m_data2 = (unsigned char)power;
}