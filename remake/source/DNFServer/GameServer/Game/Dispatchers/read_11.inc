int Dispatcher_UseDye::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_USE_DYE& req = reinterpret_cast<MSG_USE_DYE&>(msg);
    if (!packet.get_short(*reinterpret_cast<short*>(&req.m_0e)))
    {
        return LineFunc(0x6245,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<short*>(&req.m_10)))
    {
        return LineFunc(0x6248,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
