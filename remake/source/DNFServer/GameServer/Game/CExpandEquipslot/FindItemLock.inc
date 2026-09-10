bool CExpandEquipslot::FindItemLock(unsigned char lock, ENUM_ITEMSPACE& space,
                                    int& slot) const
{
    slot = 0;
    while (slot <= 0xb)
    {
        if (m_equipSlot[slot].m_amp.GetLock() == lock)
        {
            slot = slot + 10;
            space = (ENUM_ITEMSPACE)0x12;
            return 1;
        }
        slot = slot + 1;
    }
    return 0;
