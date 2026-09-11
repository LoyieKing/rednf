// [b28d-solo-CBattle_Field] 已修 AE（178=178；旧判「N95 槽基差族」作废）：
// ORIG 第二次 key 落**独立槽 -0x204**（首算 key 在 -0x1fc）⇒ P0 槽-局部
// 双向对应律：原码是新的具名局部，不是复用 `key`。改成 `int insertKey = ...`
// 后 4.4 逐条复现（info 槽 -0x2f0、key -0x204 同时归位；帧 sub $0x310 不变）。
// m_layeredMap bool 化后 0x0830cd42 段 setne 伪差已消（DIFF→NEAR→AE）。
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
}