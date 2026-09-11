// ORIG _ZN15cUserHistoryLog15BuyCashShopItemEjmjPKccN19CeraShopPaymentType1TEy
//      @0x08686ea0：价格 u64 经 NumberToString 渲染后入参。
void cUserHistoryLog::BuyCashShopItem(unsigned int itemIdx, unsigned long uid,
                                      unsigned int count, const char* buyer, char flag,
                                      CeraShopPaymentType::T payType,
                                      unsigned long long price)
{
    m_user->LogHistory("BuyCashItem, %u, %u, %u, %s, %d, %d, %s",
                       itemIdx, (unsigned int)uid, count, buyer, (int)flag,
                       (int)payType, NumberToString(price, 0));
}