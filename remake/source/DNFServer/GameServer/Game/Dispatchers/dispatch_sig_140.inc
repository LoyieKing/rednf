int Dispatcher_ConnectP2PAfterAssault::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() <= 1)
    {
        return LineFunc(0xa16f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    char count = -1;                                       // -0xd（local_15[4]）
    if (!packet.get_byte(count))
    {
        return LineFunc(0xa173,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (count > 4 || count < 0)
    {
        return LineFunc(0xa177,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    char slots[4];                                         // -0x11..-0xe（base -0x11）
    slots[0] = 0; slots[1] = 0; slots[2] = 0; slots[3] = 0;
    char team = -1;                                        // -0x12（local_16）
    for (int i = 0; i < (int)count; i++)
    {
        if (!packet.get_byte(team))
        {
            return LineFunc(0xa17e,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (team > 3 || team < 0)
        {
            return LineFunc(0xa182,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!packet.get_byte(slots[(int)team]))
        {
            return LineFunc(0xa189,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
    }
    pvp_assault::CAssaultMgr* mgr = pvp_assault::GetInstanceAssaultMgr();   // 0x082350a0
    // X2 形（同 Dispatcher_ConnectP2PAssault::dispatch_sig，Main 批准铺池）：
    // 绑 %eax 具名 unsigned char + ^= 1 ⇒ ORIG 单段 `xor $1,%eax; testb %al,%al`。
    unsigned char p2pBad =
        (unsigned char)mgr->OnConnectP2PAfterAssault(user, slots, (int)count);
    p2pBad ^= 1;
    if (p2pBad)
    {
        return LineFunc(0xa18f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
