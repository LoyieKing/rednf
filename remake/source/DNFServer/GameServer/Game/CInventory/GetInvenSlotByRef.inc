int CInventory::GetInvenSlotByRef(int invenType, int slot, Inven_Item& item)
{
    if (CheckValidSlot(invenType, slot) != 1)
    {
        return 0;
    }
    switch (invenType)
    {
    case 0:
        item = m_invenItem[slot];
        break;
    case 1:
        item = m_pEquipSlot[slot];
        break;
    case 2:
        item = m_pAvatarSlot[slot];
        break;
    case 3:
        item = m_pCreatureSlot[slot];
        break;
    case 5:
    {
        CExpandEquipslot* pExpand =
            m_pParent->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)9);
        item = pExpand->GetInvenSlot(INVEN_TYPE_5, slot);
        break;
    }
    default:
        break;
    }
    return 1;
