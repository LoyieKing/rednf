bool Inven_Item::isEquipableItemType() const
{
    return m_field1 == 1 || m_field1 == 5 || m_field1 == 6 ||
           m_field1 == 8;
}