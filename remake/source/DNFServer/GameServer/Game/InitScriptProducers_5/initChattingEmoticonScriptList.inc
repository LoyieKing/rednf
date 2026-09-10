bool initChattingEmoticonScriptList(char* dir, char* list)
{
    strncpy(g_ChattingEmoticonScriptBaseDirectory, dir, 0xff);
    if (g_ChattingEmoticonScriptBaseDirectory[strlen(g_ChattingEmoticonScriptBaseDirectory) - 1] != '\\' &&
        g_ChattingEmoticonScriptBaseDirectory[strlen(g_ChattingEmoticonScriptBaseDirectory) - 1] != '/')
    {
        __builtin_strcat(g_ChattingEmoticonScriptBaseDirectory, "\\");
    }
    if (!loadRDARScriptListFile(&g_ChattingEmoticonScriptFileList,
                                g_ChattingEmoticonScriptBaseDirectory, list, 0))
        return 0;
    return 1;
