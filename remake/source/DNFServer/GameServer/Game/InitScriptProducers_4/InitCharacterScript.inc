// ORIG 0x08a0cc3c：Character.lst 登记 → ExpTable.tbl 回填 naExpTable[200] →
// 第 4 参非 NULL 时再加载对应 pvp 价格表回填 pvpPrices_[200]。
// （DataManagerInitDepends.h 第 4 参注释作 unused，与 ORIG 0x14(%ebp) 分支
//  不符——真实语义为可选 pvp 价格表路径，此处按 ORIG 实现。）
// [b28d-solo-InitScriptProducers_4 AE] P160/P191：函数级 int i=0 复用 -0xc 槽 + 首循环双初始化；107/107。
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
}