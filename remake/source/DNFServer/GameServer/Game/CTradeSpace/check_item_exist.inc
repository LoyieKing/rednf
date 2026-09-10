int CTradeSpace::check_item_exist(int traderIdx, int itemId)
{
    for (int i = 3; i <= 0x1a; ++i) {
        if (TradeSlotAt(m_slotsData, traderIdx, i)->m_item.m_addInfo == itemId)
            return i;
    }
    return -1;
