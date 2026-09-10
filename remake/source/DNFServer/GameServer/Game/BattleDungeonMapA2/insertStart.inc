void CDungeon::insertStart(int idx, CMap* map)
{
    std::multimap<int, CMap*>::value_type value(map->get_index(), map);
    m_maplist[idx].insert(value);
