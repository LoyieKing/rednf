// ============================================================================
// _makeRewardItemInfo —— ORIG T 0x085ad0be。
// item+0x3f（调用方结构扩展标志，越界字段按裸偏移访问）：-1 → 归零并清尾随
// addInfo；为 0 → 包内 put_byte(1)；否则尝试入包（tryInsertItemIntoInventory
// 槽位<0 视为失败回退 put_byte(1)，成功 put_byte(2)）。尾部统一
// put_int(0)/put_int(item.add_info())，标志非 0 时再补写标志与尾随 addInfo。
// 尾随 addInfo 宿主为 item+0x3d 处的 Inven_Item 视图（set_add_info 写其 +0x07）。
// ============================================================================
// [b28d-solo-CParty_KillChain AE+真 bug] ORIG 0x085ad0be：本波修法——①去 itemBlob/
// extFlag 引用/tailItem 三个具名槽，改 kcTailItem 宏内联（P0：ORIG 每次重发
// `mov 0x10(%ebp),%eax; add $0x3d,%eax`）；②slot 函数头预清 -1（P198①，ORIG 0x085ad0c5
// `movl $0xffffffff,-0xc(%ebp)`）；③getCurCharacInvenR receiver 链式内联（P0/P52，消 inven
// 槽、帧由 $0x78 回到 ORIG 的 push %ebx+sub $0x64）；④if/else 极性反写（ORIG put_byte(1)
// 是跳转块，P59/P194）。**真 bug**：tryInsertItemIntoInventory 原传 `*item`，ORIG 0x085ad111
// `lea 0x3d(%eax),%ebx` + 0x3d 字节按值拷贝 ⇒ 入参是 item+0x3d 的尾随 Inven_Item 视图对象。
// 123=123 AE，P167 CLEAR。
void CParty::_makeRewardItemInfo(CUser const* user, Inven_Item* item,
                                 PacketGuard& packet)
{
    // P198①：ORIG 0x085ad0c5 函数头 `movl $0xffffffff,-0xc(%ebp)` ⇒ 具名 int
    // slot 预清 -1（else 分支才回写 tryInsert 结果）。
    int slot = -1;

    if (kcTailItem(item)->m_addInfo == 0xffffffffu)
    {
        kcTailItem(item)->m_addInfo = 0;
        kcTailItem(item)->set_add_info(0);
    }

    // ORIG 0x085ad106 `test; je <put_byte(1) 块>`：extFlag==0 走**跳转**块，
    // 非 0 直落 ⇒ 源码须写 `!= 0` 正向臂 + `== 0` 落尾（P59/P194 极性判据）。
    if (kcTailItem(item)->m_addInfo != 0)
    {
        // ORIG 0x085ad111 `lea 0x3d(%eax),%ebx` + 0x3d 字节按值拷贝 ⇒ 入参是
        // 尾随 Inven_Item 视图对象（不是 item 本体）。
        // P0/P52：ORIG 0x085ad114 无 inven 具名槽（getCurCharacInvenR 结果在
        // %eax 跨按值拷贝段保活）⇒ receiver 链式内联，禁中间指针局部。
        slot = const_cast<CUser*>(user)
                   ->getCurCharacInvenR()
                   ->tryInsertItemIntoInventory(*kcTailItem(item));
        // ORIG 0x085ad19e `js <put_byte(1) 块>`：slot<0 走跳转块，slot>=0 直落。
        if (slot >= 0)
        {
            packet.put_byte(2);
        }
        else
        {
            packet.put_byte(1);
            kcTailItem(item)->m_addInfo = 0;
            kcTailItem(item)->set_add_info(0);
        }
    }
    else
    {
        packet.put_byte(1);
    }

    packet.put_int(0);
    packet.put_int(item->get_add_info());
    if (kcTailItem(item)->m_addInfo != 0)
    {
        packet.put_int(kcTailItem(item)->m_addInfo);
        packet.put_int(kcTailItem(item)->get_add_info());
    }
}