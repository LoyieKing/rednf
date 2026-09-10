int Dispatcher_WarRoomWpPerMonster::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081d5bcc：int@0xd → int@0x11 → ushort@0x15
    MSG_WARROOM_WP_PER_MONSTER& m =
        reinterpret_cast<MSG_WARROOM_WP_PER_MONSTER&>(msg);
    if (!packet.get_int(m.m_field0d))
        return LineFunc(0x397d, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(m.m_field11))
        return LineFunc(0x3980, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_short(m.m_killerUid))
        return LineFunc(0x3983, __PRETTY_FUNCTION__, 0, 0);
    return 0;
