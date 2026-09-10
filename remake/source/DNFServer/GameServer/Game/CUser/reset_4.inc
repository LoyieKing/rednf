void InventoryMemory::reset()
{
    memset(m_items1, 0, 0x4a58);
    memset(m_items2, 0, 0x1905);
    memset(m_items3, 0, 0x39aa);
    memset(m_items4, 0, ARAD::DATABASE::sourceCargoSize);
