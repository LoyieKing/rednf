int CPowerManager::GetPowerUserCount(char side)
{
    return side == 1 ? m_side1UserCount : m_side2UserCount;
}