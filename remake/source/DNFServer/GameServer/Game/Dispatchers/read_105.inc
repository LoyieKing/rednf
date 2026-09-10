int Dispatcher_LimitNpcBuyItem::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081e0f60：int@0xd → int@0x11
    struct __attribute__((packed)) BuyItemView
    {
        char m_head[0xd];
        int m_field0d;     // +0x0d
        int m_field11;     // +0x11
    };
    BuyItemView& v = reinterpret_cast<BuyItemView&>(msg);
    if (!packet.get_int(*reinterpret_cast<unsigned long*>(&v.m_field0d)))
        return LineFunc(0x559a, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(*reinterpret_cast<unsigned int*>(&v.m_field11)))
        return LineFunc(0x559d, __PRETTY_FUNCTION__, 0, 0);
    return 0;
