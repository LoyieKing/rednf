bool CSlotBoundChecker::get_slot_bound(Inven_Item::ITEM_TYPE type, int capacity, int& start, int& end) const
{
    SlotBoundKey key((unsigned short)capacity, type);
    std::map<SlotBoundKey, SlotBound>::const_iterator it = m_map.find(key);
    if (it != m_map.end())
    {
        start = it->second.m_field0;
        end = it->second.m_field4;
        return true;
    }
    return false;
