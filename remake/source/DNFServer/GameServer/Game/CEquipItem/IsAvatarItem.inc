bool CEquipItem::IsAvatarItem() const
{
    if (m_itemType >= 0 && m_itemType <= 9)
    {
        return 1;
    }
    return 0;
