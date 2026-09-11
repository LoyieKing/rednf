Avatar_Item* CAvatarItemMgr::_FindItemAll(int key) const
{
    // ORIG：_FindItemAll 调 IsTempKey（W 0x0822d20d）判定符号位；
    // IsTempKey 为 static（mangled 不变），const 方法可直接调用。
    if (IsTempKey(key))
        return _FindWaitAssignItem(key);
    std::map<int, Avatar_Item*>::const_iterator it = m_registeredItems.find(key);
    if (it == m_registeredItems.end())
        return 0;
    return it->second;
}