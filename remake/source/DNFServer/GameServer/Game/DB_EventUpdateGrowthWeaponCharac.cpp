// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 DB_EventUpdateGrowthWeaponCharac/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— DB_EventUpdateGrowthWeaponCharac（成长武器活动角色选择 DB 请求）。
// 逐函数对照 docs/class_func_reports/DB_EventUpdateGrowthWeaponCharac/makeRequest.md
// （ORIG 0x084495c6）。语义：向 DB 队列发一条请求，表头 0x314 + -1，正文为
// accId(uint)、characNo(uint)、addInfo(int)、serverGroup(short)；
// Acquire 行号取 ORIG 常量 "DBThread.cpp" 0xb914。队列 put 的 QUEUE_IDX = 2（DB_Q）。
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "DB_EventUpdateGrowthWeaponCharac.h"

// ============================================================================
// DB_EventUpdateGrowthWeaponCharac::makeRequest @ 0x084495c6
// ============================================================================
#include "DB_EventUpdateGrowthWeaponCharac/makeRequest.inc"
}
