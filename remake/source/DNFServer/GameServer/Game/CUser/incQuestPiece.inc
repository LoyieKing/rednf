void CUser::incQuestPiece(int val, int piece, bool send)
{
    CQuestShop* shop = (CQuestShop*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb);
    // ORIG 0x868af8c：双 early-return 形（probe k2 实证）——`je nop; body; je leave;
    // call; jmp leave; nop; leave`。getPiece 结果 spill -0xc dead（技巧 8）+
    // shop=-0x10（技巧 21 先声明远槽）。
    if (shop == 0)
        return;
    int deadPiece = shop->getPiece();
    (void)deadPiece;
    shop->addPiece(val);
    if (!send)
    {
        shop->sendCharacQuestPiece(this);
        return;
    }
}