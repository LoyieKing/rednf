int CInventory::get_inven_slot_no(int itemIdx) const
{
    for (int i = 0; i <= 0x137; i++)
    {
        if (m_pEquipSlot[i].m_addInfo == itemIdx)
        {
            return i;
        }
    }
    return -1;
