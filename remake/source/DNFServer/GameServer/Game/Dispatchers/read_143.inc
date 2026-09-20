int Dispatcher_ChangeHostWarRoom::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_CHANGE_HOST_WARROOM& req = *reinterpret_cast<MSG_CHANGE_HOST_WARROOM*>(&msg);
    if (!packet.get_byte(req.m_field0d))
    {
        return LineFunc(0x418e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(req.m_field0e))
    {
        return LineFunc(0x4191,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
