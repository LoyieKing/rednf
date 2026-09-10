CWorldMap* CWorldMapList::find_world_map(int idx) const
{
    // ORIG 0x83647a8：it 先默认构造（C1Ev @-0x10）；find 结果落临时 -0x1c 后
    // 0x83647cf-0x83647d2 单字赋值拷贝 → 复现"声明+赋值"两段式。
    std::map<int, CWorldMap*>::const_iterator it;
    it = m_map.find(idx);
    return (it == m_map.end()) ? 0 : it->second;
