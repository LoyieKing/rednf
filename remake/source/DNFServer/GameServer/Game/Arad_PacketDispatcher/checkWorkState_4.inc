// ORIG Dispatcher_AvartarRoulette::checkWorkState 0x0819ee6c W：
//   Secu Check(user,0x45) → err 透传；AvatarCoin(+0x658 ABI 镜像) 不可用 →
//   错误 0x16；GetRemainCapacity(INVEN 2, ITEM_TYPE_0)<=0 → 错误 4。
bool Dispatcher_AvartarRoulette::checkWorkState(CUser* user,
                                                ::arad::SigAvatarRoulette& sig)
{
    (void)sig;   // ORIG 不读 Sig 字段
    int secuErr = GlobalData::s_pSecuProtectionField->Check(
        user, (SECURITY_PROTCTION)0x45);
    if (secuErr != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24a, (unsigned char)secuErr);
        return false;
    }
    // AvatarCoin 位于 Inven 数组尾部固定偏移 +0x658【ABI 镜像】（同
    // Arad_InternalDispatcher_INTER.cpp kAvatarRoulette 段惯例）。
    // 注意：ORIG 0x0819eec9 `add $0x658,%eax`（char* 字节偏移、无具名 this 槽，
    //   技巧37）；0x0819eed6 `xor$1;test;je` = !boolcall 直出（技巧1）。
    if (!user->getCurCharacInvenR()->m_avatarCoin.IsUsable())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24a, 0x16);
        return false;
    }
    if (user->getCurCharacInvenR()->GetRemainCapacity(
            (INVEN_TYPE)2, Inven_Item::ITEM_TYPE_0) <= 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24a, 4);
        return false;
    }
    return true;
}