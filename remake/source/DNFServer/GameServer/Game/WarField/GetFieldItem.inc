bool WarField::GetFieldItem(int idx, map_item& out)
{
    std::map<int, map_item>::iterator it;
    it = m_mapInfo.m_itemMap.find(idx);
    if (it == m_mapInfo.m_itemMap.end())
    {
        return 0;
    }
    out = it->second;
    return 1;
}