int TowerOfDespairMgr::GetBaseLayerIndex()
{
    if (m_field358 == 0)
    {
        const CDungeon* base = Get_CDungeonByLayer(1);
        if (base != 0)
        {
            m_field358 = base->get_index();
        }
    }
    return m_field358;
