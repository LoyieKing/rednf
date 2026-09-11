int Dispatcher_ChangeRandomOption::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081dc8de：ushort@0xe → ushort@0x10 → byte@0x12
    MSG_CHANGE_RANDOM_OPTION& m = reinterpret_cast<MSG_CHANGE_RANDOM_OPTION&>(msg);
    if (!packet.get_short(m.m_0e))
        return LineFunc(0x4cd8, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_short(m.m_10))
        return LineFunc(0x4cdb, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_byte(m.m_12))
        return LineFunc(0x4cde, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}