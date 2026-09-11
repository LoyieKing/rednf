// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CLogGameChannel/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "CLogGameChannel.h"
#include "SigTypes.h"
#include "GlobalData.h"
#include "DBMgr.h"
#include "MySQL.h"
#include "TimerQueue.h"
#include "StreamPool.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "CEnvironment.h"
#include <string.h>

// [A31-GI · P338/P327] 补 <iostream> 恢复 file-scope 动态初始化 __ioinit；GetLogTargetDB
// 定义前移为本 TU 首个强外部符号 ⇒ GI 键名
// `_GLOBAL__I__ZN8WongWork15CLogGameChannel14GetLogTargetDBEv`（ORIG 0x8550763）。
#include <iostream>

// df_game_r 还原 —— WongWork::CLogGameChannel（LogGameChannel 定时落库）。
// 参照 docs/class_func_reports/WongWork__CLogGameChannel/ 反汇编逐函数实现。
// 布局见 CLogGameChannel.h（ctor 0x082a71c4：+0/+4/+8 清零，+0xc period=10）。

namespace WongWork
{

#include "CLogGameChannel/GetLogTargetDB.inc"


#include "CLogGameChannel/CLogGameChannel.inc"


#include "CLogGameChannel/StartLogTimer.inc"


#include "CLogGameChannel/SetNextLogTimer.inc"


#include "CLogGameChannel/IncInUser.inc"


#include "CLogGameChannel/IncOutUser.inc"


#include "CLogGameChannel/ResetInOutUser.inc"


#include "CLogGameChannel/ReqSaveLog.inc"


#include "CLogGameChannel/DoSaveLog.inc"


} // namespace WongWork
