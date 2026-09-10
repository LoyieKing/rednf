int Dispatcher_TitleBookOtherUser::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x T：读 unsigned short(msg+0xd)，失败记 LineFunc(0x4244)
    char* p = reinterpret_cast<char*>(&msg);
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(p + 0xd)))
    {
        return LineFunc(0x4244, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
