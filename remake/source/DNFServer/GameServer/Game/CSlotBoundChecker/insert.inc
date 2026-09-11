void CSlotBoundChecker::insert(int slot, int capacity, Inven_Item::ITEM_TYPE type, bool a, bool b)
{
    int curSlot = 0;
    int v;
    for (v = 0; v <= 0x10; v += 8)
    {
        curSlot = (a != 0) ? (capacity - 0x10 + v) : capacity;
        m_map.insert(std::make_pair(
            SlotBoundKey((unsigned short)v, type),
            SlotBound(slot, curSlot, b)));
    }
}