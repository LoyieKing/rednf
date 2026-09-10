int Dispatcher_RequestAssaultPrice::read(PacketBuf& packet, MSG_BASE& msg)
{
    MsgRequestAssaultPrice& m = static_cast<MsgRequestAssaultPrice&>(msg);
    if (!packet.get_short(reinterpret_cast<unsigned short&>(m.m_0d)))
    {
        return LineFunc(0x50bd, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
