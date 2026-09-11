bool CStackableItem::IsCreatureItem() const
{
    if (m_nItemType > 8 && m_nItemType <= 0xa)
    {
        return 1;
    }
    return 0;
}