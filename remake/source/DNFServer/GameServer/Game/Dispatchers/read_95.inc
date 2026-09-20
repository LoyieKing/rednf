int Dispatcher_CancelJoinGuild::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_CANCEL_JOIN_GUILD& req = *reinterpret_cast<MSG_CANCEL_JOIN_GUILD*>(&msg);
    if (!packet.get_int(req.m_characNo))
    {
        return LineFunc(0x353c, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
