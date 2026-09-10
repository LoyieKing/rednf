bool ItemVendingMachine::_CheckEnableInventoryCondition(CUser* user, const CItem* item,
                                                        INVEN_TYPE invenType,
                                                        Inven_Item::ITEM_TYPE itemType)
{
    if (!(user->getCurCharacInvenR()->GetRemainCapacity(invenType, itemType) > 0 ||
          item->get_index() == 200 ||
          item->get_index() == 0xcb ||
          item->get_index() == 0xcc ||
          item->get_index() == 0xcd ||
          item->get_index() == 5 ||
          item->get_index() == 6 ||
          item->get_index() == 7))
        return false;
    return true;
