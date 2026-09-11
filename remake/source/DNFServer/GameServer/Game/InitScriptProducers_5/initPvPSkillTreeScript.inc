// ============================================================================
// initPvPSkillTreeScript —— ORIG 0x08a5cd6c（init.md:4968 +0x6507；失败 TRACE
// "- Load PvPSkillTree.lst fail" 0x21d7）。strncpy 长度 0xff；
// 目录 0x9500240/列表 0x9500340。消费方 PvPSkillTreeParameterScript 域。
// [b28d-solo AE 43/43] __builtin_strcat 内建展开（旧 N(P209/P75) 判词作废）。
// ============================================================================
bool initPvPSkillTreeScript(char* dir, char* list)
{
    strncpy(g_pvpSkillTreeScriptBaseDirectory, dir, 0xff);
    if (g_pvpSkillTreeScriptBaseDirectory[strlen(g_pvpSkillTreeScriptBaseDirectory) - 1] != '\\' &&
        g_pvpSkillTreeScriptBaseDirectory[strlen(g_pvpSkillTreeScriptBaseDirectory) - 1] != '/')
    {
        __builtin_strcat(g_pvpSkillTreeScriptBaseDirectory, "\\");
    }
    if (!loadRDARScriptListFile(&g_pvpSkillTreeScriptFileList,
                                g_pvpSkillTreeScriptBaseDirectory, list, 0))
        return 0;
    return 1;
}