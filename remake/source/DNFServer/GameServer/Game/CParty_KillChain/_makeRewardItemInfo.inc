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
