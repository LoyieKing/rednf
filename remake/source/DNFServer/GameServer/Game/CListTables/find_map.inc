CMap* CMapList::find_map(int idx)
{
    __gnu_cxx::hash_map<int, CMap*>::iterator it;
    it = m_mainMap.find(idx);
    if (it != m_mainMap.end())
    {
        return it->second;
    }
    return 0;
}