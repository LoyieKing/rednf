// [b28d-solo-CSecretShop] 返回类型 int→bool（判词见 CSecretShop.h 声明处）。
bool IBuyRule::CheckLimit(SECRET_SHOP_INFO& info, int itemIdx, int count, int& remain)
{
    SALE_INFO* sale = info.m_retailer.GetSaleInfo(itemIdx);
    BUY_INFO* buy = info.m_shopper.GetBuyInfo(itemIdx);
    if (sale == 0)
        return 0;
    if (buy != 0) {
        if (sale->m_limit < buy->m_count + count)
            return 0;
        remain = sale->m_limit - (buy->m_count + count);
    } else {
        if (sale->m_limit < count)
            return 0;
        remain = sale->m_limit - count;
    }
    return 1;
}