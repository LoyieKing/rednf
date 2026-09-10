bool CCreatureMgr::UnregisterCreatureItem(int uid)
{
    std::map<int, CCreatureItem*>::iterator it = m_creatureItems.find(uid);
    // ORIG 0x833a8xx 块序：失败块在前（fall-through）、成功块在后（跳转目标）
    // ⇒ 写 `if (it == end) { fail; return false; }` 早返形。
    if (it == m_creatureItems.end())
    {
        // [b28d-solo P227] 全表达式临时链（ORIG 变参跨 ctor 保活形）。
        cMyTrace(__PRETTY_FUNCTION__, 0xbc1, 5)(
            "UnregisterCreatureItem failed. uid(%d), key(%d)\n",
            m_user->get_unique_id() & 0xffff, uid);
        return false;
    }
    // ORIG 0x833a8xx：`mov 0x4(%eax),%eax; mov %eax,-0xc(%ebp)` ⇒ it->second
    // 存具名局部（P0 槽往返），FreeCreatureItem 实参从该槽回读。
    CCreatureItem* creatureItem = it->second;
    GetInstanceCreatureItemFactory()->FreeCreatureItem(creatureItem);
    m_creatureItems.erase(it);
    return true;
}
