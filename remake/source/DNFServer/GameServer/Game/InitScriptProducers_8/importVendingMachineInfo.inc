// ============================================================================
// importVendingMachineInfo —— ORIG 0x0890b74e
// 列表文件 → map<idx,name>；逐项构造 stVendingMachine_t{键, Core 解析}，
// 成功 push_back，Core 失败返回 0x1e7c。空列表 0x1e73，列表加载失败 0x1e83。
// ============================================================================
// [b28d-solo-InitScriptProducers_8 AE] ORIG 0890b74e：clean 118/118（ORIG 118 insns）；
// p167 CLEAR。已对齐：P214①/P257 正向包裹、iterator 双缓存、Core 结果内联实参（P0）。
int importVendingMachineInfo(const char* dir, const char* list,
                             std::vector<stVendingMachine_t>& out)
{
    STScriptFileList fileList;
    // P214①/P257：正向包裹复现 ORIG `call; test %al,%al; je <尾部失败块>`
    // （4.4 对 `if(!boolcall())` 恒产 xor $1 + 就地内联失败块）。
    if (loadRDARScriptListFile(&fileList, dir, list, 0))
    {
        if (fileList.m_fileMap.empty())
            return 0x1e73;

        // ORIG 70-72：++it 紧跟 push_back（body 内语句，先于 vm 析构）⇒ while 形。
        std::map<int, std::string>::iterator
            it = fileList.m_fileMap.begin(),
            end = fileList.m_fileMap.end();
        while (it != end)
        {
            stVendingMachine_t vm;                     // ctor：仅构造 +0x04 vector
            vm.m_id = it->first;
            // P0：ORIG 无 rc 槽往返（call; test %eax; setne）⇒ 内联实参。
            if (importVendingMachineInfoCore(dir, it->second.c_str(), vm) != 0)
                return 0x1e7c;
            out.push_back(vm);
            ++it;
        }
        return 0;
    }
    return 0x1e83;
}