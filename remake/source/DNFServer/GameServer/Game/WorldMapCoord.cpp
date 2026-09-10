// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 WorldMapCoord/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "WorldMapScript.h"

#include "WorldMapCoord/GetWorldMapCoordFromTable.inc"
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

#include "WorldMapCoord/GetWorldMapCoordFromTable2ND.inc"
{
    // ORIG 0x088e5f44：js→nop 出口 + body 后 jmp→pop（if 块 + 显式 return）——
    // a>=0 时逐字段复制 table2ND[a]（7 int，0x1c 字节，隐式 operator= 展开无 call）。
    if (a >= 0)
        out = WorldMapScript::aWorldMapPatternTable2ND_[a];
    return;
}
