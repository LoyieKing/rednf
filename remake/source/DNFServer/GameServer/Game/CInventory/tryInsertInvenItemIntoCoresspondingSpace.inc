int CInventory::tryInsertInvenItemIntoCoresspondingSpace(Inven_Item item) const
{
    if (!item.isEquipableItemType() &&
        !checkStackableLimit((unsigned long)item.m_addInfo, item.m_addInfo2))
    {
        return -10;
    }
    int start = 0;
    int end = 0;
    getStartEndPos((Inven_Item::ITEM_TYPE)item.m_itemType, start, end);
    for (int i = start; i <= end; i++)
    {
        if (m_pEquipSlot[i].m_addInfo == 0)
        {
            return i;
        }
    }
    return -2;
}