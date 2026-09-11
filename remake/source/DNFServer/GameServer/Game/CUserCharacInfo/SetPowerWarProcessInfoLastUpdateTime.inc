void CUserCharacInfo::SetPowerWarProcessInfoLastUpdateTime(long t)
{
    if (m_selected)
    {
    CUR->m_powerWarProcessLastUpdateTime = t;
    }
}