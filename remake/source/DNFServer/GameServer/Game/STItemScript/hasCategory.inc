bool STItemScript::hasCategory(ENUM_ITEM_CATEGORY category) const
{
    std::set<ENUM_ITEM_CATEGORY>::const_iterator it = m_set11c.find(category);
    return it != m_set11c.end();
