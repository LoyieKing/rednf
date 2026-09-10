int BuyUpgradeData::getMinLevel() const
{
    // b28d-solo-advancealtar_misc：IDENTICAL/AE 复验通过（fresh .o compare_tu clean，
    //   49=49 逐条全等）。ORIG @0x088a33c0。
    // b28d-35: IDENTICAL_AE（49/49 clean 全等）。ORIG @0x088a33c0；同 getMaxLevel。
    // b28c-L5：同 getMaxLevel —— ORIG ppEi 后置 ++ 三槽形（0x088a33c0）。
    int minLevel = 0;
    for (LevelFieldDataMap::const_iterator it = m_listMap.begin();
         it != m_listMap.end(); it++)
    {
        if (it->first < minLevel)
        {
            minLevel = it->first;
        }
    }
    return minLevel;
