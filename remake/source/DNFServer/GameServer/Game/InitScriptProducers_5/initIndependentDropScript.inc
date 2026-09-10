bool initIndependentDropScript(char* dir, char* list)
{
    strncpy(g_independentDropScriptBaseDirectory, dir, 0xff);
    if (g_independentDropScriptBaseDirectory[strlen(g_independentDropScriptBaseDirectory) - 1] != '\\' &&
        g_independentDropScriptBaseDirectory[strlen(g_independentDropScriptBaseDirectory) - 1] != '/')
    {
        __builtin_strcat(g_independentDropScriptBaseDirectory, "\\");
    }
    if (!loadRDARScriptListFile(&g_independentDropScriptFileList,
                                g_independentDropScriptBaseDirectory, list, 0))
        return 0;
    return 1;
