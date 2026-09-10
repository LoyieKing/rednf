void CUser::sendCharacQuestPiece()
{
    CQuestShop* shop =
        (CQuestShop*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb);
    if (shop != 0)
        shop->sendCharacQuestPiece(this);
    return;
}
