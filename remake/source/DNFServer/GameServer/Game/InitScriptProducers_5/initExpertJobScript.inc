bool initExpertJobScript(char* dir, char* list)
{
    strncpy(g_expertJobScriptBaseDirectory, dir, 0x100);
    // P49 禁 TU helper：内联 strlen×2 条件 + 内建 strcat 展开（无空串防御，
    // ORIG len==0 UB，P140）。
    if (g_expertJobScriptBaseDirectory[strlen(g_expertJobScriptBaseDirectory) - 1] != '\\' &&
        g_expertJobScriptBaseDirectory[strlen(g_expertJobScriptBaseDirectory) - 1] != '/')
    {
        __builtin_strcat(g_expertJobScriptBaseDirectory, "\\");
    }
    if (!loadRDARScriptListFile(&g_expertJobScriptFileList,
                                g_expertJobScriptBaseDirectory, list, 0))
        return 0;
    return 1;
