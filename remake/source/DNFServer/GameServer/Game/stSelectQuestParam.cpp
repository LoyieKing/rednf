#define _GLIBCXX_IOSTREAM 1
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

// [R18 构建修复] 原处为本 TU 私有的 `struct stSelectQuestParam` 镜像（成员名 m_field00…
//   m_field24），与 Quest.h:238 的权威定义（m_level/m_job/…）构成**重复定义**；该 TU 自权威
//   定义落地起未再重编，故长期隐性（第十八轮改 GlobalData.h 触发重编后暴露）。
//   现统一到 Quest.h 唯一定义点（偏移/类型逐项相同：+0x00 int / +0x04 int / +0x08 int /
//   +0x0c int / +0x10 int / +0x14 char / +0x15 char[3] / +0x18 int / +0x1c int / +0x20 int /
//   +0x24 int），本 TU 仅提供 ctor 定义。
#include "Quest.h"

#include "stSelectQuestParam/stSelectQuestParam.inc"
