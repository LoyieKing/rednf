// ============================================================================
// initItemShopScript —— ORIG 0x089dc4f8（init.md:2178 +0x2b71；失败 TRACE
// "- Load ItemShop.lst fail" 0x1e94）。目录 0x94fe160/列表 0x94fe260
// （消费方 CItemShop.cpp Load_list / FindIndexByFullScan）。
// ============================================================================
// [b28d-solo AE 43/43] __builtin_strcat 内建展开（旧 N(P209/P75) 判词作废）。
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
}