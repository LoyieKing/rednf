int Dispatcher_Mercenary_Competition::read(PacketBuf& packet, MSG_BASE& msg)  // ORIG 0x081ddc24 (81 insn)
{
    MsgMercenaryCompetition& req = reinterpret_cast<MsgMercenaryCompetition&>(msg);
    static const char kFunc[] =
        "virtual int Dispatcher_Mercenary_Competition::read(PacketBuf&, MSG_BASE&)";
    // ORIG 单条 `add $0x10`：m_pad(成员+0xd)+3 会被 4.4 拆成两步 add；直接对基址加常数。
    // 成员视图（自然对齐、非 packed，可绑 int&）：经 req 绑定回读 -0xc + 单 add $0x10。
    struct LenView { char m_head[0x10]; int m_len; };
    if (!packet.get_int(((LenView*)&req)->m_len))                              // msg+0x10
        return LineFunc(0x4eda, kFunc, 0, 0);
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_pad[7])))              // msg+0x14
        return LineFunc(0x4edc, kFunc, 0, 0);
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_pad[8])))              // msg+0x15
        return LineFunc(0x4ede, kFunc, 0, 0);
    return 0;
