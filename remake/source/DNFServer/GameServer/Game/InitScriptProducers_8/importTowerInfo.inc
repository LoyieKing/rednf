int importTowerInfo(const char* dir, const char* list,
                    std::map<unsigned int, stDeathTower_t>& out)
{
    STScriptFileList fileList;
    // P214①/P257：正向包裹复现 ORIG `call; test %al,%al; je <尾部失败块>`。
    if (loadRDARScriptListFile(&fileList, dir, list, 0))
    {
        if (fileList.m_fileMap.empty())
            return 0x1e8e;

        std::map<int, std::string>::iterator
            it = fileList.m_fileMap.begin(),
            end = fileList.m_fileMap.end();
        while (it != end)
        {
            unsigned int key;                          // ORIG -0x197c（块首声明）
            stDeathTower_t tower;
            // P312/P214③：`={0}` 触发 4.4 内联 rep stos 慢路（ORIG 20+ 条）
            char path[0xff] = {0};
            key = static_cast<unsigned int>(it->first);
            std::sprintf(path, "%s%s", dir, it->second.c_str());

            if (importDeathTowerScript(path, tower) != 0)
                return 0x1ea2;
            out.insert(std::make_pair(key, tower));
            ++it;
        }
        return 0;
    }
    return 0x1eab;
