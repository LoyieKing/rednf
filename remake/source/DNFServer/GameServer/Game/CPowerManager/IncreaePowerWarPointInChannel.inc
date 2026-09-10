void CPowerManager::IncreaePowerWarPointInChannel(char side, int point)
{
    if (side == 1)
        m_side1ChannelPoint += point;
    else
        m_side2ChannelPoint += point;
