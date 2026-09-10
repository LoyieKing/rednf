int DisPatcher_SetPartyInfo::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_PARTY_SET_INFO& req = *reinterpret_cast<MSG_PARTY_SET_INFO*>(&msg);
    if (!packet.get_byte(req.m_titleIndex))
    {
        return LineFunc(0x10d9, __PRETTY_FUNCTION__, 0, 0);
    }
    if (req.m_titleIndex == 0)
    {
        if (!packet.get_int(req.m_titleLen))
        {
            return LineFunc(0x10dd, __PRETTY_FUNCTION__, 0, 0);
        }
        if (!packet.get_str(req.m_title, 0x20, req.m_titleLen))
        {
            return LineFunc(0x10e0, __PRETTY_FUNCTION__, 0, 0);
        }
    }
    if (!packet.get_byte(req.m_userMax))
    {
        return LineFunc(0x10e2, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_short(req.m_dungIndex))
    {
        return LineFunc(0x10e5, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_byte(req.m_dungDiffi))
    {
        return LineFunc(0x10e6, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
