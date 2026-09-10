int Dispatcher_Teleport::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_TELEPORT& req = reinterpret_cast<MSG_TELEPORT&>(msg);
    if (!packet.get_short(req.m_field0e))
    {
        return LineFunc(0x2b34,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(req.m_field10))
    {
        return LineFunc(0x2b38,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(*reinterpret_cast<char*>(&req.m_field14)))
    {
        return LineFunc(0x2b3c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(*reinterpret_cast<char*>(&req.m_field15)))
    {
        return LineFunc(0x2b3f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
