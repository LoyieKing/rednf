Inven_Item* CExpandEquipslot::getExpandEquipslotW(ENUM_EQUIPSLOT slot)
{
    if (slot == ENUM_EQUIPSLOT_1)
    {
        alter();
        return m_equipSlot;
    }
    if (slot == ENUM_EQUIPSLOT_2)
    {
        alter();
        return m_expandEquipSlot;
    }
    if (slot == ENUM_EQUIPSLOT_3)
    {
        alter();
        return m_expandAvatarSlot;
    }
    return 0;
}