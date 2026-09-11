int Dispatcher_PartyTeleportConfirm::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x T：读 unsigned char(msg+0xd)，失败记 LineFunc(0x4bd7)
    char* p = reinterpret_cast<char*>(&msg);
    if (!packet.get_byte(*reinterpret_cast<unsigned char*>(p + 0xd)))
    {
        return LineFunc(0x4bd7, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}