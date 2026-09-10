int CInventory::GetItemType(int itemIdx) const
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
        return 0;
    }
    if (item->IsAvatarItem())
    {
        return 8;
    }
    if (!item->is_stackable())
    {
        if (item->GetItemType() == 0x16)
        {
            return 5;
        }
        if (item->GetItemType() == 0x17)
        {
            return 6;
        }
        return 1;
    }
    if (item->GetItemType() == 9)
    {
        return 7;
    }
    if (item->GetItemType() == 1)
    {
        return 3;
    }
    if (item->GetItemType() == 2)
    {
        return 10;
    }
    if (item->GetItemType() == 8)
    {
        return 4;
    }
    if (item->GetItemType() == 0x14)
    {
        return 9;
    }
    return 2;
