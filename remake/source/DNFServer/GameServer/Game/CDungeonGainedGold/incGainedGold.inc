unsigned int CDungeonGainedGold::incGainedGold(unsigned int amount)
{
    // ORIG 语义：返回「跨越的 10 万分位差」（newGold/100000 - oldGold/100000），
    // 并累计 m_gainedGold。
    unsigned int inc = (m_gainedGold + amount) / 100000 - m_gainedGold / 100000;
    m_gainedGold = m_gainedGold + amount;
    if (inc != 0)
        return inc;
    return 0;
