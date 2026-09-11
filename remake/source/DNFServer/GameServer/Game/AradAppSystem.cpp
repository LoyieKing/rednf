// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 AradAppSystem/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— AradAppSystem（启动/日志/脚本装载入口）。
// ORIG 符号（docs/class_func_reports/AradAppSystem/）：
//   AradAppInit(int, char**)   T 0x0817de00
//   script_load()              T 0x0817de9a
//   open_log(char*)            T 0x0817deea
// 逐函数按 ORIG objdump 转写（c6root gcc4.4.7 -m32 -O0）。
// 三方法 ORIG 均按静态成员编译（无 this：AradAppInit 的 argc 在 0x8(%ebp)、
// open_log 的 name 在 0x8(%ebp)、调用点只压显式实参）。

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include "AradAppSystem.h"
#include "CEnvironment.h"   // G_CEnvironment / set_file_name / get_file_name
#include "CDataManager.h"   // G_CDataManager / CDataManager::init
#include "CLog.h"           // CLog::instance / initiate
#include "HistoryLog.h"     // HistoryLog::Init

#include "AradAppSystem/AradAppInit.inc"


#include "AradAppSystem/script_load.inc"


#include "AradAppSystem/open_log.inc"

