int Dispatcher_P2P_Statistics::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_P2P_STATISTICS& data = reinterpret_cast<MSG_P2P_STATISTICS&>(msg);
    if (!packet.get_byte(*reinterpret_cast<char*>(&data.m_0d)))
    {
        return LineFunc(0x591a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(*reinterpret_cast<char*>(&data.m_0e)))
    {
        return LineFunc(0x591b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(data.m_10))
    {
        return LineFunc(0x591d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(data.m_12))
    {
        return LineFunc(0x591e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(data.m_14))
    {
        return LineFunc(0x591f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(data.m_18))
    {
        return LineFunc(0x5921,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(data.m_1c))
    {
        return LineFunc(0x5922,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(data.m_20))
    {
        return LineFunc(0x5923,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(data.m_24))
    {
        return LineFunc(0x5924,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(data.m_28))
    {
        return LineFunc(0x5926,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(data.m_2a))
    {
        return LineFunc(0x5927,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(data.m_2c))
    {
        return LineFunc(0x5928,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(data.m_30))
    {
        return LineFunc(0x592a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(data.m_34))
    {
        return LineFunc(0x592b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(data.m_38))
    {
        return LineFunc(0x592c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(data.m_3c))
    {
        return LineFunc(0x592d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}