int Dispatcher_Write_On_The_GuildBoard::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081d314a：仅 len 一个 -0xc 槽（msg 内联裸址，无指针槽）
    int len = 0;
    if (!packet.get_int(len))
        return LineFunc(0x30c5, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_str((char*)&msg + 0xd, 0x78, len))
        return LineFunc(0x30c8, __PRETTY_FUNCTION__, 0, 0);
    return 0;
