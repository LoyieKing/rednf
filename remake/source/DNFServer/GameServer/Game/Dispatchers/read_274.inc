int Dispatcher_UpdateContractOfCubeInfo::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x0825f868：byte@0xd → byte@0xe
    char* m = reinterpret_cast<char*>(&msg);
    if (!packet.get_byte(*reinterpret_cast<unsigned char*>(m + 0xd)))
        return LineFunc(0x95e, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_byte(*reinterpret_cast<unsigned char*>(m + 0xe)))
        return LineFunc(0x961, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}