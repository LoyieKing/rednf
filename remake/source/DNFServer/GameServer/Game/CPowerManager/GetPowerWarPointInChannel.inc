int CPowerManager::GetPowerWarPointInChannel(char side)
{
    return side == 1 ? m_side1ChannelPoint : m_side2ChannelPoint;
