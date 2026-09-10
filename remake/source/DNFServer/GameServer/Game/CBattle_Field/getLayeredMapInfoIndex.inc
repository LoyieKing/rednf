MapInfo* CBattle_Field::getLayeredMapInfoIndex(int idx)
{
    int key = getLayeredMapKeyValue(idx);
    std::map<int, MapInfo>::iterator it = m_mapInfo.find(key);
    if (it != m_mapInfo.end())
    {
        return &it->second;
    }

    MapInfo info;
    int key2 = getLayeredMapKeyValue(idx);
    m_mapInfo.insert(std::make_pair(key2, info));
    it = m_mapInfo.find(key2);
    return &it->second;
