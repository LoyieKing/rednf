void CReliablePerson::IncrementTradeGold(unsigned int gold)
{
    m_dailyTradeGold += gold;
    m_totalTradeGold += gold;
    if (m_totalTradeGold > 999999999)
    {
        unsigned int total = m_totalTradeGold;
        // 累计交易额超过 10 亿：十亿位进位到 billion，total 保留余数
        m_totalTradeGold = total - total / 1000000000 * 1000000000;
        m_billionTradeGold += total / 1000000000;
    }
}