// ============================================================================
// initAICharacterScript —— ORIG 0x088c98d4（init.md:1838 +0x2483；失败 TRACE
// "- Load AI Character fail" 0x1dd4）。同模式，目录 0x94f6920/列表 0x94f6a20。
// ============================================================================
// [b28d-solo AE 43/43] __builtin_strcat 内建展开（同 initExpertJobScript；旧
// N(P209/P75) 判词作废）。
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
}