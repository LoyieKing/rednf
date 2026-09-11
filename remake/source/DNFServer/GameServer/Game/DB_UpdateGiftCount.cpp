// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DB_UpdateGiftCount/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// DB_UpdateGiftCount / DB_UpdateBreakAwayAccruedCera 实现（W7 链接收敛）。
// 逐函数按 class_func_reports/{DB_UpdateGiftCount,DB_UpdateBreakAwayAccruedCera}/
// makeRequest.md、dispatch.md 转写；包构建约定与 DB_AccountCargoSync.cpp 一致：
// StreamPool::Acquire("DBThread.cpp", <行号>) → CStreamGuard → MsgQueueMgr::put(DB_Q)。

#include "DB_UpdateGiftCount.h"
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "DBMgr.h"
#include "MySQL.h"
#include "CInventory.h"      // NumberToString 声明（定义 LinkFinalDeps.cpp）

#include "DB_UpdateGiftCount/makeRequest.inc"


#include "DB_UpdateGiftCount/DB_UpdateGiftCount.inc"


#include "DB_UpdateGiftCount/dtor_DB_UpdateGiftCount.inc"


#include "DB_UpdateGiftCount/dispatch.inc"


#include "DB_UpdateGiftCount/makeRequest_1.inc"

