// b28d-solo-Dispatchers verdict=AE(routed) [S162-verify] ORIG 0x081d307e orig/ours=21/21
int Dispatcher_Open_Guild_Board::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG（帧 $0x18）：无指针物化槽 ⇒ 内联表达式（同 DonateGuildFund 口径）。
    // [P4-p167 符号性] ORIG callee 直证 get_byte(char&)；m_b2 声明为 unsigned char
    //   （PacketTypes.h:668），故调用点加 reinterpret_cast<char&>，寻址仍 base+0xb2。
    if (!packet.get_byte(reinterpret_cast<char&>(reinterpret_cast<MSG_GUILD_BOARD*>(&msg)->m_b2)))  // +0xb2
    {
        return LineFunc(0x30aa, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}