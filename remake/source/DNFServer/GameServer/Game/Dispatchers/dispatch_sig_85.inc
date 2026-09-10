int Dispatcher_ConnectP2PAssault::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() <= 1)
    {
        return LineFunc(0xa1b0,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    char count = -1;                                       // -0xd（local_19[8]）
    if (!packet.get_byte(count))
    {
        return LineFunc(0xa1b4,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (count > 8 || count < 0)
    {
        return LineFunc(0xa1b8,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    char slots[8];                                         // -0x15..-0xe（base -0x15）
    slots[0] = 0; slots[1] = 0; slots[2] = 0; slots[3] = 0;
    slots[4] = 0; slots[5] = 0; slots[6] = 0; slots[7] = 0;
    char team = -1;                                        // -0x16（local_1a）
    for (int i = 0; i < (int)count; i++)
    {
        if (!packet.get_byte(team))
        {
            return LineFunc(0xa1bf,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (team > 7 || team < 0)
        {
            return LineFunc(0xa1c2,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!packet.get_byte(slots[(int)team]))
        {
            return LineFunc(0xa1c5,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
    }
    pvp_assault::CAssaultMgr* mgr = pvp_assault::GetInstanceAssaultMgr();   // 0x082350a0
    // X2 形（Main 批准铺池，OPMC8/我 probe px2 t8/t11 双实证）：绑 %eax 的具名
    // unsigned char + 显式 ^= 1 ⇒ 复现 ORIG `xor $0x1,%eax; testb %al,%al` 单段；
    // 写 `!= 1` 会被 4.4 折成 `cmpb $1` + setne 两段。
    unsigned char p2pBad =
        (unsigned char)mgr->OnConnectP2P(user, slots, (int)count);
    p2pBad ^= 1;
    if (p2pBad)
    {
        return LineFunc(0xa1cb,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
