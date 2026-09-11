// ORIG 0x08a29ead：与 InitMapScript 完全同构（不同全局）。
// [b28d-solo-InitScriptProducers_4 AE] 同 InitMapScript：P303 + P304 权威型；43/43。
bool InitPassiveObjectScript(const char* dir, const char* list)
{
    strncpy(g_szPassiveObjectScriptBaseDirectory, dir, 0x100);
    if (g_szPassiveObjectScriptBaseDirectory[strlen(g_szPassiveObjectScriptBaseDirectory) - 1] != '\\' &&
        g_szPassiveObjectScriptBaseDirectory[strlen(g_szPassiveObjectScriptBaseDirectory) - 1] != '/')
    {
        __builtin_strcat(g_szPassiveObjectScriptBaseDirectory, "\\");
    }
    if (!loadRDARScriptListFile(&g_stPassiveObjectScriptFileList,
                                g_szPassiveObjectScriptBaseDirectory,
                                list, 0))
    {
        return 0;
    }
    return 1;
}