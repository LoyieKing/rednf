int ItemVendingMachine::GetFinalProbability(int goodsIdx)
{
    float weight;
    int rate;
    CCeraShopGoods* goods;
    goods = reinterpret_cast<CCeraShopGoods*>(G_CDataManager()->FindGoods(goodsIdx));
    weight = goods->GetBonusWeight();
    rate = reinterpret_cast<VendingItemListView*>(G_CDataManager())->m_list->GetVendingBonusRate();
    rate = (int)((float)rate * weight);
    return rate;
