int CParty::drop_item(CUser* user, int a2, int a3, char type, int slot,
                      int count)
{
    // b28d-49 判词：N 封顶（N82 帧/槽 + N7 寄存器）。P167 CLEAR。@0x85a63f4
    if (count < 0)
    {
        LogManager::logFormat(1, "party.cpp",
                              __PRETTY_FUNCTION__,
                              0x1a0c, "user - accid[%s] %s : count<0",
                              NumberToString(user->get_acc_id(), 0),
                              user->getCurCharacName());
        return 0x11;
    }
    if (user->CheckInTrade())
        return 0x13;

    map_item mi;                                   // -0x12c
    Inven_Item src;                                // -0xd5

    // [G7-sub] P0 槽律：ORIG 085a64a8 起 `movsbl -0x13c(%ebp),%eax` 直接用**参数槽**
    // 比较（0x20(%ebp) 另作 count），无 `char t` 具名槽（我方原 -0x31，帧抬升源之一）
    // ⇒ 删具名、全部回实参 `type`；ORIG 为有符号 movsbl 比较（参数即 char）。
    if (type == 2)
    {
        src = user->getCurCharacCargoR()->get_cargo_slot(slot);
    }
    else if (type == 0)
    {
        if (slot == 0)
        {
            // 直接构造"金币掉落"项：m_count=0、ITEM_TYPE=2、addInfo=0
            mi.m_count = 0;
            // [G7-sub] P183/P0：ORIG 085a64e3/085a651d/085a6530 每处 `lea -0x12c(%ebp); add $0x10`
            // 重取址，无 `Inven_Item& dst` 引用槽（我方原 -0x2c）⇒ 直用 mi.m_item。
            mi.m_item.m_field1 = 2;
            mi.m_item.m_addInfo = 0;
            mi.m_item.set_add_info(count);
            mi.m_item.m_fieldb = 0;
            mi.m_item.ResetItemAttr();
        }
        else
        {
            src = user->getCurCharacInvenR()->GetInvenSlot(1, slot);
        }
    }
    else if (type == 3)
    {
        src = user->getCurCharacInvenR()->GetInvenSlot(0, slot);
    }
    else if (type == 7)
    {
        src = user->getCurCharacInvenR()->GetInvenSlot(3, slot);
        if (src.m_field1 == 5)
            return 0x17;
    }

    int sellPrice = 0;                             // -0x8c
    if (src.m_addInfo != 0)
    {
        CItem* item = G_CDataManager()->find_item(src.m_addInfo);   // -0x24
        if (item == 0)
        {
            LogManager::logFormat(
                1, "party.cpp",
                __PRETTY_FUNCTION__,
                0x1a48, "[CParty::drop_item] Null Item");
            return -1;
        }

        int attach = item->GetAttachType();        // -0x20
        switch (attach)
        {
        case 1:
        case 2:
        case 5:
            return 0x17;
        case 3:
            if (src.m_field0 != 1)
                return 0x17;
            break;
        default:
            break;
        }

        if (src.m_upgradeSep.IsTradeRestriction())
            return 0x17;

        if (item->get_rarity() > 2)
            return 0xd;

        // ORIG drop_item.md 085a6a1f/085a6a2d `mov 0x234(%eax)`（int dword）。
        // +0x234 = CEquipItem::m_itemType（CEquipItem.h:210，单继承 CItem 偏移 0）。
        if (src.isEquipableItemType() &&
            (static_cast<const CEquipItem*>(item)->m_itemType == 0xb ||
             static_cast<const CEquipItem*>(item)->m_itemType < 10))
            return 0x17;

        mi.m_count = 0;
        // [G7-sub] P183/P0：ORIG 085a6a5e 起每个字段访问都 `lea -0x12c(%ebp); add $0x10`
        // 重取址（085a6a7c/085a6a9a/085a6ab5/085a6ad0…），无 dst 引用槽（我方原 -0x20）
        // ⇒ 直用 mi.m_item。
        mi.m_item.m_field1 = src.m_field1;
        mi.m_item.m_addInfo = src.m_addInfo;
        if (src.isEquipableItemType())
            mi.m_item.set_add_info(src.m_addInfo2);
        else
            mi.m_item.set_add_info(count);
        mi.m_item.SetItemAttr(src.GetItemAttr());
        mi.m_item.m_fieldb = src.m_fieldb;
        mi.m_item.m_field0 = src.m_field0;
        mi.m_item.m_addInfo2 = src.m_addInfo2;
        mi.m_item.m_amp = src.m_amp;                 // +0x11..0x1e 整段
        mi.m_item.m_random = src.m_random;           // +0x1f..0x32 整段
        mi.m_item.m_upgradeSep.SetUpgradeSeparate(
            src.m_upgradeSep.GetUpgradeSeparate());

        G_Store()->GetSellItemPrice(src, item, (short)count, false, sellPrice);
    }

    if (type == 2)
    {
        if (!user->getCurCharacCargoW()->delete_item(slot, count, (eItemDelReason)6))
        {
            cMyTrace trace(__PRETTY_FUNCTION__,
                           0x1ac6, 5);
            trace("CParty::drop_item , delete_item failed 2, User ch =%d , %d %d",
                  user->getCurCharacNo(), slot, count);
            return 0x11;
        }
        GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)0x16,
                                                       user, (unsigned int)count);
    }
    else if (type == 0)
    {
        if (slot == 0)
        {
            // [G7-sub] P0 槽律 + 语义修正（ORIG 直证）：ORIG 085a6ce8 `call Singleton::Get`
            // 结果**无栈槽**（直进 (%esp)），我方原具名 srm 落 -0x1c；且实参装载序
            // 085a6cf0 `movl $0x2,0xc(%esp)` / 085a6cf7 `movl $0x1,0x8(%esp)`
            // ⇒ 第3参=1、第4参=2（**`isRestricted(user, (RestrictCategory::Enum)1, 2)`**），
            // 我方原写 (Enum)2, 1 为**实参颠倒**（值语义不同）⇒ 一并修正。
            if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
                    user, (RestrictCategory::Enum)1, 2))
                return 0xd1;
            if (!user->getCurCharacInvenW()->use_money(count,
                                                       (eMoneySubReason)6, true))
            {
                cMyTrace trace(__PRETTY_FUNCTION__,
                               0x1aa6, 5);
                trace("CParty::drop_item , use_money failed, User ch =%d , %d %d",
                      user->getCurCharacNo(), slot, count);
                return 0x11;
            }
            GetInstanceValueStatistic()->AddValueStatistic(
                (VALUE_STATISTIC_FIELD)0x17, user, (unsigned int)count);
        }
        else
        {
            if (!user->getCurCharacInvenW()->delete_item(
                    (INVEN_TYPE)1, slot, count, (eItemDelReason)6, true))
            {
                cMyTrace trace(__PRETTY_FUNCTION__,
                               0x1ab5, 5);
                trace("CParty::drop_item , delete_item failed, User ch =%d , %d %d",
                      user->getCurCharacNo(), slot, count);
                return 0x11;
            }
            GetInstanceValueStatistic()->AddValueStatistic(
                (VALUE_STATISTIC_FIELD)0x16, user, (unsigned int)count);
        }
    }
    else if (type == 3)
    {
        if (!user->getCurCharacInvenW()->delete_item(
                (INVEN_TYPE)0, slot, count, (eItemDelReason)6, true))
        {
            cMyTrace trace(__PRETTY_FUNCTION__,
                           0x1ae8, 5);
            trace("CParty::drop_item , delete_item failed 3, User ch =%d , %d",
                  user->getCurCharacNo(), slot);
            return 0x11;
        }
        GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)0x16,
                                                       user, (unsigned int)count);
    }
    else if (type == 7)
    {
        if (!user->getCurCharacInvenW()->delete_item(
                (INVEN_TYPE)3, slot, count, (eItemDelReason)6, true))
        {
            cMyTrace trace(__PRETTY_FUNCTION__,
                           0x1af9, 5);
            trace("CParty::drop_item , delete_item failed 4, User ch =%d , %d",
                  user->getCurCharacNo(), slot);
            return 0x11;
        }
        GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)0x16,
                                                       user, (unsigned int)count);
    }

    if (!src.isEquipableItemType() && mi.m_item.m_addInfo != 0 &&
        mi.m_item.get_add_info() > 0x7d0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__,
                       0x1b0c, 4);
        trace("drop item(%d) stack size over %d add_info(%d)", mi.m_item.m_addInfo,
              0x7d0, mi.m_item.m_addInfo2);
    }

    int dropped = reinterpret_cast<CBattle_Field*>(&m_battleField)
                      ->drop_item(mi);           // ORIG CBattle_Field::drop_item(map_item)
    if (dropped == -1)
        return 0x16;

    PacketGuard packet;
    packet.put_header(0, 0x28);
    packet.put_short(user->get_unique_id() & 0xffff);
    packet.put_short(a2);
    packet.put_short(a3);
    packet.put_short(dropped);
    packet.put_int(mi.m_item.m_addInfo);
    packet.put_byte(mi.m_item.GetItemAttr());
    packet.put_int(mi.m_item.get_add_info());
    packet.put_short(mi.m_item.m_fieldb);
    packet.put_int(mi.m_item.m_fieldd);
    packet.put_byte(mi.m_item.m_amp.getAbilityType());
    packet.put_short(mi.m_item.m_amp.getAbilityValue());
    packet.put_packet(mi.m_item);
    packet.put_byte(mi.m_count == 0 ? 1 : 0);
    packet.finalize(true);
    send_to_party(packet);

    packet.clear();
    packet.put_header(1, 0x32);
    packet.put_byte(1);
    packet.put_byte((int)type);
    packet.put_short(slot);
    packet.put_int(count);
    packet.finalize(true);
    user->Send(packet);
    return 0;
