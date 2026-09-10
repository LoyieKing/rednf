int CTradeSpace::get_empty_itemslot(int traderIdx)
{
    for (int i = 3; i <= 0x1a; ++i) {
        if (TradeSlotAt(m_slotsData, traderIdx, i)->m_item.m_addInfo == 0)
            return i;
    }
    return -1;
