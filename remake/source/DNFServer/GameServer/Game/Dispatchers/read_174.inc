int Dispatcher_GuildCargoPushItem::read(PacketBuf& packet, MSG_BASE& msg)   // ORIG 0x081d0dfe (83 insn)
{
    static const char kFunc[] =
        "virtual int Dispatcher_GuildCargoPushItem::read(PacketBuf&, MSG_BASE&)";
    MSG_GUILD_CARGO_PUSH_ITEM& pMsg = reinterpret_cast<MSG_GUILD_CARGO_PUSH_ITEM&>(msg);
    if (!packet.get_byte(pMsg.m_field0d))
        return LineFunc(0x2c91, kFunc, 0, 0);           // msg+0xd byte itemSpace
    if (!packet.get_short(pMsg.m_field0e))
        return LineFunc(0x2c92, kFunc, 0, 0);           // msg+0xe short slot
    if (!packet.get_int(pMsg.m_field10))
        return LineFunc(0x2c93, kFunc, 0, 0);           // msg+0x10 int
    if (!packet.get_int(pMsg.m_field14))
        return LineFunc(0x2c94, kFunc, 0, 0);           // msg+0x14 int
    if (!packet.get_short(pMsg.m_field18))
        return LineFunc(0x2c95, kFunc, 0, 0);           // msg+0x18 short count
    return 0;
