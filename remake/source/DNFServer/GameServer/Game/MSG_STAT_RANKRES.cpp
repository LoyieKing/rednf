// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 MSG_STAT_RANKRES/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ 迁移 TU —— MSG_STAT_RANKRES 构造/析构（2026-08-21）。
// 权威声明见 MSG_STAT_RANKRES.h（Statistics.cpp / SigTypes.h 统一 include）。
// ORIG：
//   MSG_STAT_RANKRES::MSG_STAT_RANKRES()  0x08610798 —— 仅构造 +0x1c 的 m_result
//                                                 （call _ZN14GameResultTypeC1Ev）
//   MSG_STAT_RANKRES::~MSG_STAT_RANKRES() 0x086107ae —— 仅析构 +0x1c 的 m_result
//                                                 （call _ZN14GameResultTypeD1Ev）
// 前置字段（m_a..m_g）由使用方（Statistics::RankResult）在 SendSignal 前填充，ctor 不 init。
#include "MSG_STAT_RANKRES.h"

// 值成员 m_result 由编译器默认构造（ORIG 仅 call GameResultType::C1，无
// placement new/异常路径）；析构同理（call GameResultType::D1）。
#include "MSG_STAT_RANKRES/MSG_STAT_RANKRES.inc"
{
}

#include "MSG_STAT_RANKRES/dtor_MSG_STAT_RANKRES.inc"
{
}
