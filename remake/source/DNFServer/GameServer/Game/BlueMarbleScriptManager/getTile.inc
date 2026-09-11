// ORIG 0x088d6646 T：m_tiles（this+0xbc）find，未命中返回 0。
BlueMarbleTileScript* BlueMarbleScriptManager::getTile(int zone)
{
    std::map<int, BlueMarbleTileScript*>::iterator it = m_tiles.find(zone);
    return it != m_tiles.end() ? it->second : 0;
}