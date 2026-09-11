bool CEquipItem::IsEnchantalbe() const
{
    if (m_itemType > 9 && m_itemType <= 0x15)
    {
        return 1;
    }
    return 0;
}