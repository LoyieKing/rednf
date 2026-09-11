void CUserCharacInfo::IncPowerWarAssaultVictoryCount()
{
    if (m_selected)
    {
    enableSaveCharacStat();
    ++CUR->m_powerWarAssaultVictoryCount;
    }
}