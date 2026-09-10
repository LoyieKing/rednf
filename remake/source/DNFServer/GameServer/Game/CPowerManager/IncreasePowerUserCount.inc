void CPowerManager::IncreasePowerUserCount(char side)
{
    if (side == 1 || side == 3)
        ++m_side1UserCount;
    else
        ++m_side2UserCount;
