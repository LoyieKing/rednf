// ORIG 0x089dce98：列表登记后直接透传 loadRDARScriptListFile 结果。
// [b28d-solo-InitScriptProducers_4 AE] P303 __builtin_strcat + P304 权威型 &g_stMapScriptFileList
// （去 reinterpret_cast 物化）；43/43 逐条同构。
bool InitMapScript(const char* dir, const char* list)
{
    strncpy(g_szMapScriptBaseDirectory, dir, 0x100);
    // P49 禁 TU helper（原 AppendDirSeparator 属发明符号）：按 ORIG 汇编内联
    // strlen×3 + memcpy 2B "\\"，无空串防御（ORIG len==0 读 buf[-1] 原始 UB，P140）。
    if (g_szMapScriptBaseDirectory[strlen(g_szMapScriptBaseDirectory) - 1] != '\\' &&
        g_szMapScriptBaseDirectory[strlen(g_szMapScriptBaseDirectory) - 1] != '/')
    {
        __builtin_strcat(g_szMapScriptBaseDirectory, "\\");
    }
    if (!loadRDARScriptListFile(&g_stMapScriptFileList,
                                g_szMapScriptBaseDirectory, list, 0))
    {
        return 0;
    }
    return 1;
}