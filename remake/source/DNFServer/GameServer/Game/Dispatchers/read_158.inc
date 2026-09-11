int Dispatcher_AchievementTrigger::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081d7dfc：uint@0xd → ushort@0x11 → ushort@0x13 → ushort@0x15
    MSG_ACHIEVEMENT_TRIGGER& m = reinterpret_cast<MSG_ACHIEVEMENT_TRIGGER&>(msg);
    if (!packet.get_int(m.m_field0d))
        return LineFunc(0x4265, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_short(m.m_field11))
        return LineFunc(0x4267, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_short(m.m_field13))
        return LineFunc(0x4269, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_short(m.m_field15))
        return LineFunc(0x426b, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}