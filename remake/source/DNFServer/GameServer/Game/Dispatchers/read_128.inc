int Dispatcher_Open_Guild_Board::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG（帧 $0x18）：无指针物化槽 ⇒ 内联表达式（同 DonateGuildFund 口径）。
    if (!packet.get_byte(reinterpret_cast<MSG_GUILD_BOARD*>(&msg)->m_b2))  // +0xb2
    {
        return LineFunc(0x30aa, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
