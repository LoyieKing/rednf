int Dispatcher_DenyJoinGuild::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x T：读 unsigned int(msg+0xd)，失败记 LineFunc(0x357f)
    char* p = reinterpret_cast<char*>(&msg);
    if (!packet.get_int(*reinterpret_cast<unsigned int*>(p + 0xd)))
    {
        return LineFunc(0x357f, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
