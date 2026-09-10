bool ItemVendingMachine::_GetInvenSlotInfo(const CCeraShopGoods* goods,
                                           INVEN_TYPE& invenType,
                                           Inven_Item::ITEM_TYPE& itemType)
{
    int groupIdx = goods->GetGroupIdx();
    switch (groupIdx)
    {
    case 0:
        invenType = INVEN_TYPE_AVATAR;
        break;
    case 2:
        invenType = INVEN_TYPE_EQUIP;
        itemType = _GetItemType(goods);
        break;
    case 3:
        invenType = INVEN_TYPE_CREATURE;
        itemType = _GetItemType(goods);
        break;
    case 7:
        invenType = INVEN_TYPE_EQUIP;
        itemType = (Inven_Item::ITEM_TYPE)2;
        break;
    }
    return true;
