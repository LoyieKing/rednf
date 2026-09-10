bool eventReward::rewardWriteUserInven(Inven_Item* item, rewardKey key, int count)
{
    if (this->m_map0.empty())
        return 0;
    std::map<rewardKey, std::map<int, std::vector<itemData> > >::iterator it =
        this->m_map0.find(key);
    if (it == this->m_map0.end())
        return 0;
    // [L10-2] p167 EXTRA `_ZNKSt17_Rb_tree_iterator…ptEv` +1 ⇒ 我方比 ORIG 多一次
    // `it->second`（4.4 每处 `it->` 各发一次 const ptEv）⇒ ORIG 为单次解引用缓存。
    std::map<int, std::vector<itemData> >& subMap = it->second;
    std::map<int, std::vector<itemData> >::iterator fit = subMap.find(count);
    if (fit == subMap.end())
        return 0;
    std::vector<itemData>& vec = fit->second;
    if (vec.empty())
        return 0;
    // [L10-2] P28 增量侧：ORIG 调 `ppEi`（后置）⇒ `vit++`。
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
        int start = 0;
        int end = 0;
        StartEndPos(static_cast<Inven_Item::ITEM_TYPE>(inven.m_field1),
                    start, end, 0);
        for (int i = start; i <= end; ++i)
        {
            if (item[i].m_addInfo == 0)             // +0x02 槽判空
            {
                item[i] = inven;                    // 整体 0x3d 字节写入
                break;
            }
        }
    }
    return 1;
