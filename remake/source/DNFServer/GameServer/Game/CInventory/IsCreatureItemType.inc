bool IsCreatureItemType(int itemType)
{
    return itemType == Inven_Item::ITEM_TYPE_CREATURE_EQUIP ||
           itemType == Inven_Item::ITEM_TYPE_CREATURE_STACK ||
           itemType == Inven_Item::ITEM_TYPE_ARTIFACT;
