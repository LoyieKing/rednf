// ORIG 0x88d622c T：清两个文件列表 map 与 m_tiles/m_maps（不删指针、不触
// m_info——与 ORIG clear 逐段一致）。
void BlueMarbleScriptManager::clear()
{
    m_mapFileList.Clear();     // ORIG STScriptFileList::Clear
    m_tileFileList.Clear();
    m_tiles.clear();
    m_maps.clear();
}