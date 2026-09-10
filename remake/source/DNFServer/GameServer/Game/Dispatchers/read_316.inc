int Dispatcher_MonstercardBind::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_MONSTERCARD_BIND& m = reinterpret_cast<MSG_MONSTERCARD_BIND&>(msg);
    // ORIG：get_short 直接读入 MSG 字段（msg+0xd/0xf/0x11，无临时变量）
    if (!packet.get_short(reinterpret_cast<short&>(m.m_0d)))
    {
        return LineFunc(
            0x444a,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    if (!packet.get_short(reinterpret_cast<short&>(m.m_0f)))
    {
        return LineFunc(
            0x444c,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    if (!packet.get_short(reinterpret_cast<short&>(m.m_11)))
    {
        return LineFunc(
            0x444e,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    return 0;
