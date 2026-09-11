bool CStackableItem::verify()
{
    if (m_usablePeriod != 0 && getStackableLimit() != 1)
    {
        return 0;
    }
    if (m_nItemType == 0xf || m_nItemType == 0x10)
    {
        if (m_booster.m_elements.empty())
        {
            return 0;
        }
    }
    return 1;
}