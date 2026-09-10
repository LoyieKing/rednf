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
    // ORIG [44:47]：`movl $0x0,-0x15(%ebp); movl $0x0,-0x11(%ebp)` —— 两条 4B 立即数
    //   覆盖 8 字节；逐字节 `slots[i]=0` 会被 4.4 折成 8 条 `movb $0x0`（+6 条）。
    *(unsigned int*)(slots + 0) = 0;
    *(unsigned int*)(slots + 4) = 0;
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
    // ORIG [109:111]：`xor $0x1,%eax; test %al,%al`（无 `movb %al,-0xd(%ebp)`/
    //   `xorb $1,-0xd(%ebp)` 槽往返）⇒ 裸 `!` 形（旧 X2 具名形多 1 槽 + 2 条）。
    if (!mgr->OnConnectP2P(user, slots, (int)count))
    {
        return LineFunc(0xa1cb,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
