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

    char t = (char)type;
    if (t == 2)
    {
        src = user->getCurCharacCargoR()->get_cargo_slot(slot);
    }
    else if (t == 0)
    {
        if (slot == 0)
        {
            // 直接构造"金币掉落"项：m_count=0、ITEM_TYPE=2、addInfo=0
            mi.m_count = 0;
            Inven_Item& dst = mi.m_item;
            dst.m_field1 = 2;
            dst.m_addInfo = 0;
            dst.set_add_info(count);
            dst.m_fieldb = 0;
            dst.ResetItemAttr();
        }
        else
        {
            src = user->getCurCharacInvenR()->GetInvenSlot(1, slot);
        }
    }
    else if (t == 3)
    {
        src = user->getCurCharacInvenR()->GetInvenSlot(0, slot);
    }
    else if (t == 7)
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

        Inven_Item& dst = mi.m_item;
        mi.m_count = 0;
        dst.m_field1 = src.m_field1;
        dst.m_addInfo = src.m_addInfo;
        if (src.isEquipableItemType())
            dst.set_add_info(src.m_addInfo2);
        else
            dst.set_add_info(count);
        dst.SetItemAttr(src.GetItemAttr());
        dst.m_fieldb = src.m_fieldb;
        dst.m_field0 = src.m_field0;
        dst.m_addInfo2 = src.m_addInfo2;
        dst.m_amp = src.m_amp;                       // +0x11..0x1e 整段
        dst.m_random = src.m_random;                 // +0x1f..0x32 整段
        dst.m_upgradeSep.SetUpgradeSeparate(src.m_upgradeSep.GetUpgradeSeparate());

        G_Store()->GetSellItemPrice(src, item, (short)count, false, sellPrice);
    }

    if (t == 2)
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
    else if (t == 0)
    {
        if (slot == 0)
        {
            ServiceRestrictManager* srm =
                ARAD::Singleton<ServiceRestrictManager>::Get();
            if (srm->isRestricted(user, (RestrictCategory::Enum)2, 1))
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
    else if (t == 3)
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
    else if (t == 7)
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
    packet.put_byte((int)t);
    packet.put_short(slot);
    packet.put_int(count);
    packet.finalize(true);
    user->Send(packet);
    return 0;
