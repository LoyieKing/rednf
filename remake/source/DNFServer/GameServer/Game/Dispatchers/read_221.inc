int Dispatcher_PartyTeleportConfirm::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_PARTY_TELEPORT_CONFIRM& req = *reinterpret_cast<MSG_PARTY_TELEPORT_CONFIRM*>(&msg);
    if (!packet.get_byte(req.m_confirmFlag))
    {
        return LineFunc(0x4bd7, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
