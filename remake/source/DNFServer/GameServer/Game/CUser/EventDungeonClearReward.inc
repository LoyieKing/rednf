void CUser::EventDungeonClearReward(std::vector<std::pair<int, int> > const& items)
{
    const int count = items.size();
    for (int i = 0; i < count; ++i)
    {
        int itemId = items[i].first;
        int itemCnt = items[i].second;
        CItem* data = G_CDataManager()->find_item(itemId);
        if (data == NULL)
            break;
        Inven_Item item;
        int slot = getCurCharacInvenR()->GetInvenData(itemId, item);
        if (slot < 0)
        {
            data->make_item(item);
            // b28d-G3C 2026-09-10（语句序 = ORIG 0x0868 序）：ORIG 先写 +0x7 再写
            //   +0x2 —— `mov -0x14(%ebp),%eax; mov %eax,-0x56(%ebp)`（m_addInfo2
            //   == itemCnt，对象基址 -0x5d ⇒ +0x7）在后一条 `mov -0x18(%ebp),%eax;
            //   mov %eax,-0x5b(%ebp)`（m_addInfo == itemId ⇒ +0x2）**之前**；
            //   我方原序（addInfo 先）与之相反。
            item.m_addInfo2 = itemCnt;
            item.m_addInfo = itemId;
            slot = getCurCharacInvenW()->insertItemIntoInventory(
                item, eItemAddReason_0xd, true, false);
        }
        else
        {
            // b28d-G3C 2026-09-10（P183 去具名）：ORIG 0x0868 段 `call get_add_info`
            //   结果经 %eax **直传**实参位（`mov %eax,0xc(%esp)`，无 store/load），
            //   我方原 `int addInfo` 具名槽多 2 条（-0xc 槽往返）且独占 4 字节。
            m_historyLog.ItemAdd(INVEN_TYPE_EQUIP, itemCnt, item.get_add_info(),
                                 item, eItemAddReason_0xd);
            item.m_addInfo2 += itemCnt;
            getCurCharacInvenW()->update_item(INVEN_TYPE_EQUIP, slot, item);
        }
        if (slot >= 0)
        {
            SendUpdateItemList(eSendTarget_1, ENUM_ITEMSPACE_0, slot);
        }
    }
