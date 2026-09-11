// ============================================================================
// initPvpMissionSystem —— ORIG 0x08a5aad5（init.md:2219 +0x2c50；失败 TRACE
// "- Load PVP_Mission.lst fail" 0x1eab）。strncpy 长度 0xff；
// 目录 0x9500100/列表 0x9500200（消费方 PvP_MissionSystem.cpp）。
// ============================================================================
// [b28d-solo AE 43/43] __builtin_strcat 内建展开（旧 N(P209/P75) 判词作废）。
bool initPvpMissionSystem(char* dir, char* list)
{
    strncpy(g_PvpMissionScriptBaseDirectory, dir, 0xff);
    if (g_PvpMissionScriptBaseDirectory[strlen(g_PvpMissionScriptBaseDirectory) - 1] != '\\' &&
        g_PvpMissionScriptBaseDirectory[strlen(g_PvpMissionScriptBaseDirectory) - 1] != '/')
    {
        __builtin_strcat(g_PvpMissionScriptBaseDirectory, "\\");
    }
    if (!loadRDARScriptListFile(&g_PvpMissionScriptFileList,
                                g_PvpMissionScriptBaseDirectory, list, 0))
        return 0;
    return 1;
}