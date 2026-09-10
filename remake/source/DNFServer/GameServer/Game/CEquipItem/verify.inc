bool CEquipItem::verify()
{
    if (m_usablePeriod != 0 || m_expirationDate != 0)
    {
        if ((GetItemType() < 0 || GetItemType() > 9) &&
            GetItemType() != 0xb &&
            GetItemType() != 0x16 &&
            GetItemType() != 10)
        {
            return 0;
        }
    }
    return 1;
