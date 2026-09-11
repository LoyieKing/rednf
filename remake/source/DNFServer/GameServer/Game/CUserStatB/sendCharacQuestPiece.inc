// ---------------------------------------------------------------------------
// CUser::sendCharacQuestPiece —— ORIG 0x0868af2c。
// 取角色扩展槽 0xb（CQuestShop），非空则转发 sendCharacQuestPiece(this)。
// ---------------------------------------------------------------------------
void CUser::sendCharacQuestPiece()
{
    CQuestShop* shop =
        (CQuestShop*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb);
    if (shop != 0)
        shop->sendCharacQuestPiece(this);
    return;
}
