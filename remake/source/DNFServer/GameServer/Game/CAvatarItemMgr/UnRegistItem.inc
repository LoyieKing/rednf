bool CAvatarItemMgr::UnRegistItem(int key)
{
    Avatar_Item* item = _FindItem(key);
    if (item == 0)
        return false;
    m_registeredItems.erase(key);
    m_AvatarItemPool.Free(item);
    // [b28d-38] 单表达式形：ORIG 先求 end（-0x14）后求 find（-0x10），
    // 且 erase 传 key 地址（0xc(%ebp)）非迭代器副本；具名 it 形 4.4 恒 find 先。
    if (m_emblemEnduranceMap.find(key) != m_emblemEnduranceMap.end())
        m_emblemEnduranceMap.erase(key);
    return true;
}