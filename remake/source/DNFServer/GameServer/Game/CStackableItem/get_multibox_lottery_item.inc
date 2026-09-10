void CStackableItem::get_multibox_lottery_item(
    std::vector<std::pair<Inven_Item, bool> >* out)
{
    if (out == 0)
    {
        return;
    }
    int rand = 0;
    int sum = 0;
    int itemIdx = 0;
    int count = 0;
    // b28d-solo 修法：flag char→bool（P300/P248 档②，配 m_defaultFlag bool 化 ⇒ 直存）。
    bool flag = 0;
    std::vector<MultiBoxLotteryInfo::LotteryUnit>::iterator unitIt =
        m_multiBox.m_units.begin();
    while (unitIt != m_multiBox.m_units.end())
    {
        MultiBoxLotteryInfo::LotteryUnit unit(*unitIt);
        for (int i = 0; i < unit.m_count; i++)
        {
            std::vector<std::pair<int, int> >::iterator it =
                unit.m_items.begin();
            std::vector<std::pair<int, int> >::iterator it2 =
                unit.m_bonus.begin();
            rand = get_rand_int(1000000);
            sum = 0;
            itemIdx = 0;
            while (it != unit.m_items.end())
            {
                sum += (*it).second;
                if (sum > rand)
                {
                    itemIdx = (*it).first;
                    count = (*it2).first;
                    flag = (*it2).second != 0;
                    break;
                }
                it++;
                it2++;
            }
            if (itemIdx == 0)
            {
                itemIdx = unit.m_defaultIdx;
                count = unit.m_defaultNum;
                flag = unit.m_defaultFlag;
            }
            Inven_Item item;
            item.m_addInfo = itemIdx;
            G_CDataManager()->m_itemList->create_item(
                (ENUM_ITEM_CREATE_TYPE)2, item, count);
            std::pair<Inven_Item, bool> p;
            p.first = item;
            p.second = flag;
            out->push_back(p);
        }
        unitIt++;
    }
