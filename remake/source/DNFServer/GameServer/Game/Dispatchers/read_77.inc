int Dispatcher_UseRenameCard::read(PacketBuf& packet, MSG_BASE& msg)
{
    char* pMsg = (char*)&msg;
    if (!packet.get_short(*(short*)(pMsg + 0x0e)))
    {
        return LineFunc(0x62f3,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
