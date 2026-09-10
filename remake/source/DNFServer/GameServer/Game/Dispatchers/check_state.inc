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
