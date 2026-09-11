// ============================================================================
// LevelupSupport2ndScript::clearScript —— 报告 clearScript.md @0x08ab5964。
// 与 LevelupSupportSciprt::clearScript 同构（同样无迭代器递增，忠实复刻），
// 目标 map 为 +0x04 的 m_stepItems。
// ============================================================================
// [b28c-31] AE：`delete it->second` 复现 ORIG 0x08ab59c6 无槽往返形（43=43）。
void LevelupSupport2ndScript::clearScript()
{
    std::map<int, std::vector<ItemInfo>*>::iterator it = m_stepItems.begin();
    std::map<int, std::vector<ItemInfo>*>::iterator endIt = m_stepItems.end();
    while (it != endIt)
    {
        // [b28c-31] ORIG 0x08ab59c6：`call ptEv; mov 0x4(%eax),%ebx; test %ebx,%ebx;
        // je`（无槽往返，P0）⇒ 直接 delete it->second，编译器自带空指针守卫（P141）。
        delete it->second;   // 不 ++it（忠实）
    }
    m_stepItems.clear();
}