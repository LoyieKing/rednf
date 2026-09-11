// [b28d-solo-CPowerManager] IDENTICAL(28=28, ndiff 0) | ORIG 0x0847eba0 | 本轮修复：
// P310 翻案（旧 N18b/P209附 判词作废）——int 局部 + 显式 `(unsigned int)i <= 2`
// 复现 ORIG 0x0847ebd8 尾测 `mov slot,%eax; cmp $2,%eax; setbe; test; jne`。
int CPowerManager::GetRankingByCharacNo(unsigned int charNo)
{
    // [b28d-solo] P310：int 局部 + 显式 unsigned 比较复现两段寄存器尾测。
    for (int i = 0; (unsigned int)i <= 2; ++i)
        if (m_rankings[i].m_characNo == charNo)
            return (int)i + 1;
    return 0;
}