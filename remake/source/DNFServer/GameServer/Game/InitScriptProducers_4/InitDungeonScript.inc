bool InitDungeonScript(char* dir, char* list)
{
    strncpy(g_szDungeonScriptBaseDirectory, dir, 0x100);
    if (g_szDungeonScriptBaseDirectory[strlen(g_szDungeonScriptBaseDirectory) - 1] != '\\' &&
        g_szDungeonScriptBaseDirectory[strlen(g_szDungeonScriptBaseDirectory) - 1] != '/')
    {
        __builtin_strcat(g_szDungeonScriptBaseDirectory, "\\");
    }
    if (!loadRDARScriptListFile(&g_stDungeonScriptFileList,
                                g_szDungeonScriptBaseDirectory, list, 0))
    {
        return 0;
    }
    return 1;
