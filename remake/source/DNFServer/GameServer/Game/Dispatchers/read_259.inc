int Dispatcher_Write_On_The_GuildBoard::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_WRITE_GUILD_BOARD& req = *reinterpret_cast<MSG_WRITE_GUILD_BOARD*>(&msg);
    int len = 0;
    if (!packet.get_int(len))
        return LineFunc(0x30c5, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_str(req.m_text, 0x78, len))
        return LineFunc(0x30c8, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}
