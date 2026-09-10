// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 DB_UpdateInGameEventHistory/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— DB_UpdateInGameEventHistory（游戏内事件历史更新 DB 请求，
// DBThread 派发）。
// 逐函数对照 docs/class_func_reports/DB_UpdateInGameEventHistory/makeRequest.md。
// 语义：makeRequest（ORIG 0x08444ac4）向 DB 队列 2 发一条请求，流内容为
// 表头 opcode 0x2ad + 固定 -1 + accountId、eventType、history1、history2；
// 流来源 StreamPool::Acquire("DBThread.cpp", 0xaba8)，guard 持有所有权（RAII 析构归还）。
#include "DB_UpdateInGameEventHistory.h"

#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"

// ============================================================================
// DB_UpdateInGameEventHistory::makeRequest @ 0x08444ac4
// mangled _ZN27DB_UpdateInGameEventHistory11makeRequestEjiii
// ============================================================================
#include "DB_UpdateInGameEventHistory/makeRequest.inc"
}
