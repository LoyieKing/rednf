int Dispatcher_Change_Charac_Slot::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x082251f4：state!=2 直接返 0；连读两个 uint（失败任一 → LineFunc(0xdff5)）；
    // 锁链：isLocked4DataLoad||isLockedCreateCharac 再 ||isLockedDeleteCharac，
    // 全未锁时 ChangeCharacSlot(第一个 uint, 第二个 uint)，返 0。
    if (user->get_state() != 2)
    {
        return 0;
    }
    unsigned int first = 0;
    unsigned int second = 0;
    char readOk;
    if (packet.get_int(first) && packet.get_int(second))
    {
        readOk = 0;
    }
    else
    {
        readOk = 1;
    }
    if (readOk != 0)
    {
        return LineFunc(0xdff5,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    char locked;
    if (user->isLocked4DataLoad() == 0 && user->isLockedCreateCharac() == 0)
    {
        locked = 0;
    }
    else
    {
        locked = 1;
    }
    if (locked == 0 && user->isLockedDeleteCharac() == 0)
    {
        locked = 0;
    }
    else
    {
        locked = 1;
    }
    if (locked != 1)
    {
        user->ChangeCharacSlot(first, second);
    }
    return 0;
