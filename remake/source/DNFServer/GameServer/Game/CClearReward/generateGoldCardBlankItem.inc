// [b28d-solo-CClearReward] verdict=AE：stGoldCardBlankItemInfo_t::v 由 int[3]
// 改 unsigned int[3]（CItemGenerator.h:790）—— v[1]→Inven_Item::m_addInfo 单段
// 直落、v[2]→CItemList::create_item(...,int) 两段 eax→ebx 中转（P211 档③）。
int CClearReward::generateGoldCardBlankItem(Inven_Item& item)
{
    if (m_goldBlanks.empty())
        return 0;
    int cum = 0;
    int pick = get_rand_int(m_goldCardBlankTotal);
    // [b28d-63] ORIG 0x085391a9：const_iterator begin/end 头部各求值一次 +
    // operator!=/operator++/operator->（P167 原报 EXTRA operator[] ×4）。
    std::vector<stGoldCardBlankItemInfo_t>::const_iterator it = m_goldBlanks.begin();
    std::vector<stGoldCardBlankItemInfo_t>::const_iterator endIt = m_goldBlanks.end();
    while (it != endIt)
    {
        cum += it->v[0];
        if (pick < cum)
        {
            if (it->v[1] == -1)
                return 0;
            item.m_addInfo = it->v[1];
            G_CDataManager()->m_itemList->create_item(
                static_cast<ENUM_ITEM_CREATE_TYPE>(1), item, it->v[2]);
            return 1;
        }
        ++it;
    }
    return 0;
}