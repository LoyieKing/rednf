bool initItemShopScript(char* dir, char* list)
{
    strncpy(g_itemShopScriptBaseDirectory, dir, 0x100);
    if (g_itemShopScriptBaseDirectory[strlen(g_itemShopScriptBaseDirectory) - 1] != '\\' &&
        g_itemShopScriptBaseDirectory[strlen(g_itemShopScriptBaseDirectory) - 1] != '/')
    {
        __builtin_strcat(g_itemShopScriptBaseDirectory, "\\");
    }
    if (!loadRDARScriptListFile(&g_itemShopScriptFileList,
                                g_itemShopScriptBaseDirectory, list, 0))
        return 0;
    return 1;
