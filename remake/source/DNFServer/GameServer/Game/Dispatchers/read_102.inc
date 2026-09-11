int Dispatcher_DonateGuildFund::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x…（帧 $0x18）：无指针物化槽，直接由形参槽算地址 ⇒ 内联表达式。
    if (!packet.get_int(*reinterpret_cast<int*>(reinterpret_cast<char*>(&msg) + 0xd)))
    {
        return LineFunc(0x3491, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}