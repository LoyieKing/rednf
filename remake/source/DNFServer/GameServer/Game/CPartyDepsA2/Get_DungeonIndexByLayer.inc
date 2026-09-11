// ============================================================================
// TowerOfDespairMgr::Get_DungeonIndexByLayer —— ORIG T 0x08643d76
//   （报告 TowerOfDespairMgr/Get_DungeonIndexByLayer.md）
// ============================================================================
int TowerOfDespairMgr::Get_DungeonIndexByLayer(unsigned int layer)
{
    if (layer == 0 || 100 < layer)
    {
        return 0;
    }
    return m_dungeonIndexOfLayer[layer];
}