int DisPatcher_MoveItem::read(PacketBuf& packet, MSG_BASE& msg)

{
    MSG_MOVE_ITEMSPACE& m = *reinterpret_cast<MSG_MOVE_ITEMSPACE*>(&msg);
    if (!packet.get_byte(m.m_fromSpace))                 // +0x0d
    {
        return LineFunc(0x1373,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(m.m_fromPos))                  // +0x0e
    {
        return LineFunc(0x1374,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(m.m_count))                      // +0x10
    {
        return LineFunc(0x1375,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(m.m_field14))                    // +0x14
    {
        return LineFunc(0x1376,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(m.m_toSpace))                   // +0x18
    {
        return LineFunc(0x1377,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(m.m_toPos))                    // +0x19
    {
        return LineFunc(0x1378,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(m.m_itemIdx))                    // +0x1b
    {
        return LineFunc(0x1379,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // +0x1f 4 字节槽（m_pad1f 起点 int 视图；ORIG read 实证读取）
    if (!packet.get_int(*reinterpret_cast<int*>(m.m_pad1f)))
    {
        return LineFunc(0x137b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(m.m_expanda))                   // +0x23
    {
        return LineFunc(0x137e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(m.m_expandb))                   // +0x24
    {
        return LineFunc(0x137f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
