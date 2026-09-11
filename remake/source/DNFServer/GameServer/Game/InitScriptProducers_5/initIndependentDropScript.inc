// ============================================================================
// initIndependentDropScript —— ORIG 0x0897c01c（init.md:875 +0x1153；失败两步
// 均 TRACE "Load IndependentDrop.lst fail" 0x1b56/0x1b5c）。strncpy 0xff；
// 目录 0x94f7840/列表 0x94f7940。（头文件声明返回 int；asm 仅 eax=0/1。）
// [b28d-solo AE 43/43] __builtin_strcat 内建展开（旧 N(P209/P75) 判词作废）。
// ============================================================================
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
}