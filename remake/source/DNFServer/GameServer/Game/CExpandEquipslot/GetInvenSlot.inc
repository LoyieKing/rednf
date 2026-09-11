Inven_Item CExpandEquipslot::GetInvenSlot(INVEN_TYPE type, int slot) const
{
    Inven_Item item;
    if (slot < 10 || 0x16 < slot)
    {
        return item;
    }
    switch (type)
    {
    case INVEN_TYPE_5:
        return m_equipSlot[slot - 10];
    default:
        return item;
    }
}