Avatar_Item* CAvatarItemMgr::_FindWaitAssignItem(int key) const
{
    std::map<int, Avatar_Item*>::const_iterator it = m_waitAssignItems.find(key);
    if (it == m_waitAssignItems.end())
        return 0;
    return it->second;
}