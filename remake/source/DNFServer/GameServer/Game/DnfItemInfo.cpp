// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DnfItemInfo/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r DnfItemInfo 实现（OrigDataClears 批，2026-08-28）。
// ORIG：ctor W 0x84e910a / reset W 0x84e9094（objdump 逐指令转写）。
// 语义与 Guild/DNFGuild.cpp 的 Guild 域实现一致（同 mangled 名，Game 服务补齐）。
#include "DnfItemInfo.h"

#include "Inven_Item.h"   // ChronicleRandomOptionInfo（本文件唯一定义点）

#include "DnfItemInfo/DnfItemInfo.inc"


#include "DnfItemInfo/reset.inc"


#include "DnfItemInfo/reset_1.inc"


#include "DnfItemInfo/reset_2.inc"

