int Dispatcher_CreateDisjointStore::read(PacketBuf& packet, MSG_BASE& msg)
{
    // 同上 §8.3 TU 局部视图（冻结头未建模）。
    struct DisjointStoreMsgView    // 同上：自然对齐即达各字段偏移
    {
        char m_head[0xd];
        char m_storeName[0x103];   // +0x0d
        int m_field110;            // +0x110
        int m_field114;            // +0x114
        short m_field118;          // +0x118
        short m_field11a;          // +0x11a
    };
    DisjointStoreMsgView& base = reinterpret_cast<DisjointStoreMsgView&>(msg);
    if (!packet.get_int(base.m_field110))
    {
        return LineFunc(0x2976,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_str(base.m_storeName, 0x100,
                        base.m_field110))
    {
        return LineFunc(0x2979,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(base.m_field114))
    {
        return LineFunc(0x297c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(base.m_field118))
    {
        return LineFunc(0x297f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(base.m_field11a))
    {
        return LineFunc(0x2982,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
