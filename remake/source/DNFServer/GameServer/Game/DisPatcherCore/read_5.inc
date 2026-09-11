// ORIG 0x081c632a T _ZN23DisPatcher_SelectCharac4readER9PacketBufR8MSG_BASE
// （报告 DisPatcher_SelectCharac/read.md 逐块映射）：get_byte → msg+0xd；
// 失败 LineFunc(0x156f, PRETTY, 0, 0)。
int DisPatcher_SelectCharac::read(PacketBuf& packet, MSG_BASE& msg)

{
    MSG_CHARAC_SELECT& m = *reinterpret_cast<MSG_CHARAC_SELECT*>(&msg);
    if (!packet.get_byte(*reinterpret_cast<char*>(&m.m_slot)))  // +0x0d（signed char→char& 适配）
    {
        return LineFunc(0x156f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
