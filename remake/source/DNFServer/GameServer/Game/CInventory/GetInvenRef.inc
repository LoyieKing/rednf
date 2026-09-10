Inven_Item* CInventory::GetInvenRef(int invenType, int slot)
{
    if (CheckValidSlot(invenType, slot) != 1)
    {
        return 0;
    }
    switch (invenType)
    {
    case 0:
        return &m_invenItem[slot];
    case 1:
        return &m_pEquipSlot[slot];
    case 2:
        return &m_pAvatarSlot[slot];
    case 3:
        return &m_pCreatureSlot[slot];
    case 5:
    {
        CExpandEquipslot* pExpand =
            m_pParent->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)9);
        return pExpand->GetInvenRefW(INVEN_TYPE_5, slot);
    }
    default:
        return 0;
    }
