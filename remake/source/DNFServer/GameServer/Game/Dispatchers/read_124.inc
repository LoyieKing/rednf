int Dispatcher_InformNotice_2nd::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x T：读 unsigned int(msg+0xd)，失败记 LineFunc(0x59b8)
    char* p = reinterpret_cast<char*>(&msg);
    if (!packet.get_int(*reinterpret_cast<unsigned int*>(p + 0xd)))
    {
        return LineFunc(0x59b8, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}