void CUserCharacInfo::SetPowerWarProcessInfoLastUpdateTime(long t)
{
    if (m_selected)
    {
    m_selected->m_powerWarData.m_data4 = t;
    }
}