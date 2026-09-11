// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Inter_MonitorGuildPointDel/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— Inter_MonitorGuildPointDel 静态成员定义（ORIG W 0x82a5a4c /
//   W 0x084eb100 / B 0x943dd88）。类声明唯一声明点 InterDispatcher.h（§4.4 合并）。
//   objdump（082a5a4c）: mov 0x943dd88,%eax; ret  —— 直接返回静态 g_lastDeleteTime
//   （ORIG B _ZN26Inter_MonitorGuildPointDel16g_lastDeleteTimeE @ 0x943dd88）。
//   ORIG mangled _ZN26Inter_MonitorGuildPointDel17getLastDeleteTimeEv。
// 类声明见 Inter_MonitorGuildPointDel.h（唯一声明点）。

#include "Inter_MonitorGuildPointDel.h"

// [L9·2026-09-11] g_lastDeleteTime 的**带初值定义已迁至 InterDispatcher.cpp**（ORIG 归属实证：
// ORIG static_init 实例 0x084e9004 尾部 `call 0x84ce6d2 <..._makeDefaultTimeEv>` + `mov ds:0x943dd88,eax`，
// 由 GI `_GLOBAL__I__Z7getUserj` @0x084e904e 调用）。本文件**不得**再定义该符号（否则重复定义且丢初值）。

#include "Inter_MonitorGuildPointDel/getLastDeleteTime.inc"


#include "Inter_MonitorGuildPointDel/setLastDeleteTime.inc"

