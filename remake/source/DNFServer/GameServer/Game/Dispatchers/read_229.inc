int Dispatcher_SeriaRoom_Deco_Event::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_SERIAROOM_DECO_EVENT& req = reinterpret_cast<MSG_SERIAROOM_DECO_EVENT&>(msg);
    if (!packet.get_short(req.m_decoEventId))
    {
        return LineFunc(0x5fff,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}