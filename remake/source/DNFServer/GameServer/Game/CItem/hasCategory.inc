// ORIG W 0x822c97a（0x57）：m_set128.find(category) != m_set128.end()
bool CItem::hasCategory(STItemScript::ENUM_ITEM_CATEGORY category) const
{
    std::set<STItemScript::ENUM_ITEM_CATEGORY>::const_iterator it =
        m_set128.find(category);
    return it != m_set128.end();
}