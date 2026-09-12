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
            stDeathTower_t tower;
            // P312/P214③：`={0}` 触发 4.4 内联 rep stos 慢路（ORIG 20+ 条）
            char path[0xff] = {0};
            // [SemB09 s2_09] ② 库实例化修正：ORIG 0x0890b8d0 的调用目标是
            //   `_ZSt9make_pairIRKjR14stDeathTower_tE…`（_T1 = **const** unsigned int&），
            //   我方非 const 局部推出 `IRj`。两处实参装载逐条相同（均为
            //   `lea -0x197c(%ebp),%edx; mov %edx,0x4(%esp)` 引用传递）⇒ 原码该局部
            //   是 `const` 左值。改为 const 一次初始化（位置 = ORIG 的写入点）。
            const unsigned int key = static_cast<unsigned int>(it->first);
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