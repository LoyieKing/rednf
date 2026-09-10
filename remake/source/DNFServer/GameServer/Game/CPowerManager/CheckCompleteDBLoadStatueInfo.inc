bool CPowerManager::CheckCompleteDBLoadStatueInfo()
{
    // [b28d-solo] P310：int 局部 + 显式 unsigned 比较复现 ORIG 0x0847f1bf 尾测
    // 两段物化（`mov -0x4(%ebp),%eax; cmp $0x2,%eax; setbe; test; jne`）。
    for (int i = 0; (unsigned int)i <= 2; ++i)
        if (m_rankings[i].m_flag != 1)
            return 0;
    return 1;
