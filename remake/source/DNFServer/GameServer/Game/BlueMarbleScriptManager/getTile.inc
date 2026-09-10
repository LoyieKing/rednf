BlueMarbleTileScript* BlueMarbleScriptManager::getTile(int zone)
{
    std::map<int, BlueMarbleTileScript*>::iterator it = m_tiles.find(zone);
    return it != m_tiles.end() ? it->second : 0;
