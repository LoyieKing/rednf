// ==== Dispatcher_MonstercardBind::~Dispatcher_MonstercardBind (ORIG D1 0x08244b42 / D0 0x08244b72，基类转发) ====
// ==== Dispatcher_MonstercardBind::check_need_slot (ORIG 0x081d83c4 T) ====
// b28d-solo-Dispatchers verdict=AE(routed) [S162-verify] ORIG 0x081d83c4 orig/ours=28/28
int Dispatcher_MonstercardBind::check_need_slot(CUser* user)
{
    CInventory* invenR = const_cast<CInventory*>(user->getCurCharacInvenR());
    const int emptyCard =
        invenR->get_empty_slot((INVEN_TYPE)1, (Inven_Item::ITEM_TYPE)10);
    const int emptyAny =
        invenR->get_empty_slot((INVEN_TYPE)1, (Inven_Item::ITEM_TYPE)0);
    if (emptyCard < 0 && emptyAny >= 0)
    {
        return 1;
    }
    return 0;
}