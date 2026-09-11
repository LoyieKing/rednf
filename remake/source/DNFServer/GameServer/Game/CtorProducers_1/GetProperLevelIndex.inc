int CLevelRewardTable::GetProperLevelIndex(unsigned int level)
{
    // ORIG 0x086876c8：扫 levelKeys，遇 0 止返回 -1，命中返回下标。
    for (int i = 0; i <= 0x45; ++i)
    {
        if (m_levelKeys[i] == 0)
            break;
        if (m_levelKeys[i] == level)
            return i;
    }
    return -1;
}