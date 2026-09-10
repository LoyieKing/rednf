int Dispatcher_InformNotice::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x T：读 unsigned int(msg+0xd)，失败记 LineFunc(0x597d)
    char* p = reinterpret_cast<char*>(&msg);
    if (!packet.get_int(*reinterpret_cast<unsigned int*>(p + 0xd)))
    {
        return LineFunc(0x597d, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
