bool CExtraction::_check_need_slot(CUser* user)
{
    const CInventory* inven = user->getCurCharacInvenR();
    int s1 = inven->get_empty_slot((INVEN_TYPE)1, (Inven_Item::ITEM_TYPE)10);
    int s2 = inven->get_empty_slot((INVEN_TYPE)1, (Inven_Item::ITEM_TYPE)0);
    return !(s1 < 0 && s2 < 0);
