bool CItem::check_item_routing() const
{
    if (isEpicRoutingItem())
    {
        return true;
    }
    return m_rarity > 0;
}