int CGameManager::getBossTowerIdx(WongWork::CBossTower* tower)
{
    int idx = m_bossTowerPool.GetIndex(tower);
    return idx;
}