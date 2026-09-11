// ===== Dispatcher_Select_3rdChronicleItem_ForEnchant::check_state —— ORIG 0x08232854 =====
// ORIG：state!=3 或 getCurCharacR()==0 → LineFunc(0x5e1)；CheckInTrade() → LineFunc(0x5e4)；
// 通过 → 0（失败路径返回值即 LineFunc 返回）。
int Dispatcher_Select_3rdChronicleItem_ForEnchant::check_state(
    CUser* user, PacketBuf& /*packet*/)
{
    if (user->get_state() != 3 || user->getCurCharacR() == 0)
    {
        return LineFunc(
            0x5e1,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    if (user->CheckInTrade())
    {
        return LineFunc(
            0x5e4,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    return 0;
}