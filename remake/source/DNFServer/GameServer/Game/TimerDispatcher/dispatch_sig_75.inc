int TimerLightServerNotice::dispatch_sig(int /*p1*/, int p2, unsigned int /*p3*/)
{
    // ORIG 0x08636a7c：越界检查（idx 负或 >= size 直接 false）→ 构造 0xc 包
    // {byte 0, short 0, byte 0, int len, str} 广播 → RegistNextTimer(idx)。
    // P183 地址重物化铁律：ORIG 每个站点重发 `call G_CDataManager; add $0xa4,%eax`
    // （08636a8d/08636b1f/08636b58），禁 `std::vector<...>& notices = ...` 引用缓存。
    register int ret;
    if (p2 < 0 ||
        static_cast<unsigned int>(p2) >=
            G_CDataManager()->m_serverParameter.m_light3c.size())
    {
        ret = 0;
    }
    else
    {
    PacketGuard guard;
    // P89/P183：ORIG 每站点 `lea -0x1c(%ebp),%eax; mov %eax,(%esp)`（guard 地址直传），
    // 禁具名 `InterfacePacketBuf& packet`（多占 -0x18 槽）。
    static_cast<InterfacePacketBuf&>(guard).put_header(0, 0xc);
    static_cast<InterfacePacketBuf&>(guard).put_byte(0);
    static_cast<InterfacePacketBuf&>(guard).put_short(0);
    static_cast<InterfacePacketBuf&>(guard).put_byte(0);
    int len = G_CDataManager()->m_serverParameter.m_light3c[p2].m_message.size();
    // ORIG 0x08636b50：先 put_int(len) 再 put_str
    static_cast<InterfacePacketBuf&>(guard).put_int(len);
    static_cast<InterfacePacketBuf&>(guard).put_str(
        G_CDataManager()->m_serverParameter.m_light3c[p2].m_message.c_str(), len);
    static_cast<InterfacePacketBuf&>(guard).finalize(true);
    G_GameWorld()->send_all(guard);
    const bool done = RegistNextTimer(p2);
    // P231/P307：ORIG 汇聚变量驻 %ebx（08636bd6 `mov $0x0,%ebx` / 08636bdd
    // `mov $0x1,%ebx`）且无栈槽 ⇒ 纯 register 存储类提示（非 asm 绑定，P0 合规）。
    if (!done)
        ret = 0;
    else
        ret = 1;
    }
    return ret;
