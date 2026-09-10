CDungeon* CDungeonList::find_dungeon(int idx) const
{
    __gnu_cxx::hash_map<int, CDungeon*>::const_iterator it;
    it = m_dungeonMap.find(idx);
    if (it == m_dungeonMap.end())
    {
        return 0;
    }
    return it->second;
