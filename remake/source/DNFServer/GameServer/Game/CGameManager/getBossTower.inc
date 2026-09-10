WongWork::CBossTower* CGameManager::getBossTower()
{
    WongWork::CBossTower* tower = m_bossTowerPool.Acquire();
    if (tower == 0)
        return 0;
    tower->setIdx(m_bossTowerPool.GetIndex(tower));
    int towerIdx = tower->getIdx();
    __gnu_cxx::hash_map<int, WongWork::CBossTower*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WongWork::CBossTower*> >::iterator it =
            m_bossTowerMap.find(towerIdx);
    if (it == m_bossTowerMap.end())
    {
        m_bossTowerMap[tower->getIdx()] = tower;
        ((WongWork::CBossStage*)tower)->reset();
        return tower;
    }
    else
    {
        return 0;
    }
