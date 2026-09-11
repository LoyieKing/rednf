void CUserCharacInfo::ResetPowerWarAssaultRecord()
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_powerWarAssaultCount = 0;
    CUR->m_powerWarAssaultVictoryCount = 0;
    }
}