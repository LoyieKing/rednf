void CInventory::StartEndPos(Inven_Item::ITEM_TYPE type, int& start, int& end) const
{
    if (!g_slotBoundChecker.get_slot_bound(type, m_inventoryCapacity, start, end))
    {
        start = 0;
        end = 0;
    }
