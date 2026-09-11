#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Inter_DispatcherTaiwan/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— Inter_DispatcherTaiwan 实现（报告 docs/class_func_reports/
// Inter_DispatcherTaiwan/）。C1 0x084e8bba：基类 ISigDispatcher() + 设 vptr +
// TaiwanInternalDispatcher C1 + init()；dispatch_sig 0x084e8c2e：读 data 前 4
// 字节 code → getDispatcher → 空 cMyTrace(0x6dca,5)+getCurCharacNo 返 0x6dcb
// → 否则虚调槽2 dispatch_sig(data+4, len-4)。
#include "Inter_DispatcherTaiwan.h"

#include "GameTypes.h"                       // cMyTrace
#include "CUser.h"                           // CUserCharacInfo::getCurCharacNo
#include "TaiwanInterfaceInterDispatcher.h"  // dispatch_sig 虚调用需完整类型

#include "Inter_DispatcherTaiwan/Inter_DispatcherTaiwan.inc"


#include "Inter_DispatcherTaiwan/dispatch_sig.inc"

