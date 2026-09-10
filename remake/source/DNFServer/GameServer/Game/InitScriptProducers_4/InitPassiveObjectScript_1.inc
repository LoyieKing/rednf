bool InitPassiveObjectScript(const char* dir, const char* list, std::map<int, int>* map)
{
    strncpy(g_szPassiveObjectScriptBaseDirectory, dir, 0x100);
    if (g_szPassiveObjectScriptBaseDirectory[strlen(g_szPassiveObjectScriptBaseDirectory) - 1] != '\\' &&
        g_szPassiveObjectScriptBaseDirectory[strlen(g_szPassiveObjectScriptBaseDirectory) - 1] != '/')
    {
        __builtin_strcat(g_szPassiveObjectScriptBaseDirectory, "\\");
    }
    if (!loadRDARScriptListFile(&g_stPassiveObjectScriptFileList,
                                g_szPassiveObjectScriptBaseDirectory,
                                list, map))
    {
        return 0;
    }
    return 1;
