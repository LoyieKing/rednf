int Dispatcher_ScanBotByDll::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x：裸偏移内联（无指针槽，帧 0x18）。
    if (!packet.get_int(*reinterpret_cast<int*>((char*)&msg + 0xd)))
    {
        return LineFunc(0x3d6, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_byte(*reinterpret_cast<char*>((char*)&msg + 0x11)))
    {
        return LineFunc(0x3db, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<short*>((char*)&msg + 0x12)))
    {
        return LineFunc(0x3e0, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
