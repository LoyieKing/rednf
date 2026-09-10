int Inter_UpgradeAccountCargo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_UpgradeAccountCargoPktView* pPkt = reinterpret_cast<Inter_UpgradeAccountCargoPktView*>(data);
    if (user->get_state() <= 2)
        return 0;
    if (pPkt->m_0 != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x133,
                                 pPkt->m_0);
        return 0;
    }
    if ((unsigned char)user->IsExistAccountCargo() != 1)
        return 0;
    // ORIG：GetAccountCargo 结果物化槽 -0xc(%ebp)（pPkt 因此被挤到 -0x10），
    // 之后 pPkt->m_4 装载 → cargo 槽回读 → SetCapacity。链式调用形反产
    // 4.4 %ebx 缓存 + push ebx 帧。
    CAccountCargo* pCargo = user->GetAccountCargo();
    pCargo->SetCapacity(pPkt->m_4);
    user->SendCmdOkPacket((ENUM_CMDPACKET)0x133);
    return 0;
