int CStackableItem::quickslot_priority()
{
    if (m_nItemType == 0 || m_nItemType == 4)
    {
        return 1;
    }
    return 0;
}