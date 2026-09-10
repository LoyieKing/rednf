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
    // [G1Csig] ORIG 0x08206cfa（size 0x216）为**单条** `movl $0x0,-0x11(%ebp)`（4 字节一次清）；
    // 我方 4 句独立赋值产 4 条 `movb $0x0,-0x19/-0x18/-0x17/-0x16` ⇒ 改聚合初始化。
    char slots[4] = {0, 0, 0, 0};                          // -0x11..-0xe（base -0x11）
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
    // [G1Csig 实测] 具名 mgr 与具名 unsigned char p2pBad 各占 1 槽（我方 -0xd 为本函数
    // 独有槽、count/team/slots 全体下移 1~2 字节，diff our[111:114] `mov %al,-0xd;
    // xorb $1,-0xd; cmpb $0,-0xd`）；ORIG 为 `call; xor $1,%eax; test %al,%al` 纯寄存器
    // ⇒ 去两个具名：GetInstanceAssaultMgr() 链式直调 + `!call` 内联取反（P0/P226）。
    if (!pvp_assault::GetInstanceAssaultMgr()   // 0x082350a0
             ->OnConnectP2PAfterAssault(user, slots, (int)count))
    {
        return LineFunc(0xa18f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
