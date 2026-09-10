Inven_Item* CInventory::GetInvenStart(int invenType) const
{
    switch (invenType)
    {
    case 0:
        return const_cast<Inven_Item*>(&m_invenItem[10]);
    case 1:
        return m_pEquipSlot;
    case 2:
        return m_pAvatarSlot;
    case 3:
        return m_pCreatureSlot;
    case 5:
    {
        CExpandEquipslot* expand =
            m_pParent->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)9);
        return expand->getExpandEquipslotW((ENUM_EQUIPSLOT)1);
    }
    }
    return 0;
