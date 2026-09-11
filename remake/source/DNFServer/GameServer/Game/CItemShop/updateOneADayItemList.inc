// ---- updateOneADayItemList @ 0x8512e98 ----
void CItemShop::updateOneADayItemList()
{
    std::map<int, ItemShopScript>::iterator it = m_shopMap.begin();
    std::map<int, ItemShopScript>::iterator end = m_shopMap.end();
    m_oneADayList.clear();
    while (it != end) {
        if (it->second.m_isOneADay) {
            unsigned int listNo = setOneADayItemListNo(it->second);
            // [b28d-38b] P227 全表达式临时链：ORIG 0x08512f1a 在 cMyTrace ctor 前把
            // it->second.m_shopId 载入 %ebx 跨 ctor 保活（具名 tr 形会被 4.4 改为 ctor
            // 后即时读取）。
            cMyTrace(__PRETTY_FUNCTION__, 0xc69, 0)(
                "updateOneADayItemList shop_id:%d list_no:%d", it->second.m_shopId,
                listNo);
        }
        ++it;
    }
}