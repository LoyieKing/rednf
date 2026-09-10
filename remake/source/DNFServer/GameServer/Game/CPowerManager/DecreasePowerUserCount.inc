void CPowerManager::DecreasePowerUserCount(char side)
{
    if (side == 1 || side == 3)
    {
        --m_side1UserCount;
        if (m_side1UserCount < 0)
            m_side1UserCount = 0;
    }
    else
    {
        --m_side2UserCount;
        if (m_side2UserCount < 0)
            m_side2UserCount = 0;
    }
