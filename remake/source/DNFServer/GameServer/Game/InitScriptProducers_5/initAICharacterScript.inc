bool initAICharacterScript(const char* dir, const char* list)
{
    strncpy(g_aiCharacterScriptBaseDirectory, dir, 0x100);
    if (g_aiCharacterScriptBaseDirectory[strlen(g_aiCharacterScriptBaseDirectory) - 1] != '\\' &&
        g_aiCharacterScriptBaseDirectory[strlen(g_aiCharacterScriptBaseDirectory) - 1] != '/')
    {
        __builtin_strcat(g_aiCharacterScriptBaseDirectory, "\\");
    }
    if (!loadRDARScriptListFile(&g_aiCharacterScriptFileList,
                                g_aiCharacterScriptBaseDirectory, list, 0))
        return 0;
    return 1;
