void CDungeonList::destroy()
{
    CDungeon* dungeon = 0;
    __gnu_cxx::hash_map<int, CDungeon*>::iterator it;
    it = m_dungeonMap.begin();
    while (it != m_dungeonMap.end())
    {
        dungeon = it->second;
        delete dungeon;
        it++;
    }
    m_dungeonMap.clear();
}