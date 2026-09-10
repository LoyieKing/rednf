int CPowerManager::GetRankingByCharacNo(unsigned int charNo)
{
    // [b28d-solo] P310：int 局部 + 显式 unsigned 比较复现两段寄存器尾测。
    for (int i = 0; (unsigned int)i <= 2; ++i)
        if (m_rankings[i].m_characNo == charNo)
            return (int)i + 1;
    return 0;
