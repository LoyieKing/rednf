bool CEquipItem::IsCreatureItem() const
{
    if (m_itemType > 0x15 && m_itemType <= 0x19)
    {
        return 1;
    }
    return 0;
}