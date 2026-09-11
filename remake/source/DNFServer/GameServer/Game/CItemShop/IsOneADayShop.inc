// ---- IsOneADayShop @ 0x8513082 ----
bool CItemShop::IsOneADayShop(int shopId) const
{
    std::map<int, ItemShopScript>::const_iterator it = m_shopMap.find(shopId);
    if (it != m_shopMap.end())
        return it->second.m_isOneADay;
    return 0;
}