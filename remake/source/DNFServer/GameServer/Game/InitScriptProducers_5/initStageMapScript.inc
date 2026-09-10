bool initStageMapScript(char* dir, char* list)
{
    strncpy(g_stageMapScriptBaseDirectory, dir, 0x100);
    if (g_stageMapScriptBaseDirectory[strlen(g_stageMapScriptBaseDirectory) - 1] != '\\' &&
        g_stageMapScriptBaseDirectory[strlen(g_stageMapScriptBaseDirectory) - 1] != '/')
    {
        __builtin_strcat(g_stageMapScriptBaseDirectory, "\\");
    }
    if (!loadRDARScriptListFile(&g_stageMapScriptFileList,
                                g_stageMapScriptBaseDirectory, list, 0))
        return 0;
    return 1;
