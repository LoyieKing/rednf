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
