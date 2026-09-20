int Dispatcher_RequestIngameAdvertisement::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_REQUEST_INGAME_ADVERTISEMENT& req = *reinterpret_cast<MSG_REQUEST_INGAME_ADVERTISEMENT*>(&msg);
    if (!packet.get_short(req.m_field0d))
    {
        return LineFunc(
            0x5c10,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    return 0;
}
