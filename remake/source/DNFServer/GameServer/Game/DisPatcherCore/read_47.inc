int DisPatcher_DropItem::read(PacketBuf& packet, MSG_BASE& msg)

{
    MSG_DROP_ITEM& req = *reinterpret_cast<MSG_DROP_ITEM*>(&msg);
    if (!packet.get_short(req.m_0d))
    {
        return LineFunc(0xd0c, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_short(req.m_0f))
    {
        return LineFunc(0xd0d, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_byte(*reinterpret_cast<char*>(&req.m_11)))
    {
        return LineFunc(0xd0e, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_short(req.m_12))
    {
        return LineFunc(0xd0f, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_int(req.m_14))
    {
        return LineFunc(0xd10, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_byte(req.m_padTail[0]))
    {
        return LineFunc(0xd12, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
