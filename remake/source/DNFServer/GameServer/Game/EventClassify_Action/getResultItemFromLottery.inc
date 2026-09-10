int CEventActionMng::getResultItemFromLottery(unsigned int lotteryId,
                                              std::vector<std::pair<int, int> >& out)
{
    CItem* citem = G_CDataManager()->find_item((int)lotteryId);
    if (citem == 0)
        return 0x11;

    switch (citem->GetItemType())   // ORIG vtable slot +0x0c
    {
    case 6:
    {
        Inven_Item item;
        // [b28d-L14 实证] ORIG 0x0810dc5a 直取 m_itemList 成员（无 getItemList
        // call，P167 差集）；make_pair 实例为 <unsigned long&,int&>（m_addInfo
        // 以 unsigned long 左值视图入参，P61/P211）。
        G_CDataManager()->m_itemList->get_lotto_item(
            citem->get_index(), reinterpret_cast<char*>(&item));
        if (item.m_addInfo == 0 || item.m_addInfo == -1)
            return 0x11;
        out.push_back(std::make_pair(
            reinterpret_cast<unsigned long&>(item.m_addInfo),
            item.m_addInfo2));
        break;
    }
    case 16:
    {
        CStackableItem* stackable = static_cast<CStackableItem*>(citem);
        std::vector<stStackableBoosterItemInfo_t> boosters;
        stackable->getBoosterItem(boosters);
        for (std::vector<stStackableBoosterItemInfo_t>::const_iterator it =
                 boosters.begin();
             it != boosters.end(); ++it)
        {
            if (it->m_itemIdx == 0)
                return 0x11;
            // ORIG make_pair<unsigned int const&, unsigned short const&>
            // （m_itemIdx 字段以 unsigned int 左值视图入参，P61/P211）
            out.push_back(std::make_pair(
                reinterpret_cast<unsigned int const&>(it->m_itemIdx),
                it->m_addInfo));
        }
        break;
    }
    default:
        return 0x17;
    }
    return 0;
