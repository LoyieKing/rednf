void CUser::RefillItemByScript(std::vector<std::pair<int, int> >& list)
{
    // ORIG 0x086890be：经 const 指针走 const operator[]（ix 常量符号）
    const std::vector<stRefillItemInfo::RefillItem>* refill =
        G_CDataManager()->GetRefillItemInfo();
    const int nowSec = GlobalData::s_systemTime_.getCurSec();

    for (unsigned int i = 0; i < refill->size(); ++i)
    {
        const stRefillItemInfo::RefillItem& entry =
            refill->operator[](i);
        if (nowSec >= entry.m_endSec)      // ORIG：endSec<=curSec 跳过（jle）
            continue;
        if (entry.m_count == 0)
        {
            // std::make_pair<int const&, int>(itemId, 0) → push_back
            list.push_back(std::make_pair(entry.m_itemId, 0));
            continue;
        }

        CItem* def = G_CDataManager()->find_item(entry.m_itemId);
        if (def == 0)
            continue;

        Inven_Item item;                   // ORIG Inven_Item::Inven_Item() W 0x80cb854
        int slot = getCurCharacInvenR()->GetInvenData(entry.m_itemId, item);
        if (slot < 0)
        {
            def->make_item(item);              // ORIG *(*(def)+8)(def,&item)：vtable 槽 2
            item.m_addInfo2 = entry.m_count;   // +0x07 数量（ORIG -0x62 槽，先写）
            item.m_addInfo = entry.m_itemId;   // +0x02 物品索引（ORIG -0x67 槽，后写）
            slot = getCurCharacInvenW()->insertItemIntoInventory(
                item, eItemAddReason_0xd, true, false);
        }
        else
        {
            m_historyLog.ItemAdd((INVEN_TYPE)1, item.m_addInfo2, item.m_addInfo,
                                 item, eItemAddReason_0xd);
            item.m_addInfo2 = entry.m_count;
            getCurCharacInvenW()->update_item((INVEN_TYPE)1, slot, item);
        }
        if (slot >= 0)
            SendUpdateItemList(eSendTarget_1, (ENUM_ITEMSPACE)0, slot);
    }
