Avatar_Item* CAvatarItemMgr::_FindItem(int key) const
{
    std::map<int, Avatar_Item*>::const_iterator it = m_registeredItems.find(key);
    if (it == m_registeredItems.end())
        return 0;
    return it->second;
}