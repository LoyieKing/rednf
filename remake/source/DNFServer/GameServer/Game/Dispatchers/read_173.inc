int Dispatcher_GuildCargoMoveItem::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081d17ba：ushort@0xd → int@0x11 → short@0xf → int@0x15
    MSG_GUILD_CARGO_MOVE_ITEM& m = reinterpret_cast<MSG_GUILD_CARGO_MOVE_ITEM&>(msg);
    if (!packet.get_short(m.m_field0d))
        return LineFunc(0x2dd2, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(m.m_field11))
        return LineFunc(0x2dd4, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_short(m.m_field0f))
        return LineFunc(0x2dd6, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(m.m_field15))
        return LineFunc(0x2dd8, __PRETTY_FUNCTION__, 0, 0);
    return 0;
