long CUserCharacInfo::GetPowerWarProcessInfoLastUpdateTime()
{
    return m_selected ? CUR->m_powerWarProcessLastUpdateTime : 0;
