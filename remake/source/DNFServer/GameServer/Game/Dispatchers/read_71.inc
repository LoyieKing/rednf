int Dispatcher_HatchCreature::read(PacketBuf& packet, MSG_BASE& msg)
{
    char* pMsg = (char*)&msg;
    if (!packet.get_byte(*(unsigned char*)(pMsg + 0x0d)))
    {
        return LineFunc(0x2518,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(*(short*)(pMsg + 0x0e)))
    {
        return LineFunc(0x251b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}