const CDungeon* TowerOfDespairMgr::Get_CDungeonByLayer(unsigned int layer)
{
    if (layer == 0 || 100 < layer)
    {
        return 0;
    }
    if (m_layerDungeons[layer] == 0)
    {
        m_layerDungeons[layer] =
            G_CDataManager()->find_dungeon(
                static_cast<int>(m_dungeonIndexOfLayer[layer]));
    }
    return m_layerDungeons[layer];
