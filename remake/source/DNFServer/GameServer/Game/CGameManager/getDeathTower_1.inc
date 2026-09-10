WongWork::CDeathTower* CGameManager::getDeathTower(int index)
{
    __gnu_cxx::hash_map<int, WongWork::CDeathTower*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WongWork::CDeathTower*> >::iterator it =
            m_deathTowerMap.find(index);
    if (it == m_deathTowerMap.end())
        return 0;
    return it->second;
