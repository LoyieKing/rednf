int CDeathTower::handlePickupItem(CUser* user, int idx, bool a3, char a4)
{
    map_item item;
    CPacketHandler packet;

    if (!m_stage.peekItem(idx, item))
    {
        if (a4 > 3)
            packet.makeCommonError((ENUM_CMDPACKET)0x2e, (ENUM_ERROR)0x51);
        else
            packet.makeCommonError((ENUM_CMDPACKET)0x2e, (ENUM_ERROR)0x15);
        user->Send(packet);
        return 0;
    }

    int memberCnt = m_party->get_member_count();
    if (memberCnt < 1)
    {
        packet.makeCommonError((ENUM_CMDPACKET)0x2e, (ENUM_ERROR)0x15);
        user->Send(packet);
        return 0;
    }

    // ORIG：map_item+0x12 = Inven_Item.m_addInfo（item idx）判定金钱物品。
    if (item.m_item.m_addInfo == 0)
    {
        // Money item
        if (!m_stage.pickupItem(idx, item))
        {
            packet.makeCommonError((ENUM_CMDPACKET)0x2e, (ENUM_ERROR)0x15);
            user->Send(packet);
            return 0;
        }
        int moneyA[4] = {0};
        int moneyB[4] = {0};
        _pickupItemMoney(user, memberCnt, item, moneyA, moneyB);
        packet.makePickupItemHeader(user->get_unique_id(), (unsigned short)idx);
        packet.appendPickupMoney(moneyA, moneyB, m_party);
    }
    else
    {
        // [判据①] ORIG 0x0846425e（else 分支首条指令）`movl $0x0,-0x4c(%ebp)` 即
        // nameBuf 预清，早于 find_item/itemType（跳转落点 0x08464265 直证）；
        // ipgNo 在 genIPGNo 前无任何清零（P171 照抄原码，勿加 ={0}）。
        char ipgNo[0x18];
        int nameBuf = 0;              // ORIG -0x4c：_pickupItem/appendPickupItem 的 char*
        CItem* citem = G_CDataManager()->find_item(item.m_item.m_addInfo);
        if (!citem)
        {
            return 0;
        }

        int itemType = 0;
        if (citem->is_stackable())
        {
            if (citem->GetItemType() == 9)
            {
                itemType = 7;
            }
        }
        else
        {
            if (citem->IsCreatureItem())
            {
                itemType = 7;
            }
            else
            {
                if (citem->IsAvatarItem())
                {
                    itemType = 1;
                }
            }
        }

        CUser* targetUser = _pickupItem(
            user, (char*)&nameBuf, memberCnt, item, a3, a4, citem);
        if (!targetUser)
        {
            packet.makeCommonError((ENUM_CMDPACKET)0x2e, (ENUM_ERROR)0x16);
            user->Send(packet);
            return 0;
        }

        int slot = -1;
        // ORIG 0x08464396..0x084643a7：&item.m_item 每使用点重发 lea+add（无具名局部，P0）。
        if (item.m_item.isAvatarItemType())
        {
            int characNo = targetUser->getCurCharacNo();
            WongWork::CGenUniqueNo::genIPGNo(
                (WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE)5, characNo, ipgNo);
            slot = targetUser->addAvatarItem(
                item.m_item.m_addInfo, 0, (char)item.m_item.m_fieldb,
                ipgNo, (eAvatarItemAddReason)0x10);
        }
        else
        {
            CInventory* inven = (CInventory*)targetUser->getCurCharacInvenW();
            slot = inven->insertItemIntoInventory(item.m_item, (eItemAddReason)4, true, true);
        }

        if (slot == -1)
        {
            packet.makeCommonError((ENUM_CMDPACKET)0x2e, (ENUM_ERROR)4);
            user->Send(packet);
            return 0;
        }

        if (citem->IsRandomOption())
        {
            statistc_proxy::add(
                targetUser, "log_random_option", "pick_cnt", 1, "%d,'%s'",
                targetUser->get_charac_10_level_section(),
                GlobalData::s_systemTime_.getCurDate());
        }

        if (!m_stage.pickupItem(idx, item))
        {
            packet.makeCommonError((ENUM_CMDPACKET)0x2e, (ENUM_ERROR)0x15);
            user->Send(packet);
            return 0;
        }

        packet.makePickupItemHeader(user->get_unique_id(), (unsigned short)idx);
        packet.appendPickupItem((char*)&nameBuf, targetUser->get_unique_id(),
                                slot, itemType);
    }

    m_party->send_to_party(packet);
    return 1;
