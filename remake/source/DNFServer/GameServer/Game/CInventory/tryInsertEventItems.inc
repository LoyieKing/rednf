int CInventory::tryInsertEventItems(const std::vector<std::pair<int, int> >& items)
{
    // b28d-solo-CInventory 复修：ORIG 297 条。旧判词漏掉三处真结构：
    // ① entry/inven 在循环外声明（ORIG pair::C1Ev@+0x02c、Inven_Item::C1Ev@+0x063 各一次）；
    // ② inven.m_addInfo=itemIdx 在 is_stackable 之前（ORIG +0x144 mov -0x14→-0x83）；
    // ③ make_item 在三个分支内各调一次（ORIG +0x166/+0x24c/+0x321），非循环外一次；
    // ④ insertItem/Avatar 返回值先落具名槽 -0x10 再比较（ORIG mov %eax,-0x10）。
    int insertCount = 0;
    CItem* pItem;
    int count;
    int itemIdx;
    int slot;
    if (items.size() == 0)
    {
        return insertCount;   // P0：ORIG 0x08506782 `mov -0x20(%ebp),%eax`（返回具名局部，非字面量 0）
    }
    std::pair<int, int> entry;
    std::vector<std::pair<int, int> >::const_iterator it;
    it = items.begin();
    Inven_Item inven;
    while (it != items.end())
    {
        entry = std::pair<int, int>(*it);
        itemIdx = entry.first;
        count = entry.second;
        if (itemIdx == 0)
        {
            insertCount++;
            it++;
            continue;
        }
        if (itemIdx == 1)
        {
            insertCount++;
            it++;
            continue;
        }
        pItem = G_CDataManager()->find_item(itemIdx);
        if (pItem == 0)
        {
            it++;
            return insertCount;
        }
        inven.m_addInfo = itemIdx;
        if (pItem->is_stackable())
        {
            inven.m_addInfo2 = count;
            pItem->make_item(inven);
            slot = insertItemIntoInventory(inven, (eItemAddReason)8, false, true);
            // [直证 b28d-tu-CInventory] ORIG 该处为 `cmpl $0xffffffff,-0x10; je <return>`
            // （直跳，**非** sete 物化）⇒ 保持 if 形。
            if (slot == -1)
            {
                goto fail_return;
            }
            insertCount++;
        }
        else
        {
            if (pItem->IsAvatarItem())
            {
                pItem->make_item(inven);
                slot = insertAvatarIntoInventory(inven, eAvatarItemAddReason_0, false);
                // [P225 复现形·定位直证] ORIG 三处 slot==-1 检查中**中间那处**
                // （IsAvatarItem 分支，0x0850? index 204）为 `cmpl $0xffffffff,-0x10;
                // sete %al; test %al,%al; jne` ⇒ 该处用 while 形（前后两处为直跳 je）。
                while (slot == -1)
                {
                    goto fail_return;
                }
                insertCount++;
            }
            else
            {
                for (int i = 0; i < count; i++)
                {
                    pItem->make_item(inven);
                    slot = insertItemIntoInventory(inven, (eItemAddReason)8, false, true);
                    // [直证] ORIG 该处（index 259）为 `cmpl; je` 直跳 ⇒ 保持 if 形。
                    if (slot == -1)
                    {
                        break;
                    }
                    insertCount++;
                }
            }
        }
        it++;
    }
fail_return:
    return insertCount;
