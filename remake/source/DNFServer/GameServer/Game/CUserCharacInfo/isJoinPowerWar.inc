unsigned char CUserCharacInfo::isJoinPowerWar() const
{
    return m_selected ? m_selected->m_powerWarData.m_data2 : 0;
}