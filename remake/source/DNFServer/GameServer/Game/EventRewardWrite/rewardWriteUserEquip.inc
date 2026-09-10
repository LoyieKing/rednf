bool eventReward::rewardWriteUserEquip(Inven_Item* item, rewardKey key, int count)
{
    if (this->m_map0.empty())
        return 0;
    std::map<rewardKey, std::map<int, std::vector<itemData> > >::iterator it =
        this->m_map0.find(key);
    if (it == this->m_map0.end())
        return 0;
    // [L10-2] 同 rewardWriteUserInven：ORIG 单次解引用（消 p167 EXTRA const ptEv）。
    std::map<int, std::vector<itemData> >& subMap = it->second;
    std::map<int, std::vector<itemData> >::iterator fit = subMap.find(count);
    if (fit == subMap.end())
        return 0;
    std::vector<itemData>& vec = fit->second;
    if (vec.empty())
        return 0;
    for (std::vector<itemData>::iterator vit = vec.begin(); vit != vec.end(); vit++)
    {
                // ORIG +0x11a..+0x134：*vit 取 m_itemIdx（ebx 跨 G_CDataManager 调用存活）
        // 直接嵌套实参；citem 判空后 +0x157 再 *vit 一次写 inven.m_addInfo。
        CItem* citem = G_CDataManager()->find_item(vit->m_itemIdx);
        if (citem == 0)
            return 0;
                Inven_Item inven;
        inven.m_addInfo = vit->m_itemIdx;       // ORIG +0x162/+0x165（make_item 前）
        citem->make_item(inven);                    // vtable+0x8
        applyItemOption(inven, citem, *vit);
        if (citem->is_stackable())
            return 0;
        const int slot = citem->GetItemType();      // vtable+0xc
        item[slot - 10] = inven;                    // slot*0x3d - 0x262
    }
    return 1;
