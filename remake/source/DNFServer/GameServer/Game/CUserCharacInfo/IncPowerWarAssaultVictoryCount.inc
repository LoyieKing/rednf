void CUserCharacInfo::IncPowerWarAssaultVictoryCount()
{
    if (m_selected)
    {
    enableSaveCharacStat();
    ++m_selected->m_powerWarAssaultVictoryCount;
    }
}