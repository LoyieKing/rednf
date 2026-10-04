// ---- AvatarCoin::HistoryLog::AddLog ORIG @ 0x817ff9c ----
void AvatarCoin::HistoryLog::AddLog(CUser* user, unsigned int amount)
{
    if (user == 0)
        return;
    // P0：ORIG 内联 `call getCurCharacInvenRefR; add $0x658; call AvatarCoin::Get`
    // （无 coinOf 调用、无 coin 局部槽），同 GetPackage L129 内联形。
    user->LogHistory("AvatarCoin+,%d,%d", amount,
        user->getCurCharacInvenRefR()->m_avatarCoin.Get());
}