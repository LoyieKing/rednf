Inven_Item* CExpandEquipslot::getExpandEquipslotR(ENUM_EQUIPSLOT slot) const
{
    if (slot == ENUM_EQUIPSLOT_1)
    {
        return const_cast<Inven_Item*>(m_equipSlot);
    }
    if (slot == ENUM_EQUIPSLOT_2)
    {
        return const_cast<Inven_Item*>(m_expandEquipSlot);
    }
    if (slot == ENUM_EQUIPSLOT_3)
    {
        return const_cast<Inven_Item*>(m_expandAvatarSlot);
    }
    return 0;
}