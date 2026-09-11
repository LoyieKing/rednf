// ============================================================================
// 死亡塔 / 领主塔 / 前置祭坛 / 蓝弹珠
// ============================================================================
WongWork::CDeathTower* CGameManager::getDeathTower()
{
    WongWork::CDeathTower* tower = m_deathTowerPool.Acquire();
    if (tower == 0)
        return 0;
    tower->setIdx(m_deathTowerPool.GetIndex(tower));
    int towerIdx = tower->getIdx();
    __gnu_cxx::hash_map<int, WongWork::CDeathTower*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WongWork::CDeathTower*> >::iterator it =
            m_deathTowerMap.find(towerIdx);
    if (it == m_deathTowerMap.end())
    {
        m_deathTowerMap[tower->getIdx()] = tower;
        tower->reset();
        return tower;
    }
    else
    {
        return 0;
    }
}