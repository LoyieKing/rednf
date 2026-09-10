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
