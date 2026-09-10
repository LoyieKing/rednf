int TowerOfDespairMgr::Get_DungeonIndexByLayer(unsigned int layer)
{
    if (layer == 0 || 100 < layer)
    {
        return 0;
    }
    return m_dungeonIndexOfLayer[layer];
