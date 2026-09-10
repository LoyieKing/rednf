// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CLeadingChannelEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 引导频道事件 CLeadingChannelEvent 实现。
// 报告：docs/class_func_reports/CLeadingChannelEvent/ 全部 6 个方法
// （CLeadingChannelEvent / ~CLeadingChannelEvent / ~CLeadingChannelEvent_0811fdba /
//  StartEvent / StartEvent_0811fe34 / EndEvent）。
#include "CLeadingChannelEvent.h"

#include <iostream>          // std::ios_base::Init 静态初始化（ORIG _GLOBAL__I__ZN20CLeadingChannelEventC2Ev 0x0811ff2d）

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x0811fd64：基类构造 + vptr 置位 + m_channelFactor = 100。
#include "CLeadingChannelEvent/CLeadingChannelEvent.inc"

// ORIG 0x0811fd8a / 0x0811fdba：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
#include "CLeadingChannelEvent/dtor_CLeadingChannelEvent.inc"

// ---------------------------------------------------------------------------
// ORIG 0x0811fdd8：SetEventFlag(true) + m_channelFactor = 100 +
// cMyTrace(__FUNCTION__,0x13,0)("Leading Channel Event Start!")。
#include "CLeadingChannelEvent/StartEvent.inc"

// ---------------------------------------------------------------------------
// ORIG 0x0811fe34：SetEventFlag(true) + m_channelFactor = param.m_duration +
// cMyTrace(__FUNCTION__,0x1b,0)(同字符串)。
#include "CLeadingChannelEvent/StartEvent_1.inc"

// ---------------------------------------------------------------------------
// ORIG 0x0811fe92：SetEventFlag(false) + m_channelFactor = 100 +
// cMyTrace(__FUNCTION__,0x23,0)("Leading Channel Event End!")。
#include "CLeadingChannelEvent/EndEvent.inc"
