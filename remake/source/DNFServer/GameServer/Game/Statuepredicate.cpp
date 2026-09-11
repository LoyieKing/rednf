// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Statuepredicate/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// Statuepredicate.cpp — ORIG 真实实现（独立 TU，未入 CMake，待 Main 登记）。
// ORIG 证据（df_game_r elf32-i386 objdump）：
//   C1 @0x08a6c186：call unary_function::C1（0x8a6c192）→ 以 eax 缓存 this，
//     将按值实参 0xc(%ebp)/0x10(%ebp)/0x14(%ebp) 三字分别 mov 至 this+0x0/+0x4/+0x8
//     （P29 初始化列表形态）。
//   operator() @0x08a6c1ae：先比 this->m_appendageType==arg.m_appendageType，
//     再比 this->m_valueType==arg.m_valueType，任一不等 → mov $0；全等 → mov $1
//     （P56 分支形态，勿直 return 表达式）。不比较 m_value。
#include "Statuepredicate.h"
#include "CDataManager.h"   // pieceQuestRewardStatus 完整定义

#include "Statuepredicate/Statuepredicate.inc"


#include "Statuepredicate/operator.inc"

