int Dispatcher_HellPartyStart::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x821d97a：无 user 判空，state!=5 时返回 LineFunc 结果。
    if (user->get_state() != 5)
    {
        return LineFunc(0xd1bd,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
