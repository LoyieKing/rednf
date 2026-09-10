int Dispatcher_RequestDisjointItem::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081cfb28：short@0xe → short@0x10 → char@0x12
    MSG_REQUEST_DISJOINT_ITEM& m = reinterpret_cast<MSG_REQUEST_DISJOINT_ITEM&>(msg);
    if (!packet.get_short(m.m_field0e))
        return LineFunc(0x29ed, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_short(m.m_field10))
        return LineFunc(0x29f0, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_byte(*reinterpret_cast<char*>(&m.m_field12)))
        return LineFunc(0x29f3, __PRETTY_FUNCTION__, 0, 0);
    return 0;
