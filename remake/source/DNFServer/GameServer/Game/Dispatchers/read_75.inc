int Dispatcher_SelectStriker::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_SELECT_STRIKER& req = *reinterpret_cast<MSG_SELECT_STRIKER*>(&msg);
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_0d)))
        return LineFunc(0x5b81, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_byte(req.m_0e))
        return LineFunc(0x5b84, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}
