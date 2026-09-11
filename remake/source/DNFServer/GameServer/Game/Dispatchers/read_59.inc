int Dispatcher_TitleBookPut::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_TITLEBOOK_PUT& m = reinterpret_cast<MSG_TITLEBOOK_PUT&>(msg);
    if (!packet.get_int(m.m_itemSpace))
    {
        return LineFunc(0x41d4,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(m.m_field11))
    {
        return LineFunc(0x41d6,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(m.m_field15))
    {
        return LineFunc(0x41d8,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(m.m_field19))
    {
        return LineFunc(0x41da,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(m.m_field1d))
    {
        return LineFunc(0x41dc,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}