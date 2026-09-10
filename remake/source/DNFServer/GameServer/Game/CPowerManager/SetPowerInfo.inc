void CPowerManager::SetPowerInfo(char side, int point, int channelPoint)
{
    // ORIG 0x0847eb76：*this=side; +4=point; +8=channelPoint。
    m_winnerSide = side;
    m_side1Point = point;
    m_side2Point = channelPoint;
