bool CSlotBoundChecker::get_item_type(int slot, int capacity, Inven_Item::ITEM_TYPE& outType) const
{
    std::map<SlotBoundKey, SlotBound>::const_iterator it = m_map.begin();
    while (it != m_map.end())
    {
        if (it->second.m_field8 != 0
            && (unsigned short)it->first.m_a == capacity
            && it->second.m_field0 <= slot
            && it->second.m_field4 >= slot)
        {
            outType = it->first.m_b;
            return true;
        }
        ++it;
    }
    return false;
}