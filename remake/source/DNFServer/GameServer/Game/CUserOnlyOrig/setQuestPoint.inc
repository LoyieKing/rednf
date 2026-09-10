void CUser::setQuestPoint(int qp)
{
    CQuestShop* shop =
        reinterpret_cast<CQuestShop*>(
            GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb));
    if (shop == 0)
        return;
    shop->setQp(qp);
    shop->sendCharacQp(this, '\0');
