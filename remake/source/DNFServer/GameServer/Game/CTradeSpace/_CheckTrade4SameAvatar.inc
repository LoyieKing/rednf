// ---- _CheckTrade4SameAvatar @ 0x08529d48 ----
int CTradeSpace::_CheckTrade4SameAvatar(const TradeSlot* slots,
                                        const Inven_Item& item)
{
    for (int i = 3; i <= 0x1a; ++i) {
        if (slots[i].m_item.m_addInfo == item.m_addInfo &&
            slots[i].m_item.m_addInfo2 == item.m_addInfo2 &&
            slots[i].m_item.m_fieldb == item.m_fieldb)
            return 1;
    }
    return 0;
}