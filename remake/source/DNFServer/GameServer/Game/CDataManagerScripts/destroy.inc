void CWorldMapList::destroy()
{
    // ORIG 0x8374752：局部变量暂存 second、delete、清零局部（不写回 map 值）。
    std::map<int, CWorldMap*>::iterator it;
    std::map<int, CWorldMap*>::iterator e = m_map.end();
    it = m_map.begin();
    for (; it != e; ++it)
    {
        CWorldMap* worldMap = it->second;
        delete worldMap;
        worldMap = 0;
    }
    m_map.clear();
