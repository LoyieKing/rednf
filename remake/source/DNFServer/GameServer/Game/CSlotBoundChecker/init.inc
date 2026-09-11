void CSlotBoundChecker::init()
{
    insert(3, 8, Inven_Item::ITEM_TYPE_0, false, true);
    insert(9, 0x38, Inven_Item::ITEM_TYPE_EQUIP, true, true);
    insert(0x39, 0x68, Inven_Item::ITEM_TYPE_2, true, true);
    insert(0x69, 0x98, Inven_Item::ITEM_TYPE_3, true, true);
    insert(0x99, 0xc8, Inven_Item::ITEM_TYPE_4, true, true);
    insert(0xc9, 0xf8, Inven_Item::ITEM_TYPE_10, true, true);
    insert(0xf9, 0x137, Inven_Item::ITEM_TYPE_9, false, true);
    insert(0, 0x8b, Inven_Item::ITEM_TYPE_CREATURE_EQUIP, false, false);
    insert(0x8c, 0xbc, Inven_Item::ITEM_TYPE_ARTIFACT, false, false);
    insert(0xbd, 0xed, Inven_Item::ITEM_TYPE_CREATURE_STACK, false, false);
}