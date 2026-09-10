bool CMonsterDrop::_getBossGoldDrop(unsigned char type, unsigned short& outGold,
                                    unsigned short& outCount)
{
    if (type < 4)
    {
        int tier = 0;
        // ORIG：value 先物化 %ebx 跨 generateNumber 保活，实参和式
        // value + rate 各自独立重读（无局部）。
        if (static_cast<int>(m_conditionRateW[type][1]) >
            m_random.generateNumber(
                0, static_cast<int>(m_conditionRateW[type][1]) +
                       static_cast<int>(m_conditionRate[type].rate)))
            tier = 1;                                  // setg：value > roll → 高档

        outGold = static_cast<unsigned short>(
            m_random.generateNumber(0, m_goldQuantityS[tier].m_hi -
                                           m_goldQuantityS[tier].m_lo) +
            m_goldQuantityS[tier].m_lo);
        outCount = m_goldVolume[tier];                 // (tier+0x12c9d0)*2 → 步长2
        return true;
    }
    return false;
