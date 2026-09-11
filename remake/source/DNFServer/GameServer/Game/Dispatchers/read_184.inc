int Dispatcher_ValidateScriptHash::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x082641eb-0x08264272：裸偏移内联 add $0xd/$0xe/$0xf（无指针槽）。
    if (!packet.get_byte(*reinterpret_cast<char*>((char*)&msg + 0xd)))
    {
        return LineFunc(0x37d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(*reinterpret_cast<char*>((char*)&msg + 0xe)))
    {
        return LineFunc(0x382,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(*reinterpret_cast<int*>((char*)&msg + 0xf)))
    {
        return LineFunc(0x387,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}