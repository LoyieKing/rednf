unsigned char CUserCharacInfo::isJoinPowerWar() const
{
    return m_selected ? CUR->m_joinPower : 0;
