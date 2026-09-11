// ============================================================================
// loadWorldMapCoordPattern —— ORIG 0x08ab0234（0x154 字节；报告
// _lt_global_gt_/loadWorldMapCoordPattern.md）。
// ① loadRDARScriptFile(baseDir,"WorldMapPatternTable2ND.tbl")
//   （串 0x8e2bd7c）失败 return false；
// ② 循环 i=0..0x13（20 条）：ScanAbsOrRel(isAbs,token,got,true) 返回 float，
//   fstcw/fldcw(ah=0xc)+fistp 即 C 截断 (int)→entry[i].m_base；
//   got==false 时跳出；随后 j=0..2/k=0..1 ScanInt 写 entry[i].m_coord[j][k]；
// ③ return gotValue（最后一次扫描结果）。
// ============================================================================
bool loadWorldMapCoordPattern(void)
{
    if (!loadRDARScriptFile(g_worldMapScriptBaseDirectory,
                            "WorldMapPatternTable2ND.tbl"))
        return false;

    bool isAbs = true;                   // ebp-0x29 初值 1
    bool gotValue = false;               // ebp-0x2a 初值 0
    std::string token;
    for (int i = 0; i <= 0x13; ++i)
    {
        int base = (int)ScanAbsOrRel(isAbs, token, gotValue, true);
        if (!gotValue)
            break;                       // test al,al; jne 0x8ab0351
        WorldMapScript::aWorldMapPatternTable2ND_[i].m_base = base;
        for (int j = 0; j <= 2; ++j)
        {
            for (int k = 0; k <= 1; ++k)
            {
                WorldMapScript::aWorldMapPatternTable2ND_[i].m_coord[j][k] =
                    ScanInt((bool*)0);
            }
        }
    }
    return gotValue;                     // movzx ebx,[ebp-0x2a]
}