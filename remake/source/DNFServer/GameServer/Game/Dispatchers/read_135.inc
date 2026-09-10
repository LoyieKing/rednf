int Dispatcher_RequestJoinGuild::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_REQUEST_GUILD_JOIN& data = reinterpret_cast<MSG_REQUEST_GUILD_JOIN&>(msg);
    if (!packet.get_int(data.m_0d))
    {
        return LineFunc(0x3506,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_str(data.m_11, 0x17, data.m_0d))
    {
        return LineFunc(0x3509,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(data.m_28))
    {
        return LineFunc(0x350d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_str(data.m_2c, 0x1a, data.m_28))
    {
        return LineFunc(0x3510,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
