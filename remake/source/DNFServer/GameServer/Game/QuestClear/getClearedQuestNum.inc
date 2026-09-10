int CQuestClear::getClearedQuestNum() const
{
    // ORIG 0x0808bbc4：统计 m_cleared[1..0x752f] 非零数；有符号比较 setle（P2：i <= 0x752f）。
    int count = 0;
    for (int i = 1; i <= 0x752f; ++i)
    {
        if (m_cleared[i].m_cleared != 0)
            ++count;
    }
    return count;
