void CUserCharacInfo::IncPowerWarAssaultCount()
{
    if (m_selected)
    {
    enableSaveCharacStat();
    ++CUR->m_powerWarAssaultCount;
    }
