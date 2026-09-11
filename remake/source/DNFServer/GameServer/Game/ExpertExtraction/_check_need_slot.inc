// ============================================================================
// CExtraction::_check_need_slot —— ORIG T 0x084a29f4
//   两个空槽位（ITEM_TYPE 10 与 0）任一存在即可。
// ============================================================================
bool CExtraction::_check_need_slot(CUser* user)
{
    const CInventory* inven = user->getCurCharacInvenR();
    int s1 = inven->get_empty_slot((INVEN_TYPE)1, (Inven_Item::ITEM_TYPE)10);
    int s2 = inven->get_empty_slot((INVEN_TYPE)1, (Inven_Item::ITEM_TYPE)0);
    return !(s1 < 0 && s2 < 0);
}