MapInfo* CBattle_Field::GetMapInfoFromPos(int x, int y, bool create)
{
    int key = GetMapInfoKeyValue(x, y);
    std::map<int, MapInfo>::iterator it = m_mapInfo.find(key);
    if (it != m_mapInfo.end())
    {
        if (create)
        {
            return &it->second;
        }
        if (m_gridMaze != 0)
        {
            const GridScript* grid = &m_gridMaze->getGridR(x, y);
            if (grid != 0 && grid->m_layeredMapIndexes.size() != 0)
            {
                if (getCurrentLayeredMapInfo()->m_layeredMap)
                {
                    return getCurrentLayeredMapInfo();
                }
            }
        }
        return &it->second;
    }

    MapInfo info;
    // [b28d-sweep-CBattle_Field] P0：ORIG 第二次 key 落独立槽 -0x204（首算
    // 的 key 在 -0x1fc）⇒ 原码是新的具名局部（复用 key 时 4.4 恒复用 -0x1fc）。
    int insertKey = GetMapInfoKeyValue(x, y);
    m_mapInfo.insert(std::make_pair(insertKey, info));
    it = m_mapInfo.find(insertKey);
    return &it->second;
