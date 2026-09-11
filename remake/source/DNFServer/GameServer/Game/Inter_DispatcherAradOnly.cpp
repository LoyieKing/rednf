#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Inter_DispatcherAradOnly/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— Inter_DispatcherAradOnly 实现（报告 docs/class_func_reports/
// Inter_DispatcherAradOnly/）。C1 0x084e7eb8：基类 ISigDispatcher() + 设 vptr +
// Arad_InternalDispatcher C1 + Init()；dispatch_sig 0x084e7f2c：读 data 前 4
// 字节 code → GetDispatcher → 空 cMyTrace(0x6ba2,5)+getCurCharacNo 返 0x6ba3
// → 否则虚调槽2 dispatch_sig(data+4, len-4)。
#include "Inter_DispatcherAradOnly.h"

#include "GameTypes.h"  // cMyTrace
#include "CUser.h"      // CUserCharacInfo::getCurCharacNo

#include "Inter_DispatcherAradOnly/Inter_DispatcherAradOnly.inc"


#include "Inter_DispatcherAradOnly/dtor_Inter_DispatcherAradOnly.inc"


#include "Inter_DispatcherAradOnly/dispatch_sig.inc"

