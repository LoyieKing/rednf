unsigned char CUserCharacInfo::get_growth_power_reward() const
{
    return m_selected ? CUR->m_growthPowerReward : 0;
