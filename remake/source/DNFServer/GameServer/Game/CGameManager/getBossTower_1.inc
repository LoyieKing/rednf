WongWork::CBossTower* CGameManager::getBossTower(int index)
{
    __gnu_cxx::hash_map<int, WongWork::CBossTower*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WongWork::CBossTower*> >::iterator it =
            m_bossTowerMap.find(index);
    if (it == m_bossTowerMap.end())
        return 0;
    return it->second;
}