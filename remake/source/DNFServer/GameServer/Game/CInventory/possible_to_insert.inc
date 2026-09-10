bool CInventory::possible_to_insert(int itemIdx) const
{
    Inven_Item::ITEM_TYPE type = (Inven_Item::ITEM_TYPE)GetItemType(itemIdx);
    if (type == 0)
    {
        return 0;
    }
    if (check_empty_count(type, 1))
    {
        return 1;
    }
    CItem* pItem = G_CDataManager()->find_item(itemIdx);
    if (pItem == 0)
    {
        return 0;
    }
    // ORIG 0x8505692 +0x70: is_stackable 返回直落 test %al,%al; je -> +0x99 尾岛
    // (mov $0,%eax)；正向判断（技巧 1 之反例：非 ! 形不出 xor）。
    if (pItem->is_stackable())
    {
        return (unsigned int)(~(unsigned int)check_item_exist(itemIdx)) >> 31;
    }
    return 0;
