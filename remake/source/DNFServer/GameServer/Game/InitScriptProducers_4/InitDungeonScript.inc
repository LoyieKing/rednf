// ORIG 0x088e5e3c：签名 char*（调用点传字符串字面量，ORIG mangled PcS_）。
// [b28d-solo-InitScriptProducers_4 AE] 旧 [b28c-L23 N(P209/P75)] 判词作废：P303
// __builtin_strcat 复现 ORIG 9 条内建展开（strlen+memcpy）；43/43 逐条同构。
bool InitDungeonScript(char* dir, char* list)
{
    strncpy(g_szDungeonScriptBaseDirectory, dir, 0x100);
    if (g_szDungeonScriptBaseDirectory[strlen(g_szDungeonScriptBaseDirectory) - 1] != '\\' &&
        g_szDungeonScriptBaseDirectory[strlen(g_szDungeonScriptBaseDirectory) - 1] != '/')
    {
        __builtin_strcat(g_szDungeonScriptBaseDirectory, "\\");
    }
    if (!loadRDARScriptListFile(&g_stDungeonScriptFileList,
                                g_szDungeonScriptBaseDirectory, list, 0))
    {
        return 0;
    }
    return 1;
}