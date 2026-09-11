// ==== Dispatcher_PartyTeleport::read (ORIG 0x081dbe70) ====
int Dispatcher_PartyTeleport::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_PARTY_TELEPORT& req = reinterpret_cast<MSG_PARTY_TELEPORT&>(msg);
    if (!packet.get_byte(req.m_field0d))                      // +0x0d village（uchar& 重载）
    {
        return LineFunc(19231,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(req.m_field0e))                      // +0x0e area
    {
        return LineFunc(19232,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<short*>(&req.m_field0f)))   // +0x0f posx
    {
        return LineFunc(19233,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<short*>(&req.m_field11)))   // +0x11 posy
    {
        return LineFunc(19234,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(req.m_field13))                      // +0x13 direction
    {
        return LineFunc(19235,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}