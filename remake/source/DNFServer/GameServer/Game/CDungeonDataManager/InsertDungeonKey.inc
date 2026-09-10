bool CDungeonDataManager::InsertDungeonKey(DungeonKey& key)
{
    std::map<DungeonKey, CDungeonNode>::iterator it = m_dungeonMap.find(key);
    if (it != m_dungeonMap.end())
        return false;
    {
        CDungeonNode node;
        // ORIG 0x0848573c 的 make_pair 实例为 <DungeonKey&, CDungeonNode>（第二参按值 ⇒
        // 右值引用形），非具名左值版 <DungeonKey&, CDungeonNode&>（p167 EXTRA/MISSING 差集）。
        // 用 static_cast<T&&> 而非 std::move：4.4 对 std::move 会发 out-of-line call（P234）。
        m_dungeonMap.insert(std::make_pair(key, static_cast<CDungeonNode&&>(node)));
    }
    return true;
