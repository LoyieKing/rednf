int Dispatcher_DenyJoinGuild::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_DENY_JOIN_GUILD& req = *reinterpret_cast<MSG_DENY_JOIN_GUILD*>(&msg);
    if (!packet.get_int(req.m_characNo))
    {
        return LineFunc(0x357f, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
