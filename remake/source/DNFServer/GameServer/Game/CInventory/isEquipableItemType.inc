bool isEquipableItemType(int itemType)
{
    return itemType == Inven_Item::ITEM_TYPE_EQUIP ||
           itemType == Inven_Item::ITEM_TYPE_CREATURE_EQUIP ||
           itemType == Inven_Item::ITEM_TYPE_ARTIFACT;
