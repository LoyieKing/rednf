int CInventory::use_equip(int slot)
{
    if (slot < 0 || slot > 0x19)
    {
        return 1;
    }
    if (m_invenItem[slot].m_field1 != 1)
    {
        return 0x13;
    }
    CEquipItem* pItem = (CEquipItem*)G_CDataManager()->find_item(m_invenItem[slot].m_addInfo);
    if (pItem == 0)
    {
        return 0x11;
    }
    if (m_invenItem[slot].m_fieldb == 0 || pItem->get_endurance() == 0)
    {
        return 0xb;
    }
    if (pItem->m_itemType < 10)   // ORIG 0x84ff81c：CEquipItem+0x234（m_itemType）< 10 → 不可用
    {
        return 0xb;
    }
    if (m_invenItem[slot].m_addInfo == 0)
    {
        return 0x11;
    }
    if (m_invenItem[slot].m_fieldb == 0)
    {
        return 0x16;
    }
    if (m_invenItem[slot].m_fieldb != 0)
    {
        m_invenItem[slot].m_fieldb--;
    }
    return 0;
}