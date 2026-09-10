int WarRoom::HandleItemDrop(CUser* user, int a, int b, char c, int d, int e)
{
    // [b28d-solo-WarRoom] DIFF(504) | ORIG 0x086bf5fc | 复核 b20 判词：P167
    //   100/100 归零；残余为 N82（4.1 不按 lifetime 复用栈槽 → 帧 -0x1ec 槽号
    //   平移）+ N76/N18a store 装载序 + 4.1 switch 判定树形态 ⇒ 语义完成、
    //   byte 层 N 封顶。
    // ORIG 086bf617/086bf627：GetState() 被短路求值调用两次（&& 两侧各一次），
    // 不缓存到局部变量。
    if ((GetState() != 3) && (GetState() != 4))
        return 0x13;
    if (e < 0)
    {
        // ORIG 086bf651..086bf6a8：两个结果都作为可变参数入栈
        // （%ebx=getCurCharacName 保存在 0x18(%esp)，%eax=NumberToString 在 0x14(%esp)）。
        LogManager::logFormat(1, "WarRoom.cpp",
                              __PRETTY_FUNCTION__,
                              0xa1a, "user - accid[%s] %s : count<0",
                              NumberToString(user->get_acc_id(), 0),
                              user->getCurCharacName());
        return 0x11;
    }
    if (user->CheckInTrade() != 0)
        return 0x13;
    map_item field;
    Inven_Item item;
    // ORIG 086bf6ec：switch(c) 三分支（case 0 直落、case 2 → +0x25b、case 3 → +0x348）。
    // ORIG 086bf876/086bf96b/086bf789：按值返回直接落 sret 临时（-0x188）后
    // 逐字段赋值进 item，没有具名中转对象，也没有额外 Inven_Item::Inven_Item()。
    switch (c)
    {
    case 0:
        if (d == 0)
        {
            field.m_count = 0;
            field.m_item.m_field1 = 2;
            field.m_item.m_addInfo = 0;
            field.m_item.set_add_info(e);
            field.m_item.m_fieldb = 0;
            field.m_item.ResetItemAttr();
        }
        else
        {
            item = user->getCurCharacInvenR()->GetInvenSlot(1, d);
        }
        break;
    case 2:
        item = user->getCurCharacCargoR()->get_cargo_slot(d);
        break;
    case 3:
        item = user->getCurCharacInvenR()->GetInvenSlot(0, d);
        break;
    }
    int price = 0;
    // ORIG 086bfa3e/086bfa4c：直接重复读 item.m_addInfo（-0xd3），无 itemIdx 局部。
    if (item.m_addInfo != 0)
    {
        CItem* pItem = G_CDataManager()->find_item(item.m_addInfo);
        if (pItem == 0)
        {
            // ORIG 086bfa6e：item.m_addInfo 作为可变参数入 0x14(%esp)。
            LogManager::logFormat(
                1, "WarRoom.cpp",
                __PRETTY_FUNCTION__, 0xa4c,
                "[WarRoom::HandleDropItem] Null Item %d", item.m_addInfo);
            return 0xffffffff;
        }
        // [b28d-solo-WarRoom P321-T2] ORIG 0x086bfac4 范围检查用 `ja`（无符号）
        //   ⇒ attach 的消费语义是 unsigned；声明同型以复现 4.1 跳表分派。
        unsigned int attach = (unsigned int)pItem->GetAttachType();
        // [b28d-solo-WarRoom] ORIG 086bfac4 用跳表（cmp $0x5/ja +
        //   mov 0x8cf98b0(,%eax,4)/jmp *%eax；表项 0/4→默认、1/2/5→ret 0x17、
        //   3→下方判定）。翻正旋钮：switch 表达式按 T2 无符号化 + 6 个 case 全
        //   独立书写 ⇒ 4.4 由判定树改发跳表（shl+mov 形态）。残余 = 4.4 跳表前
        //   多一条范围检查（跨版本固有），语义与 ORIG 一致。
        // [b28d-solo-WarRoom-3] P325：ORIG 跳表后只有**一个** `mov $0x17,%ebx;
        //   jmp 086c03f6` 块（086bfacd，case 1/2/5 共用一个标签体），原重建每
        //   case 各发一块 ⇒ 多 2 块（4 条）。改成共享 case 标签复现单块形。
        switch (attach)
        {
        case 0:
            break;
        case 1:
        case 2:
        case 5:
            return 0x17;
        case 3:
            if (item.m_field0 != 1)
                return 0x17;
            break;
        case 4:
            break;
        }
        // ORIG 086bfaef..086bfb28：两层嵌套 if（外层直接 test %al），
        // 内层用具名 CEquipItem*（-0x1c）连测两次 +0x234。
        if (item.isEquipableItemType())
        {
            CEquipItem* pEquip = (CEquipItem*)pItem;
            if ((pEquip->m_itemType == 0xb) || (pEquip->m_itemType < 10))
            {
                return 0x17;
            }
        }
        field.m_count = 0;
        field.m_item.m_field1 = item.m_field1;
        field.m_item.m_addInfo = item.m_addInfo;
        // ORIG 086bfb56：xor $0x1 → 条件写作 !isEquipableItemType()，
        // 「非装备」分支在前（set_add_info(e)），装备分支在后。
        if (!item.isEquipableItemType())
        {
            field.m_item.set_add_info(e);
        }
        else
        {
            field.m_item.set_add_info(item.m_addInfo2);
        }
        field.m_item.SetItemAttr(item.GetItemAttr());
        field.m_item.m_fieldb = item.m_fieldb;
        field.m_item.m_field0 = item.m_field0;
        field.m_item.m_fieldd = item.m_fieldd;
        field.m_item.m_amp = item.m_amp;
        // ORIG 086bfc16..086bfc47：+0x25..+0x32 共 14 字节整体拷贝
        //（4+4+4+2），即 RandomOption 子对象整体赋值，而非逐成员。
        field.m_item.m_random.m_random = item.m_random.m_random;
        field.m_item.m_upgradeSep.SetUpgradeSeparate(
            item.m_upgradeSep.GetUpgradeSeparate());
        // ORIG 086bfc7c：直接把 &price（-0x8c）作为出参，无中转局部。
        G_Store()->GetSellItemPrice(item, pItem, (short)e, false, price);
    }
    // ORIG 086bfcab：第二个 switch(c)，判定树 cmp2/je → cmp2/jg → test(case 0)
    // → cmp3/je、cmp7/je；分支体按源码序 0、2、3、7 排布（cMyTrace 栈槽
    // -0x88/-0x78/-0x68/-0x58/-0x48 顺序即为此序）。
    switch (c)
    {
    case 0:
        if (d == 0)
        {
            if (((CInventory*)user->getCurCharacInvenW())
                    ->use_money(e, (eMoneySubReason)6, 1) != 1)
            {
                // ORIG 086bfd4e：只压入两个可变参数（%ebx=getCurCharacNo、0x20(%ebp)=e），
                // 格式串第三个 %d 在 ORIG 里本就无对应实参 —— 忠实保留。
                cMyTrace(__PRETTY_FUNCTION__, 0xa9f, 5)(
                    "CParty::drop_item , use_money failed, User ch=%d , %d %d",
                    user->getCurCharacNo(), e);
                return 0x11;
            }
            GetInstanceValueStatistic()->AddValueStatistic(
                (VALUE_STATISTIC_FIELD)0x17, user, (unsigned)e);
        }
        else
        {
            if (user->getCurCharacInvenW()->delete_item((INVEN_TYPE)1, d, e, (eItemDelReason)6, 1) != 1)
            {
                // ORIG 086bfe11：0x10(%esp)=0x20(%ebp)=e、0xc(%esp)=0x1c(%ebp)=d。
                cMyTrace(__PRETTY_FUNCTION__, 0xaae, 5)(
                    "CParty::drop_item , delete_item failed, User ch=%d , %d %d",
                    user->getCurCharacNo(), d, e);
                return 0x11;
            }
            GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)0x16,
                                                          user, (unsigned)price);
        }
        break;
    case 2:
        if (user->getCurCharacCargoW()->delete_item(d, e, (eItemDelReason)0x10) != 1)
        {
            // ORIG 086bfed1：0x10(%esp)=0x20(%ebp)=e、0xc(%esp)=0x1c(%ebp)=d。
            cMyTrace(__PRETTY_FUNCTION__, 0xabf, 5)(
                "CParty::drop_item , delete_item failed 2, User ch=%d , %d %d",
                user->getCurCharacNo(), d, e);
            return 0x11;
        }
        break;
    case 3:
        if (user->getCurCharacInvenW()->delete_item((INVEN_TYPE)0, d, 1, (eItemDelReason)6, 1) != 1)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xaca, 5)(
                "CParty::drop_item , delete_item failed 3, User ch=%d , %d",
                user->getCurCharacNo(), d);
            return 0x11;
        }
        GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)0x16,
                                                     user, (unsigned)price);
        break;
    case 7:
        if (user->getCurCharacInvenW()->delete_item((INVEN_TYPE)3, d, 1, (eItemDelReason)6, 1) != 1)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xaed, 5)(
                "CParty::drop_item , delete_item failed 4, User ch=%d , %d",
                user->getCurCharacNo(), d);
            return 0x11;
        }
        GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)0x16,
                                                     user, (unsigned)price);
        break;
    }
    if (item.m_field1 != 1)
    {
        if ((field.m_item.m_addInfo != 0) && (field.m_item.get_add_info() > 0x7d0))
        {
            // ORIG 086c00cf：第三个可变参数直接读 +0x07 原始字段（-0x115），
            // 不再二次调用 get_add_info()。
            cMyTrace(__PRETTY_FUNCTION__, 0xb00, 4)(
                "war room drop item(%d) stack size over %d add_info(%d)",
                field.m_item.m_addInfo, 0x7d0, field.m_item.m_addInfo2);
        }
    }
    int dropIdx = m_field.DropItem(field);
    if (dropIdx == -1)
        return 0x16;
    PacketGuard packet;
    packet.put_header(0, 0x28);
    // ORIG 086c0198：movzwl %ax,%eax —— get_unique_id 结果按 u16 零扩展后入参。
    packet.put_short((unsigned short)user->get_unique_id());
    packet.put_short(a);
    packet.put_short(b);
    packet.put_short(dropIdx);
    packet.put_int(field.m_item.m_addInfo);
    packet.put_byte(field.m_item.GetItemAttr());
    packet.put_int(field.m_item.get_add_info());
    packet.put_short(field.m_item.m_fieldb);
    packet.put_int(field.m_item.m_fieldd);
    packet.put_byte(field.m_item.m_amp.getAbilityType());
    packet.put_short(field.m_item.m_amp.getAbilityValue());
    packet.put_packet(field.m_item);
    packet.finalize(true);
    SendToRoom(packet);
    packet.clear();
    packet.put_header(1, 0x32);
    packet.put_byte(1);
    packet.put_byte((int)c);
    packet.put_short(d);
    packet.put_int(e);
    packet.finalize(true);
    user->Send(packet);
    return 0;
