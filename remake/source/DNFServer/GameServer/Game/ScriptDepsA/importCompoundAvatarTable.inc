int importCompoundAvatarTable(const char* dir, const char* path,
                              tagCompoundAvatarTable* table)
{
    STScriptFileList list;
    // [b28d-solo-ScriptDepsA 判词] AE（§6.2 全等）：修 (1) loadRDARScriptListFile
    // 返回类型 bool（P292，Main 已同步 STScriptFileList.cpp）；(2) ORIG 0x08903edb
    // `test %al,%al; je <0x107b 岛>`（无 xor）⇒ 正极性包裹（P214①/P257）；
    // (3) begin/end 缓存顺序 it(-0x24)→e(-0x28)（循环体 operator-> 实证）。
    if (loadRDARScriptListFile(&list, dir, path, 0))
    {
        if (list.m_fileMap.size() != 11)
            return 0x106e;

        // ORIG 0x08903f02 先 begin（it 落 -0x24，循环体 operator-> 实证）、
        // 0x08903f17 后 end（e 落 -0x28）；条件 operator!=(it,e)，增量 operator++。
        std::map<int, std::string>::iterator it = list.m_fileMap.begin();
        std::map<int, std::string>::iterator e = list.m_fileMap.end();
        while (it != e)
        {
            if (importCompoundAvatarTableCore(dir, it->second.c_str(), table) != 0)
                return 0x1075;
            ++it;
        }
        return 0;
    }
    return 0x107b;
}