void CUserCharacInfo::setCurCharacLastPlayTickPowerWar(long tick)
{
    enableSaveCharacStat();
    m_selected->m_field7f = tick;
}