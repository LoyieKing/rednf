// initWorldMapScript —— ORIG 0x08ab000c（init.md:2555 +0x3314；失败路径直接
// return false 由 init 终止）。四步：
// ① 目录拷贝（0x9500f00，0x100）+ loadRDARScriptListFile(0x9501000)；
// ② loadRDARScriptFile(baseDir,"WorldMapPatternTable.tbl")（串 0x8e2bd63）；
// ③ 三重循环写 aWorldMapPatternTable_E：外层 i=0..2 每次 ScanType(token,true)
//   失败即跳出（仍继续第④步）；中层 j=0..9 先 name=token，再 k=0..2/l=0..1
//   ScanInt 写 coord[k][l]；
// ④ 调 loadWorldMapCoordPattern() 后返回 true。
// [b28d-solo AE 145/145] __builtin_strcat 内建展开 + P232 两死槽（见下方注释）。
// ============================================================================
bool initWorldMapScript(char* dir, char* list)
{
    strncpy(g_worldMapScriptBaseDirectory, dir, 0x100);
    if (g_worldMapScriptBaseDirectory[strlen(g_worldMapScriptBaseDirectory) - 1] != '\\' &&
        g_worldMapScriptBaseDirectory[strlen(g_worldMapScriptBaseDirectory) - 1] != '/')
    {
        __builtin_strcat(g_worldMapScriptBaseDirectory, "\\");
    }
    if (!loadRDARScriptListFile(&g_worldMapScriptFileList,
                                g_worldMapScriptBaseDirectory, list, 0))
        return false;
    if (!loadRDARScriptFile(g_worldMapScriptBaseDirectory,
                            "WorldMapPatternTable.tbl"))
        return false;

    std::string token;
    // P232/P171 死局部复现：ORIG 0x08ab00f7/0x08ab00fe 在 string ctor 之后对
    // -0x30/-0x2c 各写一次 `movl $0x0` 且全函数无读取（原码遗留计数槽）⇒ 补两个
    // 未使用零初始化 int 精确复原槽位（token→-0x34，i/j/k/l→-0x28..-0x1c）。
    int legacyCountA = 0;
    int legacyCountB = 0;
    for (int i = 0; i <= 2; ++i)
    {
        if (!ScanType(token, true))
            break;                       // ORIG jne 0x8ab01f7 → 直达 ④
                for (int j = 0; j <= 9; ++j)
        {
            // ORIG 0x8ab013b/0x8ab017a：完整表达式逐次重算 i/j 偏移
            // （(i*50+j*5+k)*2+l 复合索引），不缓存条目引用。
            WorldMapScript::aWorldMapPatternTable_[i][j].m_str0 = token;
            for (int k = 0; k <= 2; ++k)
            {
                for (int l = 0; l <= 1; ++l)
                {
                    WorldMapScript::aWorldMapPatternTable_[i][j]
                        .m_coord.m_coord[k][l] = ScanInt((bool*)0);
                }
            }
        }
    }
    loadWorldMapCoordPattern();
    return true;
}