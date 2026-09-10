int CExpertJobMgr::OnEnchantByBead(CUser* user, int param2, int beadSlot,
                                   int param4, int ringSlot)
{
    (void)param2;
    (void)param4;

    Inven_Item* beadRef = user->getCurCharacInvenW()->GetInvenRef(1, beadSlot);
    if (beadRef == 0)
    {
        return 0x11;
    }
    Inven_Item* ringRef = user->getCurCharacInvenW()->GetInvenRef(1, ringSlot);
    if (ringRef == 0)
    {
        return 0x11;
    }

    // 租赁两连查（ORIG 0x82702dc 实证：查的是 ringRef 的 m_addInfo，kind=1/2）
    if (GlobalData::s_rentalSystem->CheckRentableItem(
            1, (unsigned int)ringRef->m_addInfo))
    {
        return 0x11;
    }
    if (GlobalData::s_rentalSystem->CheckRentableItem(
            2, (unsigned int)ringRef->m_addInfo))
    {
        return 0x11;
    }

    // P48：ORIG G_CDataManager() 每用点 fresh（无 dm 槽）
    CItem* beadItem = G_CDataManager()->find_item(beadRef->m_addInfo);
    if (beadItem == 0)
    {
        return 0x11;
    }
    CItem* ringItem = G_CDataManager()->find_item(ringRef->m_addInfo);
    if (ringItem == 0)
    {
        return 0x11;
    }
    if (!beadItem->is_stackable())
    {
        return 0x11;
    }

    // 怪物卡主表链（ORIG 序）：cardId→masterItem→EnableWorld 校验
    int cardId = ((CStackableItem*)beadItem)->GetMonsterCardId();
    CItem* masterItem = G_CDataManager()->find_item(cardId);
    if (masterItem == 0)
    {
        return 0x11;
    }
    if (EnableWorld(ringItem, beadItem) != 1)
    {
        return 0x13;
    }
    // ORIG 0x849eeb3：ringItem->GetItemType() 结果作实参调
    // masterItem->getUsableItemType()（虚槽 vptr+0xc / vptr+0x20），失败返回 0x17。
    if (!masterItem->getUsableItemType(ringItem->GetItemType()))
    {
        return 0x17;
    }

    bool deleted =
        user->getCurCharacInvenW()->delete_item((INVEN_TYPE)1, beadSlot, 1,
                                                (eItemDelReason)8, true);
    if (!deleted)
    {
        // b28d-solo（P118/P139/P227）：ORIG 0x0849ef5f ctor 实参为
        // __PRETTY_FUNCTION__ 局部静态符号，且 getCurCharacNo() 在 ctor **前**
        // 求值并入 %ebx 跨 ctor 保活（全表达式临时链）⇒ 不用具名 trace 对象。
        cMyTrace(__PRETTY_FUNCTION__, 0x6cd, 5)(
            "CExpertJobMgr::OnEnchantByBead, getCurCharacInvenW()->delete_item "
            "failed , %d %d", user->getCurCharacNo(), beadSlot);
        return 0x11;
    }

    int savedCardIdx = ringRef->m_fieldd;
    // b28d-solo：ORIG 0x0849f076 先写 ringRef->m_fieldd = masterItem->get_index()
    //（第一次调用），无 replacedCardIdx 具名局部（P0 无槽往返）。
    ringRef->m_fieldd = masterItem->get_index();

    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0xe);
    packet.put_byte(0);
    packet.put_short(2);
    user->getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, ringSlot, packet);
    user->getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, beadSlot, packet);
    packet.finalize(true);
    user->Send(packet);

    // 统计（ORIG 0x860f940/0x860f9d4：increaseItemCount((ENUM_EXPERT_JOB_TYPE)1,
    // masterIndex, 1)）——ORIG 0x0849f076 为**第二次独立** get_index 调用（P48）。
    GetInstanceExpertJobStatistic()->increaseItemCount(
        (ENUM_EXPERT_JOB_TYPE)1, (unsigned long)masterItem->get_index(), 1);

    // 历史（ORIG 0x8684d74：EnchantItem(ringRef->m_addInfo, savedCardIdx,
    // ringRef->m_fieldd 重读, true, acc_name, characName)）
    user->m_historyLog.EnchantItem(ringRef->m_addInfo, savedCardIdx,
                                   ringRef->m_fieldd, true, user->get_acc_name(),
                                   user->getCurCharacName());

    packet.clear();
    packet.put_header(1, 0x113);
    packet.put_byte(1);
    packet.finalize(true);
    user->Send(packet);
    return 0;
