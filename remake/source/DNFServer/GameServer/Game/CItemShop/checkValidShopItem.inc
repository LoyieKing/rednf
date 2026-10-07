// ---- checkValidShopItem @ 0x8512cda ----
bool CItemShop::checkValidShopItem(unsigned int shopId, unsigned long itemIdx)
{
    std::map<int, ItemShopScript>::iterator it = m_shopMap.find((int)shopId);
    if (it == m_shopMap.end())
    {
        return false;
    }
    // [b28d-38b] ORIG 0x08512cda 形态：bool 标志预清 + 空 then/else 守卫（4.4 对
    // if(!b) 会多 movzbl+xor 归一化，空 then 形才复现 ORIG 的 cmpb/jne 尾跳布局）
    // + 外层条件处 xorb（P168a④ 栈槽物化）+ 内层 while(bFlag) 单次体。
    bool bFlag = false;
    if (bFlag)
    {
    }
    else
    {
        std::vector<int>::iterator vit = it->second.m_curItemList.begin();
        std::vector<int>::iterator vend = it->second.m_curItemList.end();
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
}