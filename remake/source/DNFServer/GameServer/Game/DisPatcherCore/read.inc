int DisPatcher_BuyItem::read(PacketBuf& packet, MSG_BASE& msg)

{
    // ORIG 0x081...：无指针局部，实参内联 `mov 0x10(%ebp),%eax; add $0xd/11/15`（帧 0x18）。
    if (!packet.get_int(reinterpret_cast<MSG_BUY_ITEM*>(&msg)->m_itemIdx))   // +0x0d
    {
        return LineFunc(0x188,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(reinterpret_cast<MSG_BUY_ITEM*>(&msg)->m_buyCount))  // +0x11
    {
        return LineFunc(0x18b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(reinterpret_cast<MSG_BUY_ITEM*>(&msg)->m_deliveryType))  // +0x15
    {
        return LineFunc(0x18e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
