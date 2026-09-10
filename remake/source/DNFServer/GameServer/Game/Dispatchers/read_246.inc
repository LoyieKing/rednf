int Dispatcher_LogIngameAdvertisement::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081e4cd0：char@0xd → short@0xe → char@0x10 → uint@0x11
    MSG_LOG_INGAME_ADVERTISEMENT& m =
        reinterpret_cast<MSG_LOG_INGAME_ADVERTISEMENT&>(msg);
    if (!packet.get_byte(*reinterpret_cast<char*>(&m.m_field0d)))
        return LineFunc(0x5d6f, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_short(m.m_field0e))
        return LineFunc(0x5d71, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_byte(*reinterpret_cast<char*>(&m.m_field10)))
        return LineFunc(0x5d73, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(m.m_field11))
        return LineFunc(0x5d75, __PRETTY_FUNCTION__, 0, 0);
    return 0;
