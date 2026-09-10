int Dispatcher_MotionHack::read(PacketBuf& packet, MSG_BASE& msg)
{
    if (!packet.get_byte(*reinterpret_cast<char*>((char*)&msg + 0xd)))
    {
        return LineFunc(0x414, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_int(*reinterpret_cast<int*>((char*)&msg + 0xe)))
    {
        return LineFunc(0x416, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_int(*reinterpret_cast<int*>((char*)&msg + 0x12)))
    {
        return LineFunc(0x418, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
