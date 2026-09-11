// ==== Dispatcher_PurifyItem::read (ORIG 0x08218b94 T) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x08218b94 orig/ours=69/80 ndiff-blocks=13
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    -0xc(%ebp),%eax | add    $0xd,%eax | movl   $&_ZZN21Dispatcher_PurifyItem4readER9PacketBufR8MSG_BASEE19__PRETTY_FUNCTION__,0x4(%esp)
int Dispatcher_PurifyItem::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_PURIFY_ITEM& m = reinterpret_cast<MSG_PURIFY_ITEM&>(msg);
    // ORIG 0x08218b94：四段都是 `mov -0xc(%ebp),%eax; add $K,%eax; mov %eax,0x4(%esp)`
    // —— 目标地址**直接**传给取包函数（word/itemIdx 两个临时槽与「取完再回写」的
    // store 全不存在，P0）⇒ 删临时，按字段地址入参。m_0f/m_15 在权威头是 int，
    // get_item_idx 取 unsigned long& ⇒ 具名字段上的引用 cast（不新增偏移、不改头）。
    if (!packet.get_short(m.m_0d))
        return LineFunc(0xcbbf, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_item_idx(reinterpret_cast<unsigned long&>(m.m_0f)))
        return LineFunc(0xcbc1, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_short(m.m_13))
        return LineFunc(0xcbc3, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_item_idx(reinterpret_cast<unsigned long&>(m.m_15)))
        return LineFunc(0xcbc5, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}