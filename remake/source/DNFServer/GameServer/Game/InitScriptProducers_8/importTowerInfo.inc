// ============================================================================
// importTowerInfo —— ORIG 0x0890b8d0
// 列表文件遍历：sprintf("%s%s") 拼 per-file 路径 → importDeathTowerScript →
// insert(key, tower)。列表加载失败 0x1eab、空列表 0x1e8e、单文件失败 0x1ea2。
// ============================================================================
// [b28d-solo-InitScriptProducers_8 AE] ORIG 0890b8d0：clean 160/160（ORIG 160 insns）；
// p167 CLEAR。已对齐：P214① 正向包裹、sprintf 路径拼接、importDeathTowerScript +
// insert(key,tower)、三档错误码 0x1eab/0x1e8e/0x1ea2。
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
}