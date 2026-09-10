int CInventory::check_slot_empty(int slot) const
{
    if (slot > 0x137 || slot <= 0)
        return 0;
    if (m_pEquipSlot[slot].m_addInfo == 0)
    {
        return 1;
    }
    return 0;
