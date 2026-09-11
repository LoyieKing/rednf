Avatar_Item* CAvatarItemMgr::EraseAvatarItem(int key)
{
    Avatar_Item* item = _FindItem(key);
    if (item == 0)
        return 0;
    m_registeredItems.erase(key);
    if (m_emblemEnduranceMap.find(key) != m_emblemEnduranceMap.end())
        m_emblemEnduranceMap.erase(key);
    return item;
}