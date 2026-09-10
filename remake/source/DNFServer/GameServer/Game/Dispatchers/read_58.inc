int Dispatcher_TitleBookGet::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_TITLEBOOK_GET& m = reinterpret_cast<MSG_TITLEBOOK_GET&>(msg);
    if (!packet.get_int(m.m_itemSpace))
    {
        return LineFunc(0x420c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(m.m_field11))
    {
        return LineFunc(0x420e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(m.m_field15))
    {
        return LineFunc(0x4210,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(m.m_category))
    {
        return LineFunc(0x4212,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(m.m_field1d))
    {
        return LineFunc(0x4214,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
