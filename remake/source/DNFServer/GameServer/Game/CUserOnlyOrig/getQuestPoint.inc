int CUser::getQuestPoint()
{
    CQuestShop* shop = (CQuestShop*)GetCharacExpandData(
        static_cast<ENUM_CHARAC_EXPAND_TYPE>(0xb));
    if (shop == 0)
        return 0;
    return shop->getQp();
}