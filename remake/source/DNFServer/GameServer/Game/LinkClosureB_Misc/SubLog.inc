// ---------------------------------------------------------------------------
// AvatarCoin::HistoryLog::SubLog —— ORIG T 0x0817ff54 (size 0x45)
// user!=0 → LogHistory("AvatarCoin-,%d,%d", amount, coin.Get())。
// coin 对象位于背包基址 +0x658（Arad_AvatarRoulette.cpp 同约定）。
// ---------------------------------------------------------------------------
void AvatarCoin::HistoryLog::SubLog(CUser* user, unsigned int amount)
{
    if (user == 0)
        return;
    // P0/P183：ORIG 无 `cur` 栈槽（Get() 结果 %eax 直传 LogHistory 出参槽）；
    // 具名局部致 spill/reload 2 条 + 帧 +0x10 → coin.Get() 内联为实参右值。
    user->LogHistory("AvatarCoin-,%d,%d", amount,
                     reinterpret_cast<AvatarCoin*>(
                         const_cast<char*>(reinterpret_cast<const char*>(
                             user->getCurCharacInvenRefR())) +
                         0x658)
                         ->Get());
}