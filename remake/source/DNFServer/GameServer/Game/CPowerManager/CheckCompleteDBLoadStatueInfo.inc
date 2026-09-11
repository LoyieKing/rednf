// [b28d-solo-CPowerManager] IDENTICAL(28=28, ndiff 0) | ORIG 0x0847f1b0 | 本轮修复：
// P310 翻案（旧 N18b/P209附 判词作废）——循环局部由 unsigned 改 int + 显式
// `(unsigned int)i <= 2` 比较，4.4 即发 ORIG 的两段寄存器尾测
// `mov slot,%eax; cmp $2,%eax; setbe; test %al,%al; jne`（unsigned 局部恒折
// `cmpl $2,slot` 单段）。同族三件（GetRankingByCharacNo/SetPowerWarUserRankingInfo）
// 同法一次归零。
bool CPowerManager::CheckCompleteDBLoadStatueInfo()
{
    // [b28d-solo] P310：int 局部 + 显式 unsigned 比较复现 ORIG 0x0847f1bf 尾测
    // 两段物化（`mov -0x4(%ebp),%eax; cmp $0x2,%eax; setbe; test; jne`）。
    for (int i = 0; (unsigned int)i <= 2; ++i)
        if (m_rankings[i].m_flag != 1)
            return 0;
    return 1;
}