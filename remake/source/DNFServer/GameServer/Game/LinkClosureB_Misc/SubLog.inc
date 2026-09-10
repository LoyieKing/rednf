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
