advancealtar::AdvanceAltarShopParameter::getBuyUpgradeDataVec(
    int key, AdvanceAltarShopType::T type)
{
    std::map<int, BuyShopData>::iterator it = m_buyShopMap.find(key);
    // ORIG 0x88a102c 调 ne、命中块 fall-through（0x88a1035），je→0x88a1054 mov$0 尾岛
    if (it != m_buyShopMap.end())
        return it->second.getBuyUpgradeDataVec(type);
    return 0;
