// ORIG 0x088d66b6 T：m_maps（this+0xd4）find，未命中返回 0。
BlueMarbleMapScript* BlueMarbleScriptManager::getMap(int idx)
{
    std::map<int, BlueMarbleMapScript*>::iterator it = m_maps.find(idx);
    return it != m_maps.end() ? it->second : 0;
}