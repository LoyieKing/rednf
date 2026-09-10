bool CEquipItem::check_eType(char eType) const
{
    if (eType > 0x15)
    {
        return 0;
    }
    if ((char)m_itemType == eType)
    {
        return 1;
    }
    return 0;
