int CPowerManager::GetPowerWarPoint(ENUM_POWER_SIDE_TYPE side)
{
    return (int)side == 1 ? m_side1Point : m_side2Point;
}