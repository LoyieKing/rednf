int Dispatcher_GuildCargoPopItem::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081d1388：ushort@0xd → int@0xf → int@0x13 → byte@0x17
    MSG_GUILD_CARGO_POP_ITEM& m =
        reinterpret_cast<MSG_GUILD_CARGO_POP_ITEM&>(msg);
    if (!packet.get_short(m.m_field0d))
        return LineFunc(0x2d3b, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(m.m_itemIdx))
        return LineFunc(0x2d3c, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(m.m_field13))
        return LineFunc(0x2d3d, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_byte(m.m_field17))
        return LineFunc(0x2d3e, __PRETTY_FUNCTION__, 0, 0);
    return 0;
