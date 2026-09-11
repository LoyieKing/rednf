// ============================================================================
// TowerOfDespairMgr::Get_CDungeonByLayer —— ORIG T 0x08643d14
//   （报告 TowerOfDespairMgr/Get_CDungeonByLayer.md）
// ============================================================================
// [b28d-Main·检查点17] 返回类型 CDungeon*→const CDungeon*（元素 const 指针，mangled 不变）。
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
}