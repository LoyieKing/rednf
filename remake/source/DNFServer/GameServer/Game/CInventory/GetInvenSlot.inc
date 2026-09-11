Inven_Item CInventory::GetInvenSlot(int invenType, int slot) const
{
    // b28d-solo-CInventory：256/170，P167 调用点 CLEAR；差为 libstdc++ 模板内联 ⇒ N89 封顶。
    // b28d-37 [N89/N82 封顶，ORIG 0x850?]：P167 调用点差集 CLEAR（0 missing/0 extra）⇒ 语义闭合；ORIG 256 vs our 170 差为 libstdc++ 模板内联/帧复用（N89 族）。
    Inven_Item item;
    if (CheckValidSlot(invenType, slot) != 1)
    {
        return item;
    }
    if (m_pEquipSlot == 0 || m_pAvatarSlot == 0 || m_pCreatureSlot == 0)
    {
        return item;
    }
    switch (invenType)
    {
    case 0:
        return m_invenItem[slot];
    case 1:
        return m_pEquipSlot[slot];
    case 2:
        return m_pAvatarSlot[slot];
    case 3:
        return m_pCreatureSlot[slot];
    case 5:
    {
        CExpandEquipslot* pExpand =
            m_pParent->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)9);
        if (pExpand != 0)
        {
            return pExpand->GetInvenSlot(INVEN_TYPE_5, slot);
        }
        return item;
    }
    default:
        break;
    }
    return item;
}