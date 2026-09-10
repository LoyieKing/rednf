BlueMarbleMapScript* BlueMarbleScriptManager::getMap(int idx)
{
    std::map<int, BlueMarbleMapScript*>::iterator it = m_maps.find(idx);
    return it != m_maps.end() ? it->second : 0;
