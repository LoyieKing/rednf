int Dispatcher_GuildCargoPushItem::read(PacketBuf& packet, MSG_BASE& msg)   // ORIG 0x081d0dfe (83 insn)
{
    // [P139 2026-09-11] 手写串改 __PRETTY_FUNCTION__：补齐 ORIG 的
    //   `_ZZ…E19__PRETTY_FUNCTION__` 局部符号；串内容经 probe_pretty3 逐字节核对一致
    //   （.rodata 不变、引用点仍共享同一静态数组）。
    MSG_GUILD_CARGO_PUSH_ITEM& pMsg = reinterpret_cast<MSG_GUILD_CARGO_PUSH_ITEM&>(msg);
    if (!packet.get_byte(pMsg.m_field0d))
        return LineFunc(0x2c91, __PRETTY_FUNCTION__, 0, 0);           // msg+0xd byte itemSpace
    if (!packet.get_short(pMsg.m_field0e))
        return LineFunc(0x2c92, __PRETTY_FUNCTION__, 0, 0);           // msg+0xe short slot
    if (!packet.get_int(pMsg.m_field10))
        return LineFunc(0x2c93, __PRETTY_FUNCTION__, 0, 0);           // msg+0x10 int
    if (!packet.get_int(pMsg.m_field14))
        return LineFunc(0x2c94, __PRETTY_FUNCTION__, 0, 0);           // msg+0x14 int
    if (!packet.get_short(pMsg.m_field18))
        return LineFunc(0x2c95, __PRETTY_FUNCTION__, 0, 0);           // msg+0x18 short count
    return 0;
}