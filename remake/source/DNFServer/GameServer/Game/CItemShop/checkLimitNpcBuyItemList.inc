bool CItemShop::checkLimitNpcBuyItemList(unsigned int shopId, unsigned long itemIdx)
{
    std::map<int, ItemShopScript>::iterator it = m_shopMap.find((int)shopId);
    if (it == m_shopMap.end())
        return false;
    // [b28d-38b] ORIG 0x08513580：与 checkValidShopItem 同构（xorb 栈槽物化 +
    // 空 then 守卫 + 内层 while 单次体）。
    bool bFlag = false;
    if (bFlag)
    {
    }
    else
    {
        std::vector<int>::iterator vit = it->second.m_limitNpcBuyList.begin();
        std::vector<int>::iterator vend = it->second.m_limitNpcBuyList.end();
        while (vit != vend && (bFlag = !bFlag))
        {
            int* pItem = &*vit;
            while (bFlag)
            {
                if ((unsigned long)*pItem == itemIdx)
                    return true;
                bFlag = false;
            }
            vit++;
        }
    }
    return false;
