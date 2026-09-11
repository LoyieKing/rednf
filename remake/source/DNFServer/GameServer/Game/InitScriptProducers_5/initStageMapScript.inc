// ============================================================================
// initStageMapScript —— ORIG 0x088a9e6c（init.md:2565 +0x3345，紧随 worldMap
// 之后、返回值被丢弃）。目录 0x93f4900/列表 0x9494e20。消费方 StageMapList 域。
// ============================================================================
// [b28d-solo AE 43/43] __builtin_strcat 内建展开（旧 N(P209/P75) 判词作废）。
bool initStageMapScript(char* dir, char* list)
{
    strncpy(g_stageMapScriptBaseDirectory, dir, 0x100);
    if (g_stageMapScriptBaseDirectory[strlen(g_stageMapScriptBaseDirectory) - 1] != '\\' &&
        g_stageMapScriptBaseDirectory[strlen(g_stageMapScriptBaseDirectory) - 1] != '/')
    {
        __builtin_strcat(g_stageMapScriptBaseDirectory, "\\");
    }
    if (!loadRDARScriptListFile(&g_stageMapScriptFileList,
                                g_stageMapScriptBaseDirectory, list, 0))
        return 0;
    return 1;
}