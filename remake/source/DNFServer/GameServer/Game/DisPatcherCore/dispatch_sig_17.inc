int DisPatcher_UseStack::dispatch_sig(CUser* user, PacketBuf& packet)
{
    short slotNo = 0;                            // ORIG local_6e（get_short(short&)/movswl 实证）
    if (!packet.get_short(slotNo))
        return LineFunc(0x8315,
                        __PRETTY_FUNCTION__,
                        0, 0);
    unsigned char itemSpace = 0;                 // ORIG local_7c[0]
    if (!packet.get_byte(itemSpace))
        return LineFunc(0x8318,
                        __PRETTY_FUNCTION__,
                        0, 0);
    int itemAddInfo = 0;                         // ORIG local_80
    int param2 = 0;                              // ORIG local_84
    if (!packet.get_int(itemAddInfo))
        return LineFunc(0x831d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(param2))
        return LineFunc(0x831e,
                        __PRETTY_FUNCTION__,
                        0, 0);

    bool stateOk =
        (user->get_state() == 5) || (user->get_state() == 7) ||
        (user->get_state() == 8) || (user->get_state() == 3) ||
        (user->get_state() == 6) || (user->get_state() == 10) ||
        (user->get_state() == 12);
    // ---- 状态拒绝包形态：header(1,0x2f)/byte0/byte(code)/byte(space)/
    //      int(itemAddInfo)/int(param2)/finalize(true) ----
