// ORIG _ZN13user_creature12CCreatureMgr22UnregisterCreatureItemEi T 0x0833a854。
// 在 m_creatureItems 中查 uid：命中则经 CCreatureItemFactory 释放并 erase，返回 true；
// 未命中 → cMyTrace("UnregisterCreatureItem failed. uid(%d), key(%d)\n") 并返回 false。
// 注：CCreatureItemFactory::FreeCreatureItem 未建模（CCreature.cpp 未入 CMake），
// 与 Reset() 一致采用 BLOCKED 标注；erase 照常执行。
// [b28d-solo-CCreatureMgr AE] 翻正两旋钮：①块序改 `if (it == end) { fail; return false; }`
// 早返（失败块 fall-through、成功块跳转目标，与 ORIG 一致）；②`it->second` 存具名局部
// （P0 槽往返：ORIG `mov 0x4(%eax),%eax; mov %eax,-0xc(%ebp)`）；68=68 逐条全等。
// ORIG 0x0833a854
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
