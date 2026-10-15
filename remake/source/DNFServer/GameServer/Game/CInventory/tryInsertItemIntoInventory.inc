int CInventory::tryInsertItemIntoInventory(Inven_Item item) const
{
    if (item.m_addInfo == 0)
    {
        if (item.m_addInfo2 < 0)
        {
            return -1;
        }
        return 0;
    }
    if (item.m_itemType == 8)
    {
        int result = tryInsertAvatarIntoInventory(item);
        if (result >= 0)
        {
            return result;
        }
        return -1;
    }
    if (item.m_itemType != 1)
    {
        int result = tryStackInvenItemIntoQuickSlot(item);
        if (result >= 0)
        {
            return result;
        }
        if (result == -10)
        {
            return -1;
        }
        result = tryStackInvenItemIntoCorrespondingSpace(item);
        if (result >= 0)
        {
            return result;
        }
        if (result == -10)
        {
            return -1;
        }
    }
    if (item.m_itemType == 2)
    {
        int result = tryInsertInvenItemIntoQuickSlot(item);
        if (result >= 0)
        {
            return result;
        }
        if (result == -10)
        {
            return -1;
        }
        result = tryInsertInvenItemIntoCoresspondingSpace(item);
        if (result >= 0)
        {
            return result;
        }
        if (result == -10)
        {
            return -1;
        }
    }
    else
    {
        int result = tryInsertInvenItemIntoCoresspondingSpace(item);
        if (result >= 0)
        {
            return result;
        }
        result = tryInsertInvenItemIntoQuickSlot(item);
        if (result >= 0)
        {
            return result;
        }
    }
    return -1;
}