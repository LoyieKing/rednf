void CUserCharacInfo::ResetPowerWarAssaultRecord()
{
    if (m_selected)
    {
    enableSaveCharacStat();
    m_selected->m_powerWarAssaultCount = 0;
    m_selected->m_powerWarAssaultVictoryCount = 0;
    }
}