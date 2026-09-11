// ============================================================================
// initEventCharacterScript —— ORIG 0x0895ae5c（init.md:1596 +0x1f9b；失败
// TRACE "- initEventCharacterScript Fail" 0x1cd4）。目录 0x94f73c0/列表 0x94f74c0。
// [b28d-solo AE 43/43] __builtin_strcat 内建展开（旧 N(P209/P75) 判词作废）。
// ============================================================================
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
}