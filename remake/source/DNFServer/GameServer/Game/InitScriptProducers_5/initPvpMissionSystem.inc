bool initPvpMissionSystem(char* dir, char* list)
{
    strncpy(g_PvpMissionScriptBaseDirectory, dir, 0xff);
    if (g_PvpMissionScriptBaseDirectory[strlen(g_PvpMissionScriptBaseDirectory) - 1] != '\\' &&
        g_PvpMissionScriptBaseDirectory[strlen(g_PvpMissionScriptBaseDirectory) - 1] != '/')
    {
        __builtin_strcat(g_PvpMissionScriptBaseDirectory, "\\");
    }
    if (!loadRDARScriptListFile(&g_PvpMissionScriptFileList,
                                g_PvpMissionScriptBaseDirectory, list, 0))
        return 0;
    return 1;
