// ORIG 0x08a3c5c2（0xbb 字节）：与 2 参数版同构，仅把 map 透传
// loadRDARScriptListFile 第 4 参（ORIG 0x8a3c646 mov 0x10(%ebp)，
// 2 参数版同址为 movl $0）。
// [b28d-solo-InitScriptProducers_4 AE] 3 参版同构（第 4 参 map 透传）；44/44。
bool InitPassiveObjectScript(const char* dir, const char* list, std::map<int, int>* map)
{
    strncpy(g_szPassiveObjectScriptBaseDirectory, dir, 0x100);
    if (g_szPassiveObjectScriptBaseDirectory[strlen(g_szPassiveObjectScriptBaseDirectory) - 1] != '\\' &&
        g_szPassiveObjectScriptBaseDirectory[strlen(g_szPassiveObjectScriptBaseDirectory) - 1] != '/')
    {
        __builtin_strcat(g_szPassiveObjectScriptBaseDirectory, "\\");
    }
    if (!loadRDARScriptListFile(&g_stPassiveObjectScriptFileList,
                                g_szPassiveObjectScriptBaseDirectory,
                                list, map))
    {
        return 0;
    }
    return 1;
}