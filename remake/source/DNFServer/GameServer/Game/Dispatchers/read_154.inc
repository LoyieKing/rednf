int Dispatcher_SecretShopBuyItem::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081d25f8：int@0xd → int@0x11（须 >=1 否则 LineFunc 0x2f84）
    MSG_SECRET_SHOP_BUY_ITEM& m =
        reinterpret_cast<MSG_SECRET_SHOP_BUY_ITEM&>(msg);
    if (!packet.get_int(m.m_itemIdx))
        return LineFunc(0x2f80, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(m.m_count))
        return LineFunc(0x2f81, __PRETTY_FUNCTION__, 0, 0);
    if (m.m_count < 1)
        return LineFunc(0x2f84, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}