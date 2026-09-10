// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 DB_UpdateChuseokPackagePurchase/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— DB_UpdateChuseokPackagePurchase（2010 秋夕包购买计数 DB
// 请求，DBThread 派发）。逐函数对照
// docs/class_func_reports/DB_UpdateChuseokPackagePurchase/{makeRequest,dispatch}.md。
// 语义：makeRequest（0x0843c8b6）向 DB 队列发一条请求，表头 0x213 +
// characNo(-1) + m_id；dispatch（0x0843c7bc）读流取 m_id 后对硬编码表
// event_1009_package_count 执行
// "upDate event_1009_package_count set purchase_cnt = purchase_cnt+1 where
// m_id=%s"（无行则 insert）。与 DB_UpdatePurchaseCashItemBonus 不同，
// ORIG 表名不走 get_db_table_name 且无 select 回发。
#include "DB_UpdateChuseokPackagePurchase.h"

#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "DBMgr.h"
#include "MySQL.h"
#include "SkillSlot.h"          // NumberToString
#include "Stream.h"

// ============================================================================
// DB_UpdateChuseokPackagePurchase::makeRequest @ 0x0843c8b6
// mangled _ZN31DB_UpdateChuseokPackagePurchase11makeRequestEj
// ============================================================================
#include "DB_UpdateChuseokPackagePurchase/makeRequest.inc"
}

// ============================================================================
// DB_UpdateChuseokPackagePurchase::dispatch @ 0x0843c7bc
// mangled _ZN31DB_UpdateChuseokPackagePurchase8dispatchEiiP6Stream
// ============================================================================
#include "DB_UpdateChuseokPackagePurchase/dispatch.inc"
}
