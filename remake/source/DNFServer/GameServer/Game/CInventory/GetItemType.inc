Inven_Item::ITEM_TYPE CInventory::GetItemType(int itemIdx) const
{
    CItem* item = G_CDataManager()->find_item(itemIdx);
    if (item == 0)
    {
        if (itemIdx != 0)
        {
            LogManager::logFormat(
                1, "inventory.cpp",
                __PRETTY_FUNCTION__, 0x13c2,
                "No Item %d", itemIdx);
        }
        return (Inven_Item::ITEM_TYPE)0;
    }
    if (item->IsAvatarItem())
    {
        return (Inven_Item::ITEM_TYPE)8;
    }
    if (!item->is_stackable())
    {
        if (item->GetItemType() == 0x16)
        {
            return (Inven_Item::ITEM_TYPE)5;
        }
        if (item->GetItemType() == 0x17)
        {
            return (Inven_Item::ITEM_TYPE)6;
        }
        return (Inven_Item::ITEM_TYPE)1;
    }
    if (item->GetItemType() == 9)
    {
        return (Inven_Item::ITEM_TYPE)7;
    }
    if (item->GetItemType() == 1)
    {
        return (Inven_Item::ITEM_TYPE)3;
    }
    if (item->GetItemType() == 2)
    {
        return (Inven_Item::ITEM_TYPE)10;
    }
    if (item->GetItemType() == 8)
    {
        return (Inven_Item::ITEM_TYPE)4;
    }
    if (item->GetItemType() == 0x14)
    {
        return (Inven_Item::ITEM_TYPE)9;
    }
    return (Inven_Item::ITEM_TYPE)2;
}