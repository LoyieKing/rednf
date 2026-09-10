int CGameManager::getBlueMarbleIdx(BlueMarble* marble)
{
    int idx = m_blueMarblePool.GetIndex(marble);
    return idx;
