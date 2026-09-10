int Store::user_buy_item(CUser* user, int itemId, int count)
{
    if (user->CheckInTrade())
    {
        return 0x13;
    }

    CItem* pItem = G_CDataManager()->m_itemList->find_item(itemId);
    if (pItem == NULL)
    {
        return 0x11;
    }

    int totalPrice;
    if (pItem->is_stackable())
    {
        totalPrice = pItem->get_price() * count;
    }
    else
    {
        totalPrice = pItem->get_price();   // ORIG +0x71 分支：不可堆叠同样取价（不乘 count）
    }
    if (user->getCurCharacInvenR()->get_money() < totalPrice)
    {
        return 0xa;
    }

    Inven_Item item;
    item.m_addInfo = itemId;

    if (pItem->is_stackable())
    {
        // 可堆叠物品：ORIG 0x08618b0e 三次独立虚调 GetItemType()（P48）。
        if (pItem->GetItemType() == 1)
            item.m_field1 = 3;
        if (pItem->GetItemType() == 2)
            item.m_field1 = 10;
        if (pItem->GetItemType() == 8)
            item.m_field1 = 4;
        item.set_add_info(count);
        if (count > 0x3e7)
        {
            // ORIG 0x08618b89：ctor 前先读 item.m_addInfo 入 %ebx 跨 ctor 保活
            // ⇒ 全表达式临时链（P227）。
            cMyTrace("int Store::user_buy_item(CUser*, int, int)", 0x3d, 5)
                ("Store::user_buy_item,  ITEM #%d, %d", item.m_addInfo, count);
        }
        item.m_fieldb = 0;
        item.ResetItemAttr();
    }
    else
    {
        // 装备类
        CEquipItem* pEquip = static_cast<CEquipItem*>(pItem);
        if (pEquip->m_itemType == 0xb)
        {
            return 0x17;
        }
        if (pEquip->GetAttachType() == 3)
        {
            item.m_field0 = 1;
        }
        item.m_field1 = 1;
        item.set_add_info(count);
        item.m_fieldb = (unsigned short)pEquip->get_endurance();
        item.ResetItemAttr();
    }

    // ORIG 0x08618c52 压栈 (0x44=reason 0, 0x48=1, 0x4c=0) ⇒ (true, false)。
    int slot = user->getCurCharacInvenW()->insertItemIntoInventory(
        item, eItemAddReason_0, true, false);
    if (slot == -1)
    {
        return 4;
    }

    if (!user->getCurCharacInvenW()->use_money(totalPrice, eMoneySubReason_0, true))
    {
        return 0xa;
    }

    // 统计
    int sellPrice = 0;
    // ORIG 0x08618d38 直读 item+0x07（m_addInfo2）并 cwtl ⇒ (short)item.m_addInfo2。
    GetSellItemPrice(item, pItem, (short)item.m_addInfo2, false, sellPrice);
    GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)6, user, sellPrice);
    GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)0xd, user, totalPrice);

    // 发送结果包
    PacketGuard packet;
    packet.put_header(1, 0x15);
    packet.put_byte(1);
    packet.put_int(user->getCurCharacInvenR()->get_money());
    packet.put_short(slot);
    packet.put_int(itemId);
    packet.put_int(count);
    packet.finalize(true);
    user->Send(packet);

    return 0;
