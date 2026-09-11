// ORIG 0x08991601：列表登记 → pricetable.tbl 清零回填 → 头 3 个价格系数 →
// "[repair cost]" 可选段 → 30 行×100 列主表。
// [b28d-solo-InitScriptProducers_4 AE] P303 + P160（函数级 i 复用 -0x14）+ P191 首循环双初始化
// + 删死 bool ok；152/152。
bool InitEquipmentScript(const char* dir, const char* list,
                         const char* priceTable)
{
    strncpy(g_szEquipmentScriptBaseDirectory, dir, 0x100);
    if (g_szEquipmentScriptBaseDirectory[strlen(g_szEquipmentScriptBaseDirectory) - 1] != '\\' &&
        g_szEquipmentScriptBaseDirectory[strlen(g_szEquipmentScriptBaseDirectory) - 1] != '/')
    {
        __builtin_strcat(g_szEquipmentScriptBaseDirectory, "\\");
    }

    if (!loadRDARScriptListFile(&g_stEquipmentScriptFileList,
                                g_szEquipmentScriptBaseDirectory, list, 0))
        return false;
    if (!loadRDARScriptFile(g_szEquipmentScriptBaseDirectory, priceTable))
        return false;

    // ORIG 三个外层循环复用同一 -0x14 槽（P160）；首循环双初始化（P191）。
    // 两个内层 j 为独立作用域 ⇒ 各自占槽（ORIG -0x10 / -0xc）。
    int i = 0;
    for (i = 0; i <= 29; ++i)               // ORIG cmpl $0x1d,setle
        for (int j = 0; j <= 99; ++j)       // ORIG cmpl $0x63,setle
            STEquipmentScript::priceTable_[i][j] = 0;

    std::string token;
    ScanType(token, true);                  // 首标记（结果不校验）
    for (i = 0; i <= 2; ++i)                // ORIG cmpl $0x2,setle
        itemPriceRates_[i] = ScanInt((bool*)0);

    ScanType(token, true);                  // ORIG 0x8e1602b "[repair cost]"
    if (token == "[repair cost]")
        itemRepairCost_ = ScanFloat((bool*)0);

    for (i = 0; i <= 29; ++i)               // ScanType 失败即正常收尾
    {
        if (!ScanType(token, true))
            break;
        for (int j = 0; j <= 99; ++j)       // ORIG ScanInt(NULL)
            STEquipmentScript::priceTable_[i][j] = ScanInt((bool*)0);
    }
    return true;
}