int CInventory::UseUpCoin(unsigned int amount)
{
    if (amount < m_coin)
    {
        m_coin -= amount;
        amount = 0;
    }
    else
    {
        amount -= m_coin;
        m_coin = 0;
    }
    if (amount < (unsigned int)m_eventCoin)
    {
        m_eventCoin -= amount;
        amount = 0;
    }
    else
    {
        amount -= m_eventCoin;
        m_eventCoin = 0;
    }
    if (amount < (unsigned int)m_payCoin)
    {
        m_payCoin -= amount;
        // ORIG 0x84faa63 +0x8f: if 体 amount = 0。
        amount = 0;
    }
    else
    {
        // ORIG 0x84faa6c +0x98: else 体 amount -= m_payCoin 后置 0。
        amount -= m_payCoin;
        m_payCoin = 0;
    }
    return m_coin + m_eventCoin + m_payCoin;
