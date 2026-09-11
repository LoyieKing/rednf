// ============================================================================
// TowerOfDespairMgr::Get_APCIndexByLayer —— ORIG T 0x08643d9c
//   （报告 TowerOfDespairMgr/Get_APCIndexByLayer.md）：
//   层号在 m_npcLayers 中 → 返回层号；否则 -1；>100 → 0。
// ============================================================================
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
}