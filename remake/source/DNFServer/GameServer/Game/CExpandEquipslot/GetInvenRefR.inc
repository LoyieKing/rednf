Inven_Item* CExpandEquipslot::GetInvenRefR(INVEN_TYPE type, int slot) const
{
    if (slot < 10 || 0x16 < slot)
    {
        return 0;
    }
    switch (type)
    {
    case INVEN_TYPE_5:
        return const_cast<Inven_Item*>(&m_equipSlot[slot - 10]);
    default:
        return 0;
    }
