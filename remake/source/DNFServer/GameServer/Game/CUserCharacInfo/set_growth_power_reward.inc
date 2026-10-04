void CUserCharacInfo::set_growth_power_reward(int reward)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    m_selected->m_growthPowerReward = (unsigned char)reward;
    }
}