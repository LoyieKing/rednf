int CCoinEventPerDay::GetCoinNoPerDay(int level)
{
    if (level < 0x12)
        return m_coin01;
    if (level < 0x1b)
        return m_coin02;
    return m_coin03;
