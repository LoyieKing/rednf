int Dispatcher_CompatibilityIndex::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_COMPATIBILITY_INDEX& data = reinterpret_cast<MSG_COMPATIBILITY_INDEX&>(msg);
    if (!packet.get_byte(data.m_0d))
    {
        return LineFunc(0x58df,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(data.m_0e))
    {
        return LineFunc(0x58e0,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(*reinterpret_cast<unsigned int*>(&data.m_10)))
    {
        return LineFunc(0x58e1,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(&data.m_14)))
    {
        return LineFunc(0x58e2,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(&data.m_16)))
    {
        return LineFunc(0x58e3,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(&data.m_18)))
    {
        return LineFunc(0x58e4,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(&data.m_1a)))
    {
        return LineFunc(0x58e5,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(data.m_1c))
    {
        return LineFunc(0x58e6,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(data.m_1d))
    {
        return LineFunc(0x58e7,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}