int TowerOfDespairMgr::Get_APCIndexByLayer(unsigned int layer)
{
    if (100 < layer)
    {
        return 0;
    }
    std::vector<unsigned int>::iterator findIt =
        std::find(m_npcLayers.begin(), m_npcLayers.end(), layer);
    if (findIt != m_npcLayers.end())
    {
        return static_cast<int>(layer);
    }
    return -1;
