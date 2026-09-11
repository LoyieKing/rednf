// b28d-solo-Dispatchers verdict=DIFF(routed) [N89] ORIG 0x08217c06 orig/ours=579/538 ndiff-blocks=91
//   ORIG 段内 std::/__gnu_cxx out-of-line 内联实例 call 3 个，4.4 libstdc++ 全内联不可复现
//   首差: mov    %eax,-0x24(%ebp) | cmpl   $0x0,-0x24(%ebp) | mov    -0x24(%ebp),%eax
int Dispatcher_DisJointAvatar::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08217c06: state 门
    if (user->get_state() != 3)
        return 0;

    // +0x038: isEnableAvatarSocketAction 门
    int avatarSocketAction = user->isEnableAvatarSocketAction();
    if (avatarSocketAction != 0)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0xcd),
                                 static_cast<unsigned char>(avatarSocketAction));
        return 0;
    }

    // +0x065: ServiceRestrictManager::isRestricted(user,1,0x14) 门
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, static_cast<RestrictCategory::Enum>(1), 0x14))
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0xcd), 0xd1);
        return 0;
    }

    // +0x0b2: local_3e=ushort slot, local_44=ulong itemIdx
    unsigned short slot = 0;
    unsigned long itemIdx = 0;
    if (!packet.get_short(slot))
    {
        return LineFunc(0xcaa7, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_item_idx(itemIdx))
    {
        return LineFunc(0xcaa8, __PRETTY_FUNCTION__, 0, 0);
    }

    // +0x147: CheckItemLock(2, slot)
    if (user->CheckItemLock(2, static_cast<int>(slot)))
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0xcd), 0xd5);
        return 0;
    }

    // +0x18e: Inven_Item item; CItem* pItem=0; vector<pair<int,int>> results
    Inven_Item item;
    CItem* pItem = static_cast<CItem*>(0);
    std::vector<std::pair<int, int> > results;

    // ORIG 0x08217eaa..0x08218239：主链全部 throw 落在同一 try 内，
    // catch (ENUM_ERROR) 发 0xcd 错误包后返回 0（ORIG 0x0821824a 段）。
    try
    {
    // +0x1ae: GetInvenSlot(2, slot) → item
    item = user->getCurCharacInvenR()->GetInvenSlot(2, static_cast<int>(slot));

    // +0x29d: item.m_addInfo(+0x02) == itemIdx
    if (item.m_addInfo != static_cast<int>(itemIdx))
    {
        throw static_cast<ENUM_ERROR>(0x11);
    }

    // +0x2d1: G_CDataManager()->find_item(item.m_addInfo)
    pItem = G_CDataManager()->find_item(item.m_addInfo);
    if (pItem == static_cast<CItem*>(0))
    {
        throw static_cast<ENUM_ERROR>(0x11);
    }

    // +0x322: get_grade()==0 || get_grade()>3 → error 0x13
    // ORIG 0x08217f23/0x08217f32：get_grade 两处各自重取（P183）。
    if (pItem->get_grade() == 0 || pItem->get_grade() > 3)
    {
        throw static_cast<ENUM_ERROR>(0x13);
    }

    // +0x37a: 虚调用 vtbl+0x24 = isEnableAvatarFunc(2)
    if (!(pItem->isEnableAvatarFunc(static_cast<ENUM_AVATAR_FUNC_FILTER>(2))))
    {
        throw static_cast<ENUM_ERROR>(0x17);
    }

    // +0x3bf: GetAvatarItemMgrR()->GetExpireDate(item.m_addInfo2) != 0 → error 0x17
    if (user->getCurCharacInvenW()->GetAvatarItemMgrR()->GetExpireDate(item.m_addInfo2) != 0)
    {
        throw static_cast<ENUM_ERROR>(0x17);
    }

    // +0x419: item.m_upgradeSep(+0x33).IsTradeRestriction() → error 0x17
    if (item.m_upgradeSep.IsTradeRestriction())
    {
        throw static_cast<ENUM_ERROR>(0x17);
    }

    // +0x45f: getJewelSocketData(item.m_addInfo2) → _selectJewelTypeIndex
    // ORIG 0x0821806b：getJewelSocketData 经 getCurCharacInvenW()（p167 净差归零）。
    stAvatarEmblemInfo_t* jewelSocketData =
        reinterpret_cast<stAvatarEmblemInfo_t*>(
            const_cast<char*>(user->getCurCharacInvenW()->GetAvatarItemMgrR()->getJewelSocketData(item.m_addInfo2)));
    int jewelTypeIndex = _selectJewelTypeIndex(jewelSocketData);

    // +0x49c: pItem+0x234(itemType) == 9 → jewelTypeIndex = -1
    if (reinterpret_cast<CEquipItem*>(pItem)->m_itemType == 9)
    {
        jewelTypeIndex = -1;
    }

    // +0x4d3: _getResultItems(user, pItem, jewelTypeIndex, results)
    int getResultError = _getResultItems(user, pItem, jewelTypeIndex, results);
    if (getResultError != 0)
    {
        throw static_cast<ENUM_ERROR>(getResultError);
    }

    // +0x50c: results.size(); check_empty_count(ITEM_TYPE 9, size)
    size_t resultCount = results.size();
    if (!(user->getCurCharacInvenR()->check_empty_count(
              static_cast<Inven_Item::ITEM_TYPE>(9), static_cast<int>(resultCount))))
    {
        throw static_cast<ENUM_ERROR>(4);
    }

    // +0x585: delete_item(INVEN 2, slot, 1, eItemDelReason 6, true)
    if (!(user->getCurCharacInvenW()->delete_item(
              static_cast<INVEN_TYPE>(2), static_cast<int>(slot), 1,
              static_cast<eItemDelReason>(6), true)))
    {
        throw static_cast<ENUM_ERROR>(0x16);
    }

    // +0x5fc: GetAvatarItemMgrW()->UnRegistItem(item.m_addInfo2)
    if (!(user->getCurCharacInvenW()->GetAvatarItemMgrW()->UnRegistItem(item.m_addInfo2)))
    {
        throw static_cast<ENUM_ERROR>(0x16);
    }
    }
    catch (ENUM_ERROR e)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0xcd),
                                 static_cast<unsigned char>(e));
        return 0;
    }

    // +0x6ca: SendAvatarEvent(0x36, uid, 5, uid+10, 0)
    user->getCurCharacInvenW()->SendAvatarEvent(
        0x36, item.m_addInfo2, 5, item.m_addInfo2 + 10, 0);

    // +0x6f4: DB_UpdateAvatarHistory::makeRequest(characNo, uid, "")
    DB_UpdateAvatarHistory::makeRequest(
        static_cast<unsigned int>(user->getCurCharacNo()), item.m_addInfo2, const_cast<char*>(""));

    // +0x70a: vector<int> updates; insert_event_items(results, updates, 0, 10, 0, "", "")
    std::vector<int> updates;
    int insertResult = user->getCurCharacInvenW()->insert_event_items(
        results, updates, static_cast<eMoneyAddReason>(0), static_cast<eItemAddReason>(10),
        false, "", "");

    // +0x751: PacketGuard pg; put_header(1,0xcd); put_byte(1); put_short(slot); put_short(insertResult)
    PacketGuard pg;
    pg.put_header(1, 0xcd);
    pg.put_byte(1);
    pg.put_short(static_cast<int>(slot));
    pg.put_short(insertResult);

    // +0x7d8: for (it = updates.begin(); it != updates.end();)
    //          { put_short(*it++); put_item_idx(*it++); put_int(*it++); }
    for (std::vector<int>::const_iterator it = updates.begin(); it != updates.end();)
    {
        pg.put_short(*it++);
        pg.put_item_idx(static_cast<unsigned long>(*it++));
        pg.put_int(*it++);
    }

    // +0x8b2: finalize(true); Send(pg)
    pg.finalize(true);
    user->Send(pg);

    // +0x8e4/+0x8f6: SendDisjointAvatarInfo(pItem); SendCreateEmblemInfo(results)
    SendDisjointAvatarInfo(pItem);
    SendCreateEmblemInfo(results);

    return 0;
}