// ORIG 0x081c2bee T _ZN19DisPatcher_DropItem4readER9PacketBufR8MSG_BASE
// （docs/class_func_reports/DisPatcher_DropItem/read.md 汇编+反编译 C 全文转写）：
// 依次 get_short(+0x0d)/get_short(+0x0f)/get_byte(char&)(+0x11)/get_short(+0x12)/
// get_int(+0x14)/get_byte(char&)(+0x18)；失败分别 LineFunc 0xd0c..0xd12；成功返回 0。
int DisPatcher_DropItem::read(PacketBuf& packet, MSG_BASE& msg)

{
    MSG_DROP_ITEM& req = *reinterpret_cast<MSG_DROP_ITEM*>(&msg);
    if (!packet.get_short(req.m_0d))
    {
        return LineFunc(0xd0c, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_short(req.m_0f))
    {
        return LineFunc(0xd0d, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_byte(req.m_11))
    {
        return LineFunc(0xd0e, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_short(req.m_12))
    {
        return LineFunc(0xd0f, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_int(req.m_14))
    {
        return LineFunc(0xd10, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_byte(req.m_padTail[0]))
    {
        return LineFunc(0xd12, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
