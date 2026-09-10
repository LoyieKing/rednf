bool CCacheCharacterMgr::CollectGarbage()
{
    unsigned int now = static_cast<unsigned int>(time(0));
    bool removed = false;
    std::map<int, CACHE_CHARACTER_TYPE*>::iterator it = m_map.begin();
    while (it != m_map.end())
    {
        if ((int)(now - it->second->m_cacheTime) > 0x1d)
        {
            m_pool.Free(it->second);
            // [b28d-solo-ExchangeServer_Infra] P28 增量侧推论：ORIG 0x8490a3c 段
            // 调 `_ZSt17_Rb_tree_iteratorI...EppEi`（后置 ++，带 sret 临时槽 -0x14）
            // 并把旧值直接作 erase 实参 ⇒ 原码是 `m_map.erase(it++);`；写成
            // 具名 cur 拷贝 + 前置 ++it 会改发 ppEv 并丢临时槽（it 槽 -0x1c→-0x18）。
            m_map.erase(it++);
            removed = true;
        }
        else
        {
            ++it;
        }
    }
    return removed;
