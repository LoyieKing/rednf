bool PvPSkillTreeParameterScript::load_pvp_skill_tree_list()
{
    // P50 复活/P314：ORIG 0x08a5ce28 先 `_Rb_tree_iterator::C1Ev`（-0x14）再
    // `it = begin()`（begin sret -0x1c，单 dword 拷贝）；非 const iterator
    // （operator!=/operator->/operator++ 均非 const_iterator 实例，P96/P270）。
    std::map<int, std::string>::iterator it;
    it = g_pvpSkillTreeScriptFileList.m_fileMap.begin();
    for (; it != g_pvpSkillTreeScriptFileList.m_fileMap.end(); ++it)
    {
        const char* name = it->second.c_str();
        // ORIG 0x08a5ce73：`cmpl $0,-0xc; je +0xe5(nop) → 尾 mov $1` 空名跳出。
        if (name == 0)
        {
            break;
        }
        if (!importPvPSkillTreeParameterScript(name))
        {
            LogManager::logFormat(
                1, "../RDARScriptPvPSkillTree.cpp",
                __FUNCTION__, 0x45,
                "PvPSkillTreeParameterScript File Script Error: %s\n",
                name);
            return false;
        }
    }
    return true;
