// ORIG T 0x088a33c0：初值 0，取 m_listMap 最小 key。
// [P8S3 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
int BuyUpgradeData::getMinLevel() const
{
    // b28d-solo-advancealtar_misc：IDENTICAL/AE 复验通过（fresh .o compare_tu clean，
    //   49=49 逐条全等）。ORIG @0x088a33c0。
    // b28d-35: IDENTICAL_AE（49/49 clean 全等）。ORIG @0x088a33c0；同 getMaxLevel。
    // b28c-L5：同 getMaxLevel —— ORIG ppEi 后置 ++ 三槽形（0x088a33c0）。
    // [W13C 2026-09-11 落地] 同 getMaxLevel 的符号轴（ORIG 用 operator* / deEv）。
    int minLevel = 0;
    for (LevelFieldDataMap::const_iterator it = m_listMap.begin();
         it != m_listMap.end(); it++)
    {
        if ((*it).first < minLevel)
        {
            minLevel = (*it).first;
        }
    }
    return minLevel;
}