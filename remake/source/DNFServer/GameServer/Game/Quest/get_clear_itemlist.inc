// b28d-20 AE：ORIG 0x08353404。声明序 item/out/p + pItem/randValue 循环外
// 具名（ORIG -0x2c/-0x28 清 0 实证）+ 正向 `if (is_stackable())`（消 xor $1）
// + pair 字段直赋（消 make_pair 临时）；134/134 槽位对齐。
IntPairVector Quest::get_clear_itemlist(
    int dungeonIdx) const
{
    DungeonClearItem item;
    std::vector<std::pair<int, int> > out;
    std::pair<int, int> p;
    CItem* pItem = 0;
    int randValue = 0;
    for (int i = 0; i < (int)m_clearItems.size(); ++i)
    {
        item = m_clearItems.at(i);
        if (item.m_itemIdx != dungeonIdx)
        {
            continue;
        }
        pItem = G_CDataManager()->find_item(item.m_rewardItem);
        if (!pItem)
        {
            // ORIG 0x83534e3：find_item 失败记录日志
            LogManager::logFormat(
                1, "data_manager.cpp",
                __PRETTY_FUNCTION__, 0x1279,
                "G_CDataManager()->find_item(%d)", item.m_rewardItem);
            continue;
        }
        if (pItem->is_stackable())
        {
            int count = 0;
            for (int k = 0; k < item.m_count; ++k)
            {
                randValue = get_rand_int(100);
                if (randValue <= item.m_percent)
                {
                    ++count;
                }
            }
            if (count > 0)
            {
                p.first = item.m_rewardItem;
                p.second = count;
                out.push_back(p);
            }
        }
        else
        {
            randValue = get_rand_int(100);
            if (randValue <= item.m_percent)
            {
                p.first = item.m_rewardItem;
                p.second = 1;
                out.push_back(p);
            }
        }
    }
    return out;
}