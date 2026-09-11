// ===== Dispatcher_Enchant_3rdChronicleItem::check_state —— ORIG 0x082328ec =====
// ORIG：state!=3 或 getCurCharacR()==0 → LineFunc(0x5f6)；CheckInTrade() → LineFunc(0x5f9)；
// 通过 → 0（失败路径返回值即 LineFunc 返回）。
int Dispatcher_Enchant_3rdChronicleItem::check_state(CUser* user, PacketBuf& /*packet*/)
{
    if (user->get_state() != 3 || user->getCurCharacR() == 0)
    {
        return LineFunc(
            0x5f6,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    if (user->CheckInTrade())
    {
        return LineFunc(
            0x5f9,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    return 0;
}