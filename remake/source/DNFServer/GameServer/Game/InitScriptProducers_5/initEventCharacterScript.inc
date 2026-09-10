bool initEventCharacterScript(char* dir, char* list)
{
    strncpy(g_eventCharacterScriptBaseDirectory, dir, 0x100);
    if (g_eventCharacterScriptBaseDirectory[strlen(g_eventCharacterScriptBaseDirectory) - 1] != '\\' &&
        g_eventCharacterScriptBaseDirectory[strlen(g_eventCharacterScriptBaseDirectory) - 1] != '/')
    {
        __builtin_strcat(g_eventCharacterScriptBaseDirectory, "\\");
    }
    if (!loadRDARScriptListFile(&g_eventCharacterScriptFileList,
                                g_eventCharacterScriptBaseDirectory, list, 0))
        return 0;
    return 1;
