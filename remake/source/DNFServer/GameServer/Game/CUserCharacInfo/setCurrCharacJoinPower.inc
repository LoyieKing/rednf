void CUserCharacInfo::setCurrCharacJoinPower(char power)
{
    enableSaveCharacStat();
    if (m_selected)
        CUR->m_joinPower = (unsigned char)power;
