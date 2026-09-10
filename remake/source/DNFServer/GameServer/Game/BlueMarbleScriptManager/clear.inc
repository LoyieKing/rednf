void BlueMarbleScriptManager::clear()
{
    m_mapFileList.Clear();     // ORIG STScriptFileList::Clear
    m_tileFileList.Clear();
    m_tiles.clear();
    m_maps.clear();
