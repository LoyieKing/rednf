// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 DB_AradDatabaseDispatcher/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— DB_AradDatabaseDispatcher 实现（报告 docs/class_func_reports/
// DB_AradDatabaseDispatcher/）。C1 0x0844bc38：基类 DBDispatcher() + 设 vptr +
// Arad_DatabaseDispatcher C1 + Init()；dispatch 0x0844bcac：Stream>>code →
// GetDispatcher → 空 cMyTrace(0xbc8b,5) 返 false → 否则虚调槽2 dispatch。
#include "DB_AradDatabaseDispatcher.h"

#include "GameTypes.h"  // cMyTrace

#include "DB_AradDatabaseDispatcher/DB_AradDatabaseDispatcher.inc"
}

#include "DB_AradDatabaseDispatcher/dtor_DB_AradDatabaseDispatcher.inc"
}

// [b28c-32] verdict=NEAR：N7 虚调用 callee 寄存器互换（50=50）。
#include "DB_AradDatabaseDispatcher/dispatch.inc"
}
