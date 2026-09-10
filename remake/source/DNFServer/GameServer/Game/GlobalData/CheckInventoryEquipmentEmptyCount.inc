bool CheckInventoryEquipmentEmptyCount(CUser& user, int need)
{
    return user.getCurCharacInvenR()->check_empty_count(
        (Inven_Item::ITEM_TYPE)1, need);
