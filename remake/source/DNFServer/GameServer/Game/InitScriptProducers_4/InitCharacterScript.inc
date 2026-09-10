bool InitCharacterScript(const char* dir, const char* list,
                         const char* expTable, const char* pvpPriceTable)
{
    strncpy(g_szCharacterScriptBaseDirectory, dir, 0x100);
    if (g_szCharacterScriptBaseDirectory[strlen(g_szCharacterScriptBaseDirectory) - 1] != '\\' &&
        g_szCharacterScriptBaseDirectory[strlen(g_szCharacterScriptBaseDirectory) - 1] != '/')
    {
        __builtin_strcat(g_szCharacterScriptBaseDirectory, "\\");
    }

    if (!loadRDARScriptListFile(&g_stCharacterScriptFileList,
                                g_szCharacterScriptBaseDirectory, list, 0))
        return 0;

    // ORIG 四个 200 次循环复用同一 -0xc 槽（P160 复用 ORIG 复用的槽）
    int i = 0;                              // P191：ORIG 首循环双初始化两条 movl $0,-0xc
    for (i = 0; i <= 199; ++i)              // ORIG cmpl $0xc7,setle
        STCharacterScript::naExpTable[i] = 0;

    if (!loadRDARScriptFile(g_szCharacterScriptBaseDirectory, expTable))
        return 0;
    for (i = 0; i <= 199; ++i)
        STCharacterScript::naExpTable[i] = ScanInt((bool*)0);

    if (pvpPriceTable != 0)                 // ORIG cmpl $0x0,0x14(%ebp)
    {
        for (i = 0; i <= 199; ++i)
            STCharacterScript::pvpPrices_[i] = 0;
        if (!loadRDARScriptFile(g_szCharacterScriptBaseDirectory,
                                pvpPriceTable))
            return 0;
        for (i = 0; i <= 199; ++i)
            STCharacterScript::pvpPrices_[i] = ScanInt((bool*)0);
    }
    return 1;
