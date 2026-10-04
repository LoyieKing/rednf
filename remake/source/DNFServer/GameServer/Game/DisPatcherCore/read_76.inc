int Dispather_RegenerationRandomOption::read(PacketBuf& packet, MSG_BASE& msg)

{
    MSG_REGENERATION_RANDOM_OPTION& req =
        *reinterpret_cast<MSG_REGENERATION_RANDOM_OPTION*>(&msg);
    if (!packet.get_short(req.m_equipSlot0))
    {
        return LineFunc(0x40d3,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(req.m_equipSlot1))
    {
        return LineFunc(0x40d6,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(req.m_srcOptIndex0))
    {
        return LineFunc(0x40da,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(req.m_srcOptIndex1))
    {
        return LineFunc(0x40dd,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
