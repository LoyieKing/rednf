int Dispatcher_Register_Quick_Party::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081ddf36：short@0xe → char@0x10
    MSG_QUICK_PARTY& m = reinterpret_cast<MSG_QUICK_PARTY&>(msg);
    if (!packet.get_short(m.m_0e))
        return LineFunc(0x4f1e, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_byte(m.m_10))
        return LineFunc(0x4f20, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}