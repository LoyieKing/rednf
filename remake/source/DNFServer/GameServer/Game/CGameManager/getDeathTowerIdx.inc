int CGameManager::getDeathTowerIdx(WongWork::CDeathTower* tower)
{
    int idx = m_deathTowerPool.GetIndex(tower);
    return idx;
