bool CItem::hasCategory(STItemScript::ENUM_ITEM_CATEGORY category) const
{
    std::set<STItemScript::ENUM_ITEM_CATEGORY>::const_iterator it =
        m_set128.find(category);
    return it != m_set128.end();
