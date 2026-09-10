void CUserCharacInfo::DecreasePowerWarPoint(short point)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    // P254 packed enum 视角（同 IncreasePowerWarPoint）。
    CUR->m_powerWarPointEnum =
        (EPowerWarPointValue)((int)CUR->m_powerWarPointEnum - point);
    if ((short)CUR->m_powerWarPoint < 0)
        CUR->m_powerWarPoint = 0;
    }
