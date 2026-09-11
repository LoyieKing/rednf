// [b28d-solo-CStackableItem] ORIG 0x850ebfc：pairIt 先声明（默认构造 C1Ev，P50）后
// `pairIt = begin()`；循环体 `item = std::pair<int,int>(*pairIt)` 走 aSEOS0_（P135），
// 之后一律读 item.first/second（-0x44/-0x40）；countIt 为 const_iterator 转换构造。
void CStackableItem::get_lotto_item(char* out)
{
    Inven_Item* pItem = (Inven_Item*)out;
    int itemIdx = -1;
    unsigned long range = 100000;
    int rand = m_pRand->randInt(range);
    int sum = 0;
    std::vector<std::pair<int, int> >::iterator pairIt;
    int count = 1;
    std::vector<int>::const_iterator countIt = m_lotto.m_counts.begin();
    std::pair<int, int> item;
    pairIt = m_lotto.m_items.begin();
    while (pairIt != m_lotto.m_items.end())
    {
        item = std::pair<int, int>(*pairIt);
        sum += item.second;
        if (sum > rand)
        {
            itemIdx = item.first;
            count = *countIt;
            break;
        }
        pairIt++;
        ++countIt;
    }
    if (itemIdx == 0 && count > 0)
    {
        pItem->m_addInfo = 0;
        pItem->m_addInfo2 = count;
    }
    else
    {
        if (itemIdx == -1)
        {
            itemIdx = m_lotto.m_defaultIdx;
            count = m_lotto.m_defaultCount;
            if (itemIdx == 0)
            {
                pItem->m_addInfo = itemIdx;
                pItem->m_addInfo2 = count;
                return;
            }
        }
        pItem->m_addInfo = itemIdx;
        G_CDataManager()->m_itemList->create_item(
            (ENUM_ITEM_CREATE_TYPE)2, *pItem, count);
    }
}