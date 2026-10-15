bool Inven_Item::isEquipableItemType() const
{
    return m_itemType == 1 || m_itemType == 5 || m_itemType == 6 ||
           m_itemType == 8;
}