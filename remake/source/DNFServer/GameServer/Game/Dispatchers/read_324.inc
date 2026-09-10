int Dispatcher_ExpertExtraction::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_EXPERT_EXTRACTION& m = reinterpret_cast<MSG_EXPERT_EXTRACTION&>(msg);
    if (!packet.get_byte(reinterpret_cast<char&>(m.m_0d)))
    {
        return LineFunc(0x45ac, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_short(reinterpret_cast<short&>(m.m_0e)))
    {
        return LineFunc(0x45ae, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_short(reinterpret_cast<short&>(m.m_10)))
    {
        return LineFunc(0x45b0, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
