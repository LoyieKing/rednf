// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 stSelectQuestParam/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— stSelectQuestParam::stSelectQuestParam(CUser*)（ORIG T 0x83480b4）
//   ORIG mangled _ZN18stSelectQuestParamC1EP5CUser / _ZN18stSelectQuestParamC2EP5CUser。
//   反编译（see docs/class_func_reports/stSelectQuestParam/stSelectQuestParam.md）：
//     +0x00 = level                +0x04 = job
//     +0x08 = firstGrowType        +0x0c = secondGrowType
//     +0x10 = pvpGrade             +0x14 = powerSide
//     +0x1c = expertJobType        +0x20 = expertJobLevel
//     +0x24 = *( (char*)user->getCurCharacQuestR() + 0x75f0 )
//   布局与 CListTables.cpp 局部 struct stSelectQuestParam（0x18 pad + ...）同源，
//   本 TU 提供该构造函数的唯一定义；GameStubs 旧桩已删除。

#include "CUser.h"

struct stSelectQuestParam
{
    stSelectQuestParam(CUser* user);

    int  m_field00;   // +0x00
    int  m_field04;   // +0x04
    int  m_field08;   // +0x08
    int  m_field0c;   // +0x0c
    int  m_field10;   // +0x10
    char m_field14;   // +0x14
    char m_pad15[3];  // +0x15..0x17
    int  m_field18;   // +0x18
    int  m_field1c;   // +0x1c
    int  m_field20;   // +0x20
    int  m_field24;   // +0x24
};

#include "stSelectQuestParam/stSelectQuestParam.inc"
