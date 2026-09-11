int Dispatcher_SelectStriker::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081e3d32：char@0xd → byte@0xe
    char* m = reinterpret_cast<char*>(&msg);
    if (!packet.get_byte(*reinterpret_cast<char*>(m + 0xd)))
        return LineFunc(0x5b81, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_byte(*reinterpret_cast<unsigned char*>(m + 0xe)))
        return LineFunc(0x5b84, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}