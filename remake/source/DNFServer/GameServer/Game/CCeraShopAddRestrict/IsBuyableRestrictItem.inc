bool Manager::IsBuyableRestrictItem(CUser* user, unsigned int ipgNo,
                                   unsigned int itemIdx, int type)
{
    return ARAD::Singleton<CerashopAddRestrict::Manager>::Get()
        ->CheckBuyableProduct(user, ipgNo, itemIdx, type);
