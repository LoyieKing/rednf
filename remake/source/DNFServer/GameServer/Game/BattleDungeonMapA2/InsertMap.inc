void CDungeon::InsertMap(int idx, int direction, CMap* map)
{
    if (map->get_map_type() == 0)
    {
        if ((direction & 1) != 0)
        {
            std::multimap<int, CMap*>::value_type value((PATH_DIRECTION)1, map);
            m_map1a4[idx].insert(value);
        }
        if ((direction & 2) != 0)
        {
            std::multimap<int, CMap*>::value_type value((PATH_DIRECTION)2, map);
            m_map1a4[idx].insert(value);
        }
        if ((direction & 4) != 0)
        {
            std::multimap<int, CMap*>::value_type value((PATH_DIRECTION)4, map);
            m_map1a4[idx].insert(value);
        }
        if ((direction & 8) != 0)
        {
            std::multimap<int, CMap*>::value_type value((PATH_DIRECTION)8, map);
            m_map1a4[idx].insert(value);
        }
    }
    else
    {
        if (map->get_map_type() == 2)
        {
            if ((direction & 1) != 0)
            {
                std::multimap<int, CMap*>::value_type value((PATH_DIRECTION)1, map);
                m_dummyMap4a4[idx].insert(value);
            }
            if ((direction & 2) != 0)
            {
                std::multimap<int, CMap*>::value_type value((PATH_DIRECTION)2, map);
                m_dummyMap4a4[idx].insert(value);
            }
            if ((direction & 4) != 0)
            {
                std::multimap<int, CMap*>::value_type value((PATH_DIRECTION)4, map);
                m_dummyMap4a4[idx].insert(value);
            }
            if ((direction & 8) != 0)
            {
                std::multimap<int, CMap*>::value_type value((PATH_DIRECTION)8, map);
                m_dummyMap4a4[idx].insert(value);
            }
            m_hasDummyMap = 1;                         // +0x624（byte 写）
        }
        else
        {
            if ((direction & 1) != 0)
            {
                std::multimap<int, CMap*>::value_type value((PATH_DIRECTION)1, map);
                m_dummyMap324[idx].insert(value);
            }
            if ((direction & 2) != 0)
            {
                std::multimap<int, CMap*>::value_type value((PATH_DIRECTION)2, map);
                m_dummyMap324[idx].insert(value);
            }
            if ((direction & 4) != 0)
            {
                std::multimap<int, CMap*>::value_type value((PATH_DIRECTION)4, map);
                m_dummyMap324[idx].insert(value);
            }
            if ((direction & 8) != 0)
            {
                std::multimap<int, CMap*>::value_type value((PATH_DIRECTION)8, map);
                m_dummyMap324[idx].insert(value);
            }
        }
    }
