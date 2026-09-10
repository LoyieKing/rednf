// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 OrigMiscStrong21/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// OrigMiscStrong21.cpp — 杂项强补齐（独占 TU，任务 OrigMiscStrong21）
//
// 覆盖符号（ORIG df_game_r nm 核实）：
//   stNPCCommonData_t::getIllustIndex(int)             T 0x089faeee
//   stNPCCommonData_t::stOneBuff_t::~stOneBuff_t()     W 0x08a04b54（D1/D2 同址，空）
// ============================================================================

#include "CDataManager.h"

#include "OrigMiscStrong21/getIllustIndex.inc"
}

#include "OrigMiscStrong21/dtor_stOneBuff_t.inc"
}
