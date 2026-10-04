int Dispatcher_EnchantByBead::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081d2218：byte@0xd → short@0xe → byte@0x10 → short@0x12，失败逐级 LineFunc
    MSG_ENCHANT_BY_BEAD& m = reinterpret_cast<MSG_ENCHANT_BY_BEAD&>(msg);
    if (!packet.get_byte(m.m_field0d))
        return LineFunc(0x2f1c, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_short(m.m_field0e))
        return LineFunc(0x2f1d, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_byte(m.m_field10))
        return LineFunc(0x2f1f, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_short(m.m_field12))
        return LineFunc(0x2f20, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}