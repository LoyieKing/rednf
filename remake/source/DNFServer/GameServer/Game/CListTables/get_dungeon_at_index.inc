CDungeon* CDungeonList::get_dungeon_at_index(int idx) const
{
    // [b28d-solo-CListTables] ORIG 0x0834d846：`while (it != end)` 底部条件形
    // （4.1 把条件块置循环尾：+0x74 end()/operator!=/jne 体；相等命中直接
    // `return it->second`，遍历完落 `mov $0,%eax`）。旧写 `while(true){ if(it==end)
    // return 0; ... break; }` 多 1 条对齐 nop 且块序相反。
    __gnu_cxx::hash_map<int, CDungeon*>::const_iterator it;
    int count = 0;
    it = m_dungeonMap.begin();
    while (it != m_dungeonMap.end())
    {
        if (idx == count)
        {
            return it->second;
        }
        count++;
        it++;
    }
    return 0;
}