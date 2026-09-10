void BlueMarbleUserInfo::setBoardZone(int zone, int pos)
{
    if (m_boardZone + zone >= pos)
    {
        m_boardZone = pos;
    }
    else if (m_boardZone + zone <= 1)
    {
        m_boardZone = 1;
    }
    else
    {
        m_boardZone = m_boardZone + zone;
    }
