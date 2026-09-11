bool CStackableItem::getUsableItemType(int type) const
{
    std::vector<int>::const_iterator it =
        std::find(m_usableTypes.begin(), m_usableTypes.end(), type);
    if (it != m_usableTypes.end())
    {
        return 1;
    }
    return 0;
}