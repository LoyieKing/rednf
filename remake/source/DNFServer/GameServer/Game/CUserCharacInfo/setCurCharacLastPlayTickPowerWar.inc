void CUserCharacInfo::setCurCharacLastPlayTickPowerWar(long tick)
{
    enableSaveCharacStat();
    CUR->m_lastPlayTickPowerWar = tick;
