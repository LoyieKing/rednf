bool InitStackableScript(const char* dir, const char* list,
                         const char* priceTable)
{
    strncpy(g_szStackableScriptBaseDirectory, dir, 0x100);
    if (g_szStackableScriptBaseDirectory[strlen(g_szStackableScriptBaseDirectory) - 1] != '\\' &&
        g_szStackableScriptBaseDirectory[strlen(g_szStackableScriptBaseDirectory) - 1] != '/')
    {
        __builtin_strcat(g_szStackableScriptBaseDirectory, "\\");
    }

    if (!loadRDARScriptListFile(&g_stStackableScriptFileList,
                                g_szStackableScriptBaseDirectory, list, 0))
        return false;
    if (!loadRDARScriptFile(g_szStackableScriptBaseDirectory, priceTable))
        return false;

    // ORIG 两个外层循环复用同一 -0x14 槽（P160）；首循环双初始化（P191）。
    int i = 0;
    for (i = 0; i <= 29; ++i)
        for (int j = 0; j <= 99; ++j)
            STStackableScript::priceTable_[i][j] = 0;

    std::string token;
    for (i = 0; i <= 29; ++i)
    {
        if (!ScanType(token, true))         // 无更多行 → 正常成功返回
            break;
        for (int j = 0; j <= 99; ++j)
            STStackableScript::priceTable_[i][j] = ScanInt((bool*)0);
    }
    return true;
