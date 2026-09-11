int Dispatcher_Float_RDATA_Modulation::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x T：读 unsigned int(msg+0x10)，失败记 LineFunc(0x582)
    if (!packet.get_int(*reinterpret_cast<unsigned int*>((char*)&msg + 0x10)))
    {
        return LineFunc(0x582, __PRETTY_FUNCTION__, 0, 0);
    }
    // ORIG 0x T：读 unsigned int(msg+0x14)，失败记 LineFunc(0x586)
    if (!packet.get_int(*reinterpret_cast<unsigned int*>((char*)&msg + 0x14)))
    {
        return LineFunc(0x586, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}