// ORIG 0x081c9bfe T _ZN19DisPatcher_SortItem4readER9PacketBufR8MSG_BASE
// （报告 DisPatcher_SortItem/read.md 逐块映射）：get_byte → msg+0xd；
// 失败 LineFunc(0x1d16, PRETTY, 0, 0)。
int DisPatcher_SortItem::read(PacketBuf& packet, MSG_BASE& msg)

{
    MSG_SORT_ITEM& m = *reinterpret_cast<MSG_SORT_ITEM*>(&msg);
    if (!packet.get_byte(m.m_invenType))                  // +0x0d
    {
        return LineFunc(0x1d16,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
