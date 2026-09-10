int Dispatcher_BlueMarble::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_BLUE_MARBLE& req = reinterpret_cast<MSG_BLUE_MARBLE&>(msg);
    if (!packet.get_byte(req.m_0d))
    {
        return LineFunc(0x609a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
