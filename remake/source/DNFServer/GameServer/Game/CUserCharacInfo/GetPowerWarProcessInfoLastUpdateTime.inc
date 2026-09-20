long CUserCharacInfo::GetPowerWarProcessInfoLastUpdateTime()
{
    return m_selected ? m_selected->m_powerWarData.m_data4 : 0;
}