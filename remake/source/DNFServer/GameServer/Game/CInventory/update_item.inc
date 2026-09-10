void CInventory::update_item(INVEN_TYPE invenType, int slot, Inven_Item item)
{
    if (CheckValidSlot(invenType, slot) != 1)
    {
        return;
    }
    switch (invenType)
    {
    case 0:
        m_invenItem[slot] = item;
        break;
    case 1:
        m_pEquipSlot[slot] = item;
        break;
    case 2:
        m_pAvatarSlot[slot] = item;
        break;
    case 3:
        m_pCreatureSlot[slot] = item;
        break;
    }
