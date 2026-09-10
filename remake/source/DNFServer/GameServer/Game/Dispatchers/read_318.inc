int Dispatcher_PcRoomRentItem::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_RENT_ITEM& req = reinterpret_cast<MSG_RENT_ITEM&>(msg);
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_0e)))          // +0x0e type
    {
        return LineFunc(24124,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_0d)))          // +0x0d count
    {
        return LineFunc(24128,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // ORIG 0x081e51xx：两判定各自 `mov -0x10(%ebp),%eax; movzbl 0xd(%eax),%eax`
    // —— count 值**无栈槽往返**（P0：具名 char 局部会占 -0xd 并把 msg 槽顶到 -0x14）。
    if (static_cast<char>(req.m_0d) < 1 ||
        static_cast<char>(req.m_0d) > 1)                              // 恒为 1 项
    {
        return LineFunc(24131,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    for (int i = 0; i < static_cast<int>(static_cast<char>(req.m_0d)); ++i)
    {
        // 元素对 @+0x0f+2i/+0x10+2i（RentInElem sectionIdx/itemIdx；count==1 不越界）
        if (!packet.get_byte(req.m_0f[i * 2]))
        {
            return LineFunc(24136,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!packet.get_byte(req.m_0f[i * 2 + 1]))
        {
            return LineFunc(24138,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
    }
    return 0;
