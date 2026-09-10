int Dispatcher_Select_Item_Grwoth_Power::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_GROWTH_POWER& req = reinterpret_cast<MSG_GROWTH_POWER&>(msg);
    if (!packet.get_byte(req.m_0d))
    {
        return LineFunc(0x4a55,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    for (int i = 0; i < static_cast<int>(req.m_0d); ++i)
    {
        if (!packet.get_byte(*reinterpret_cast<unsigned char*>(&req.m_0e[i])))
        {
            return LineFunc(0x4a5a,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
    }
    return 0;
