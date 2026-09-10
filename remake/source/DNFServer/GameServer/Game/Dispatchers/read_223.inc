int Dispatcher_PrecheckSoloTeleport::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081e1920：int@0x2c（len）→ str@0xd（max 0x1e）
    MSG_SOLO_TELEPORT& m = reinterpret_cast<MSG_SOLO_TELEPORT&>(msg);
    if (!packet.get_int(m.m_2c))
        return LineFunc(0x572d, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_str(m.m_0d, 0x1e, m.m_2c))
        return LineFunc(0x5730, __PRETTY_FUNCTION__, 0, 0);
    return 0;
