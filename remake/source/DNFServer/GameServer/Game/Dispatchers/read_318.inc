// ==== Dispatcher_PcRoomRentItem::read (ORIG 0x081e512e) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081e512e orig/ours=96/94 ndiff-blocks=13
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,-0x10(%ebp) | mov    -0x10(%ebp),%eax | movl   $&_ZZN25Dispatcher_PcRoomRentItem4readER9PacketBufR8MSG_BASEE19__PRETTY_FUNCTION__,0x4(%esp
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
        // [L7DispG4 2026-09-11 · 技巧14/32 disp 折叠] ORIG 循环体为
        //   `2i + msg` 基址 + **单级 disp**（第二元素 `add $0x10,%eax`，
        //   我方 `req.m_0f[i*2+1]` 产 `add $0x1; add %eax; add $0xf` 多 1 条）。
        //   元素对 @+0x0f+2i/+0x10+2i（RentInElem sectionIdx/itemIdx）⇒ packed 视图成员。
        struct __attribute__((packed)) RentElemView
        {
            unsigned char m_section;      // +0x0f + 2i
            unsigned char m_item;         // +0x10 + 2i
        };
        struct __attribute__((packed)) RentItemView
        {
            char m_head[0xf];
            RentElemView m_elems[1];
        };
        // [G6·read_318 负结论 2026-09-11] 视图引用形（`RentItemView& rv = …` 两使用点）
        //   实测 ndiff 8→15（条数 96/98 不变）⇒ 劣化 ⇒ 保持裸 cast 表达式形。
        if (!packet.get_byte(reinterpret_cast<char&>(
                reinterpret_cast<RentItemView*>(&req)->m_elems[i].m_section)))
        {
            return LineFunc(24136,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!packet.get_byte(reinterpret_cast<char&>(
                reinterpret_cast<RentItemView*>(&req)->m_elems[i].m_item)))
        {
            return LineFunc(24138,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
    }
    return 0;
}