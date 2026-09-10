// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 DB_UpdateGiftCount/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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

// ============================================================================
// DB_UpdateGiftCount::makeRequest ORIG @ 0x084271ec
//   Acquire("DBThread.cpp", 0x6841) | opcode 0xd1 | -1 | accId(uint) | count(int)
// ============================================================================
#include "DB_UpdateGiftCount/makeRequest.inc"
}

// ============================================================================
// DB_UpdateGiftCount::DB_UpdateGiftCount ORIG W C1/C2 @ 0x0844e756：
// 仅调用基类 ctor 并设 vptr —— 与空派生体一致。
// ============================================================================
#include "DB_UpdateGiftCount/DB_UpdateGiftCount.inc"
}

// ============================================================================
// DB_UpdateGiftCount::~DB_UpdateGiftCount ORIG D1 @ 0x0845eac4 / D0 @0x0845eaf4：
// 设 vptr + 析构基类（无成员），delete 分支按 ORIG 同型由编译器展开。
// ============================================================================
#include "DB_UpdateGiftCount/dtor_DB_UpdateGiftCount.inc"
}

// ============================================================================
// DB_UpdateGiftCount::dispatch ORIG T @ 0x08427112 [dispatch.md]
//   read u32 cnt + i32 id → MySQL(6, group==0)：
//   "upDate member_login set last_gift_time=unix_timestamp(now()),
//    gift_cnt=gift_cnt+%d where m_id=%s" % (id, NumberToString(cnt,0))
//   → exec(true)；返回 1。
// ============================================================================
#include "DB_UpdateGiftCount/dispatch.inc"
}

// ============================================================================
// DB_UpdateBreakAwayAccruedCera::makeRequest ORIG @ 0x0843856a
//   Acquire("DBThread.cpp", 0x85fe) | opcode 0x146 | -1 | param_1 | param_2
// ============================================================================
#include "DB_UpdateGiftCount/makeRequest_1.inc"
}
