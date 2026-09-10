bool CDungeonList::insert_dungeon(CDungeon* dungeon)
{
    // [b28d-solo-CListTables] ORIG 0x0834d6d2：迭代器先 C1Ev 默认构造（-0x34）
    // 再 `it = find(idx)`（sret -0x40 拷贝，P50）；insert 用 value_type 直构
    // `pair<const int,CDungeon*>(get_index(), dungeon)`（ORIG 0x0838a848 实例
    // `C2IiRS2_EEOT_OT0_`，且 key 是**重新调用** get_index()，非缓存 idx）。
    int idx;
    __gnu_cxx::hash_map<int, CDungeon*>::iterator it;
    idx = dungeon->get_index();
    it = m_dungeonMap.find(idx);
    if (it == m_dungeonMap.end())
    {
        m_dungeonMap.insert(
            __gnu_cxx::hash_map<int, CDungeon*>::value_type(
                dungeon->get_index(), dungeon));
        return true;
    }
    LogManager::logFormat(
        1, "data_manager.cpp", __PRETTY_FUNCTION__,
        0x921, "Already registed dungeon index detected : %d",
        dungeon->get_index());
    return false;
