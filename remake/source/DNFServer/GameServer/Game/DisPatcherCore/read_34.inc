int DisPatcher_SetPos::read(PacketBuf& packet, MSG_BASE& msg)

{
    MSG_USER_SET_POS& req = *reinterpret_cast<MSG_USER_SET_POS*>(&msg);
    if (!packet.get_short(req.m_0d))
    {
        return LineFunc(0x1ca2,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(req.m_0f))
    {
        return LineFunc(0x1ca4,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(req.m_11))
    {
        return LineFunc(0x1ca6,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(req.m_12))
    {
        return LineFunc(0x1ca8,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
