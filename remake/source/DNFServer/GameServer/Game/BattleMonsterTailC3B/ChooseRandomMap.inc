int CDungeon::ChooseRandomMap(int mapType, int direction, int entrance) const
{
    typedef std::multimap<int, CMap*> MapMultiMap;

    if (mapType < 0 || direction < 0 || entrance < 0)
    {
        LogManager::logFormat(
            1, "data_manager.cpp",
            __PRETTY_FUNCTION__, 0x71b,
            "CDungeon::ChooseRandomMap [1](Dungeon Index(%d))(map_type(%d) < 0 || "
            "direction(%d) < 0 || entrance(%d) < 0)==false",
            m_index, mapType, direction, entrance);
        return -1;
    }

    // ORIG 局部形态：p_map 置空、range 默认构造、迭代器默认构造、计数清零。
    CMap* pMap = NULL;
    std::pair<MapMultiMap::const_iterator, MapMultiMap::const_iterator> range;
    MapMultiMap::const_iterator it;
    int count = 0;

    if (mapType == 1)
    {
        if (entrance == 0)
        {
            if (m_dummyMap324[direction].size() == 0)
            {
                LogManager::logFormat(
                    1, "data_manager.cpp",
                    __PRETTY_FUNCTION__, 0x72c,
                    "CDungeon::ChooseRandomMap "
                    "(boss_maplist_[direction].size()==0)(Dungeon Index(%d))"
                    "(map_type(%d),direction(%d),entrance(%d)",
                    m_index, mapType, direction, entrance);
                return -1;
            }
            it = m_dummyMap324[direction].begin();
            for (unsigned int step = 0;
                 static_cast<unsigned int>(rand()) %
                         m_dummyMap324[direction].size() > step;
                 ++step)
                it++;
            pMap = it->second;
            if (pMap->get_index() == -1)
            {
                LogManager::logFormat(
                    1, "data_manager.cpp",
                    __PRETTY_FUNCTION__, 0x736,
                    "CDungeon::ChooseRandomMap "
                    "(p_map->get_index()==-1)[1](Dungeon Index(%d))"
                    "(map_type(%d),direction(%d),entrance(%d)",
                    m_index, mapType, direction, entrance);
            }
            return pMap->get_index();
        }
        range = m_dummyMap324[direction].equal_range(entrance);
        count = m_dummyMap324[direction].count(entrance);
    }
    else if (mapType == 2)
    {
        if (entrance == 0)
        {
            if (m_dummyMap4a4[direction].size() == 0)
            {
                LogManager::logFormat(
                    1, "data_manager.cpp",
                    __PRETTY_FUNCTION__, 0x745,
                    "CDungeon::ChooseRandomMap "
                    "(dummy_maplist_[direction].size()==0)(Dungeon Index(%d))"
                    "(map_type(%d),direction(%d),entrance(%d)",
                    m_index, mapType, direction, entrance);
                return -1;
            }
            it = m_dummyMap4a4[direction].begin();
            for (unsigned int step = 0;
                 static_cast<unsigned int>(rand()) %
                         m_dummyMap4a4[direction].size() > step;
                 ++step)
                it++;
            pMap = it->second;
            if (pMap->get_index() == -1)
            {
                LogManager::logFormat(
                    1, "data_manager.cpp",
                    __PRETTY_FUNCTION__, 0x74f,
                    "CDungeon::ChooseRandomMap "
                    "(p_map->get_index()==-1)[1](Dungeon Index(%d))"
                    "(map_type(%d),direction(%d),entrance(%d)",
                    m_index, mapType, direction, entrance);
            }
            return pMap->get_index();
        }
        range = m_dummyMap4a4[direction].equal_range(entrance);
        count = m_dummyMap4a4[direction].count(entrance);
    }
    else
    {
        if (entrance == 0)
        {
            if (m_map1a4[direction].size() == 0)
            {
                LogManager::logFormat(
                    1, "data_manager.cpp",
                    __PRETTY_FUNCTION__, 0x75e,
                    "CDungeon::ChooseRandomMap "
                    "(maplist_[direction].size()==0)(Dungeon Index(%d))"
                    "(map_type(%d),direction(%d),entrance(%d)",
                    m_index, mapType, direction, entrance);
                return -1;
            }
            it = m_map1a4[direction].begin();
            for (unsigned int step = 0;
                 static_cast<unsigned int>(rand()) % m_map1a4[direction].size() >
                     step;
                 ++step)
                it++;
            pMap = it->second;
            if (pMap->get_index() == -1)
            {
                LogManager::logFormat(
                    1, "data_manager.cpp",
                    __PRETTY_FUNCTION__, 0x768,
                    "CDungeon::ChooseRandomMap "
                    "(p_map->get_index()==-1)[2](Dungeon Index(%d))"
                    "(map_type(%d),direction(%d),entrance(%d)",
                    m_index, mapType, direction, entrance);
            }
            return pMap->get_index();
        }
        range = m_map1a4[direction].equal_range(entrance);
        count = m_map1a4[direction].count(entrance);
    }

    if (count == 0)
    {
        LogManager::logFormat(
            1, "data_manager.cpp",
            __PRETTY_FUNCTION__, 0x77b,
            "CDungeon::ChooseRandomMap "
            "(Key count is zero! : plz check map script)(Dungeon Index(%d))"
            "(map_type(%d),direction(%d),entrance(%d)",
            m_index, mapType, direction, entrance);
        return -1;
    }

    MapMultiMap::const_iterator itRange = range.first;
    for (int step = 0; rand() % count > step; ++step)
        itRange++;
    pMap = itRange->second;
    if (pMap->get_index() == -1)
    {
        LogManager::logFormat(
            1, "data_manager.cpp",
            __PRETTY_FUNCTION__, 0x786,
            "CDungeon::ChooseRandomMap "
            "(p_map->get_index()==-1)[3](Dungeon Index(%d))"
            "(map_type(%d),direction(%d),entrance(%d)",
            m_index, mapType, direction, entrance);
    }
    return pMap->get_index();
