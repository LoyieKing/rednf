void CStackableItem::get_multibox_default_item_arad(
    std::vector<std::pair<Inven_Item, bool> >* out)
{
    if (out == 0)
    {
        return;
    }
    static std::vector<MultiBoxLotteryInfo::LotteryDefaultArad>
        defaultItemListArad;
    if (defaultItemListArad.empty())
    {
        defaultItemListArad.push_back(
            MultiBoxLotteryInfo::LotteryDefaultArad(0x28be16, 10, 50000));
        defaultItemListArad.push_back(
            MultiBoxLotteryInfo::LotteryDefaultArad(0x28be16, 0x32, 25000));
        defaultItemListArad.push_back(
            MultiBoxLotteryInfo::LotteryDefaultArad(0x28be16, 100, 5000));
        defaultItemListArad.push_back(
            MultiBoxLotteryInfo::LotteryDefaultArad(0x28d150, 1, 10000));
    }
    int rand = 0;
    int sum = 0;
    int itemIdx = 0;
    int count = 0;
    unsigned int i = 0;
    if (i < defaultItemListArad.size())
    {
        rand = get_rand_int(100000);
        sum = 0;
        itemIdx = 0;
        for (std::vector<MultiBoxLotteryInfo::LotteryDefaultArad>::iterator
                 it = defaultItemListArad.begin();
             it != defaultItemListArad.end(); it++)
        {
            sum += (*it).m_field8;
            if (sum >= rand)
            {
                itemIdx = (*it).m_field0;
                count = (*it).m_field4;
                break;
            }
        }
        Inven_Item item;
        item.m_addInfo = itemIdx;
        G_CDataManager()->m_itemList->create_item(
            (ENUM_ITEM_CREATE_TYPE)2, item, count);
        std::pair<Inven_Item, bool> p;
        p.first = item;
        p.second = false;
        out->push_back(p);
    }
