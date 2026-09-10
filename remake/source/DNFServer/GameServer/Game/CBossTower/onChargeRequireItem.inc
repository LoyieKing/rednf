int CBossTower::onChargeRequireItem(CUser* user)
{
    if (user == 0)
        return 0;
    if (m_party->m_battleField.m_bloodState == 1)
        return 1;

    if (m_dungeon->m_entryRequiredItem.m_enableFlag != 0)
    {
        CItem* itemData = 0;                        // ORIG 0x8145316 `movl $0x0,-0x10` 先于 item ctor（P191/P198③ 双 init）
        Inven_Item item;                               // ebp-0x4d

        itemData = G_CDataManager()->find_item(
            m_dungeon->m_entryRequiredItem.m_itemIdx);
        if (itemData == 0)
            return 0;

        itemData->make_item(item);                     // 虚槽 vptr+0x08
        item.m_addInfo2 = m_dungeon->m_entryRequiredItem.m_itemCount;  // item+0x07
        item.m_addInfo = m_dungeon->m_entryRequiredItem.m_itemIdx;   // item+0x02（ORIG 0x8145341 先 +0x07 后 +0x02）

        int slot = -1;                                 // ORIG 0x8145337 `movl $0xffffffff,-0xc` 预置

        slot = user->getCurCharacInvenW()->insertItemIntoInventory(
            item, (eItemAddReason)4, true, true);
        if (slot >= 0)
        {
            user->SendUpdateItemList(CUser::eSendTarget_1,
                                     (ENUM_ITEMSPACE)0, slot);
        }
    }
    return 1;
