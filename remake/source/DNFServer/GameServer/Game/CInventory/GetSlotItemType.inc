int CInventory::GetSlotItemType(int slot) const
{
    Inven_Item::ITEM_TYPE type;
    if (g_slotBoundChecker.get_item_type(slot, m_inventoryCapacity, type))
    {
        return type;
    }
    return Inven_Item::ITEM_TYPE_INVALID;
