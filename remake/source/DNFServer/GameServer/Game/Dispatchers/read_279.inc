int Dispatcher_Buy_Charac_Status_Using_QP::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x T：读 unsigned char(msg+0xd)，失败记 LineFunc(0x3fb8)
    char* p = reinterpret_cast<char*>(&msg);
    if (!packet.get_byte(*reinterpret_cast<unsigned char*>(p + 0xd)))
    {
        return LineFunc(0x3fb8, __PRETTY_FUNCTION__, 0, 0);
    }
    // ORIG 0x T：读 unsigned char(msg+0xe)，失败记 LineFunc(0x3fba)
    if (!packet.get_byte(*reinterpret_cast<unsigned char*>(p + 0xe)))
    {
        return LineFunc(0x3fba, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}