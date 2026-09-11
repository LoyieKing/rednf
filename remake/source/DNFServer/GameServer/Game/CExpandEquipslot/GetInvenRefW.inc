Inven_Item* CExpandEquipslot::GetInvenRefW(INVEN_TYPE type, int slot)
{
    if (slot < 10 || 0x16 < slot)
    {
        return 0;
    }
    switch (type)
    {
    case INVEN_TYPE_5:
        alter();
        return &m_equipSlot[slot - 10];
    default:
        return 0;
    }
}