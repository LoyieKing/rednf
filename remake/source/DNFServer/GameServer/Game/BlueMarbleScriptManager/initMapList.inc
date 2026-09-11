// ORIG T 0x088d6438：同 initTileList，作用于 m_mapFileList/m_maps 与
// BlueMarbleMapScript（import 失败时 ORIG 先析构再 delete）。
bool BlueMarbleScriptManager::initMapList(const char* dir, const char* list)
{
    if (!loadRDARScriptListFile(&m_mapFileList, dir, list, 0))
    {
        return false;
    }

    m_maps.clear();
    // b28d-96/270：同 initTileList——ORIG 非 const 迭代器。
    for (std::map<int, std::string>::iterator it = m_mapFileList.m_fileMap.begin();
         it != m_mapFileList.m_fileMap.end(); ++it)
    {
        BlueMarbleMapScript* map = new BlueMarbleMapScript;
        const int key = it->first;
        std::string path(dir);
        path += it->second;
        // b28d-59/168a①：同 initTileList（否定早返形）。
        if (!map->importScript(path.c_str()))
        {
            // [b29 R20] ORIG 0x088d656c/0x088d6572 双判空（delete 判空 + dtor 判空）；
            // 我方只有 dtor 判空 ⇒ 补外层 if（P141 反向使用，ORIG 确有该守卫）。
            if (map != 0)
                delete map;
            map = 0;
        }
        else
        {
            m_maps[key] = map;
        }
    }
    return true;
}