int Dispatcher_GuildAttendanceInfo::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x T：读 char(msg+0xd)，失败记 LineFunc(0x35d9)
    char* p = reinterpret_cast<char*>(&msg);
    if (!packet.get_byte(*reinterpret_cast<char*>(p + 0xd)))
    {
        return LineFunc(0x35d9, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
