// ORIG 0x08a97438：先整表装入局部 STScriptFileList，再按条目分Job 装载；
// idx > 10 的条目跳过；任一分表装载失败即整体失败。
// [b28d-solo-InitScriptProducers_4 AE] P160：删具名 idx 局部，(*it).first 二次 deEv 直读；129/129。
bool InitSkillScript(const char* dir, const char* list)
{
    strncpy(g_szSkillScriptBaseDirectory, dir, 0x100);
    if (g_szSkillScriptBaseDirectory[strlen(g_szSkillScriptBaseDirectory) - 1] != '\\' &&
        g_szSkillScriptBaseDirectory[strlen(g_szSkillScriptBaseDirectory) - 1] != '/')
    {
        __builtin_strcat(g_szSkillScriptBaseDirectory, "\\");
    }

    STScriptFileList files;
    if (!loadRDARScriptListFile(&files, g_szSkillScriptBaseDirectory, list, 0))
        return false;

    // [b28d-lane16 P167/P28] ORIG 发非 const `_Rb_tree_iterator` 的 deEv/ppEi/ne
    // （p167 MISSING -3 deEv/ppEi/ne，EXTRA +2 const_iterator 转换构造 +ptEv/ppEv）
    // ⇒ 源形必须是非 const iterator + `(*it)` + 后置 `it++`。
    for (std::map<int, std::string>::iterator it = files.m_fileMap.begin();
         it != files.m_fileMap.end(); it++)
    {
        if ((*it).first > 10)               // ORIG cmp $0xa,setg → continue
            continue;
        // ORIG 目标元素地址 = 0x9500820 + (*it).first*0x18（二次 deEv，无具名 idx）
        if (!loadRDARScriptListFile(&g_staSkillScriptFileList[(*it).first],
                                    g_szSkillScriptBaseDirectory,
                                    (*it).second.c_str(), 0))
            return false;
    }
    return true;
}