// ---- GetItemPrice（ORIG T 0x85c7d04，报告 GetItemPrice.md 全文）----
// 引参写出顺序（ORIG 硬序）：outPrice←+0x40 先于一切校验，随后两金清零；
// 堆叠分支做数量乘并溢出检测，非堆叠仅放行化身/整条购买。
bool CPrivateStore::GetItemPrice(CItem const* item, PrivateStoreItem& storeItem,
                                 int count, int& outPrice,
                                 unsigned int& outBuyGold, unsigned int& outSellGold)
{
    outPrice = storeItem.m_field40;   // 校验前先写单价
    outBuyGold = 0;
    outSellGold = 0;

    if (item->is_stackable())
    {
        if (count < 1)
        {
            m_errorHandler.Do(1);
            return false;
        }
        if ((int)storeItem.m_item.m_addInfo2 < count)   // 库存数 < 请求数
        {
            m_errorHandler.Do(0x11);
            return false;
        }
        // ORIG 无中转槽：arg1 直载 *outPrice 旧值（4.1 前端不落临时）
        if (COperator<int>::Product(outPrice, count, outPrice) != 1)
        {
            m_errorHandler.Do(1);
            return false;
        }
        outBuyGold = outBuyGold * count;   // 0*count（ORIG imul 保形）
        outSellGold = outSellGold * count;
    }
    else
    {
        // ORIG vtable+0x10 虚调 = CItem::IsAvatarItem（CCeraShop_Special.cpp:1024 同证）。
        // 仅当「非化身 且 库存数 != 请求数」报错。
        if (!item->IsAvatarItem() && (int)storeItem.m_item.m_addInfo2 != count)
        {
            m_errorHandler.Do(1);
            return false;
        }
    }

    // ORIG choose 双岛形：jg → mov$1；else Do(1) → mov$0
    if (outPrice <= 0)
    {
        m_errorHandler.Do(1);
        return false;
    }
    return true;
}