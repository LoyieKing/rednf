BlueMarble* CGameManager::getBlueMarble(int index)
{
    __gnu_cxx::hash_map<int, BlueMarble*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<BlueMarble*> >::iterator it =
            m_blueMarbleMap.find(index);
    if (it == m_blueMarbleMap.end())
        return 0;
    return it->second;
