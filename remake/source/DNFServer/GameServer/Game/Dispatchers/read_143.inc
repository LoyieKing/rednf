int Dispatcher_ChangeHostWarRoom::read(PacketBuf& packet, MSG_BASE& msg)
{
    char* p = reinterpret_cast<char*>(&msg);
    if (!packet.get_byte(*reinterpret_cast<char*>(p + 0xd)))
    {
        return LineFunc(0x418e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(*reinterpret_cast<unsigned long*>(p + 0xe)))
    {
        return LineFunc(0x4191,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}