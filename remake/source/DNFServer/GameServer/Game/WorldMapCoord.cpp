// ============================================================================
// WorldMapCoord.cpp —— df_game_r 还原（WorldMap 坐标查询域，独占 TU）。
// ORIG nm 真实 T 符号，逐指令对照
// docs/class_func_reports/_lt_global_gt_/GetWorldMapCoordFromTable.md /
// GetWorldMapCoordFromTable2ND.md：
//   GetWorldMapCoordFromTable   _Z25GetWorldMapCoordFromTableiiR17STWorldmapPattern   0x088e5ef8 T
//   GetWorldMapCoordFromTable2ND _Z28GetWorldMapCoordFromTable2NDiR20STWorldmapPattern2ND 0x088e5f44 T
// 静态表（WorldMapScript::aWorldMapPatternTable_E B 0x09501020 / 0x4b0、
// aWorldMapPatternTable2ND_E B 0x095014e0）定义于 InitScriptProducers_5.cpp。
// ============================================================================

#include "WorldMapScript.h"

void GetWorldMapCoordFromTable(int a, int b, STWorldmapPattern& out)
{
    // ORIG 0x088e5ef8：early-return 双出口（jle→nop+jmp / jle→nop）——
    // a>0 && b>0 时经 STWorldmapPattern::operator=（0x088f2e04）整块拷贝
    // table[(a-1)*400 + (b-1)*0x28]（[3][10] 行 stride 400B）；否则 out 不变。
    if (a <= 0)
        return;
    if (b <= 0)
        return;
    out = WorldMapScript::aWorldMapPatternTable_[a - 1][b - 1];
}

void GetWorldMapCoordFromTable2ND(int a, STWorldmapPattern2ND& out)
{
    // ORIG 0x088e5f44：js→nop 出口 + body 后 jmp→pop（if 块 + 显式 return）——
    // a>=0 时逐字段复制 table2ND[a]（7 int，0x1c 字节，隐式 operator= 展开无 call）。
    if (a >= 0)
        out = WorldMapScript::aWorldMapPatternTable2ND_[a];
    return;
}
