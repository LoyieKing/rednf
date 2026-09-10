bool TowerOfDespairMgr::IsTowerOfDespairDungeon_ByIndex(int idx) const
{
    std::set<int>::const_iterator it = m_layerSet.find(idx);
    return m_layerSet.end() != it;
